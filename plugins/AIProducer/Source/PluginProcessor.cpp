#include "PluginProcessor.h"
#include "PluginEditor.h"

AIProducerProcessor::AIProducerProcessor()
    : AudioProcessor(BusesProperties()
                        .withInput("Input", juce::AudioChannelSet::stereo(), true)
                        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      aiClient_(std::make_unique<AIClient>(AIClient::Provider::OllamaLocal)) {
}

AIProducerProcessor::~AIProducerProcessor() {}

void AIProducerProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/) {
    lastSampleRate_ = sampleRate;
    wasPlaying_ = false;
}

void AIProducerProcessor::releaseResources() {}

namespace {

// A host's reported position can differ from where the last block ended by
// rounding or tempo automation; anything beyond this is a loop or a seek.
constexpr double kJumpToleranceInQuarterNotes = 1.0e-3;

} // namespace

void AIProducerProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    buffer.clear();
    midiMessages.clear(); // This plugin only outputs the MIDI it generates

    const int numSamples = buffer.getNumSamples();

    // Follow the host transport: the track is laid out along the song
    // timeline, with bar 1 at PPQ 0
    bool isPlaying = false;
    double blockStartPpq = 0.0;
    double bpm = 120.0;

    if (auto* playHead = getPlayHead()) {
        if (auto position = playHead->getPosition()) {
            isPlaying = position->getIsPlaying();
            blockStartPpq = position->getPpqPosition().orFallback(0.0);
            bpm = position->getBpm().orFallback(120.0);
        }
    }

    if (!isPlaying) {
        if (wasPlaying_) {
            heldNotes_.releaseAll(midiMessages, 0); // Transport stopped: silence what is sounding
        }
        wasPlaying_ = false;
        return;
    }

    const double ppqPerSample = bpm / (60.0 * lastSampleRate_);
    const double blockEndPpq = blockStartPpq + numSamples * ppqPerSample;

    // Starting, looping or seeking: release sounding notes and pick up from here
    const bool isContinuous = wasPlaying_
                              && std::abs(blockStartPpq - nextBlockStartPpq_) < kJumpToleranceInQuarterNotes;
    if (!isContinuous) {
        heldNotes_.releaseAll(midiMessages, 0);
        renderedUpToPpq_ = blockStartPpq;
    }

    wasPlaying_ = true;
    nextBlockStartPpq_ = blockEndPpq;

    // Try, never wait: if the message thread is mid-swap in publishSequence()
    // we skip this block, and because renderedUpToPpq_ is not advanced the
    // skipped range is caught up on the next one. Holding the lock until the
    // end of the function keeps the sequence alive while we read it.
    const juce::SpinLock::ScopedTryLockType lock(sequenceLock_);
    const MIDIGenerator::Sequence* sequence = lock.isLocked() ? sequence_.get() : nullptr;

    if (sequence == nullptr) return;

    // A different track from the one we were playing: start it cleanly
    if (sequence->id != playingSequenceId_) {
        heldNotes_.releaseAll(midiMessages, 0);
        playingSequenceId_ = sequence->id;
        renderedUpToPpq_ = blockStartPpq;
    }

    MIDIGenerator::renderRange(*sequence, midiMessages, heldNotes_,
                               renderedUpToPpq_, blockEndPpq,
                               blockStartPpq, ppqPerSample, numSamples);
    renderedUpToPpq_ = blockEndPpq;
}

void AIProducerProcessor::publishSequence(std::unique_ptr<const MIDIGenerator::Sequence> next) {
    // Swap under the lock, but only swap: the audio thread's try-lock fails for
    // as long as we hold it. The old sequence is now in `next` and is freed
    // when this function returns, here on the message thread.
    {
        const juce::SpinLock::ScopedLockType lock(sequenceLock_);
        std::swap(sequence_, next);
    }
}

void AIProducerProcessor::generateTrack(const juce::String& prompt) {
    if (isGenerating_) return;

    isGenerating_ = true;
    statusMessage_ = "Generating track...";
    progress_ = 0.0f;

    // For MVP: Generate a hardcoded house track structure
    // Later: Use AI to generate based on prompt
    TrackStructure structure;
    structure.genre = "House";
    structure.bpm = 128;
    structure.key = "Am";
    structure.scale = "Minor";
    structure.progression = "i-VI-III-VII";  // Minor house progression

    // Create arrangement: Intro -> Build -> Drop -> Break -> Build -> Drop -> Outro
    structure.sections.push_back({"intro", 0, 8, 3});      // 8 bars, low energy
    structure.sections.push_back({"build", 8, 8, 6});      // 8 bars, building
    structure.sections.push_back({"drop", 16, 16, 10});    // 16 bars, peak energy
    structure.sections.push_back({"breakdown", 32, 8, 4}); // 8 bars, calm
    structure.sections.push_back({"build", 40, 8, 7});     // 8 bars, building again
    structure.sections.push_back({"drop", 48, 16, 10});    // 16 bars, peak
    structure.sections.push_back({"outro", 64, 8, 2});     // 8 bars, fade out

    structure.hasDrums = true;
    structure.hasBass = true;
    structure.hasChords = true;
    structure.hasLead = true;
    structure.hasPads = false;

    structure.styleNotes = "Classic progressive house with energetic drops";

    // Generate MIDI
    onTrackGenerated(structure);
}

void AIProducerProcessor::onTrackGenerated(TrackStructure structure) {
    statusMessage_ = "Generating MIDI...";
    progress_ = 0.5f;

    auto sequence = MIDIGenerator::generateFromStructure(structure);
    const int totalBars = sequence->totalBars;
    publishSequence(std::move(sequence));

    // Playback follows the host's tempo, so the track only sounds as intended
    // when the DAW is set to the tempo it was written for
    isGenerating_ = false;
    statusMessage_ = "Track ready: " + juce::String(totalBars) + " bars at "
                     + juce::String(structure.bpm) + " BPM. Set your DAW's tempo to match, then press play from bar 1.";
    progress_ = 1.0f;
}

void AIProducerProcessor::onGenerationError(std::string error) {
    isGenerating_ = false;
    statusMessage_ = "Error: " + juce::String(error);
    progress_ = 0.0f;
}

juce::AudioProcessorEditor* AIProducerProcessor::createEditor() {
    return new AIProducerEditor(*this);
}

void AIProducerProcessor::getStateInformation(juce::MemoryBlock& destData) {
    // Save state (for now, nothing to save)
}

void AIProducerProcessor::setStateInformation(const void* data, int sizeInBytes) {
    // Load state
}

// Create plugin instance
#ifndef AUDIOFORGE_TESTS
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new AIProducerProcessor();
}
#endif
