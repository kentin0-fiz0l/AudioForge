#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace {

juce::StringArray toStringArray(const std::vector<std::string>& names) {
    juce::StringArray array;
    for (const auto& name : names) array.add(name);
    return array;
}

constexpr int kMaxVariation = 100;

// A host's reported position can differ from where the last block ended by
// rounding or tempo automation; anything beyond this is a loop or a seek.
constexpr double kJumpToleranceInQuarterNotes = 1.0e-3;


juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    const Styles::Settings defaults;
    params.push_back(std::make_unique<juce::AudioParameterChoice>("genre", "Genre", toStringArray(Styles::genreNames()), defaults.genre));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("key", "Key", toStringArray(Styles::keyNames()), defaults.keyRoot));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("scale", "Scale", juce::StringArray{"Minor", "Major"}, 0));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("length", "Length", toStringArray(Styles::lengthNames()), defaults.length));
    params.push_back(std::make_unique<juce::AudioParameterBool>("drums", "Drums", true));
    params.push_back(std::make_unique<juce::AudioParameterBool>("bass", "Bass", true));
    params.push_back(std::make_unique<juce::AudioParameterBool>("chords", "Chords", true));
    params.push_back(std::make_unique<juce::AudioParameterBool>("lead", "Lead", true));
    params.push_back(std::make_unique<juce::AudioParameterInt>("variation", "Variation", 1, kMaxVariation, defaults.variation));
    return {params.begin(), params.end()};
}

} // namespace

AIProducerProcessor::AIProducerProcessor()
    : AudioProcessor(BusesProperties()
                        .withInput("Input", juce::AudioChannelSet::stereo(), true)
                        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()) {
    regenerateIfChanged(); // A track is ready as soon as the plugin loads
    startTimerHz(5);       // Then follow the controls, including host automation
}

AIProducerProcessor::~AIProducerProcessor() {
    stopTimer();
}

void AIProducerProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/) {
    lastSampleRate_ = sampleRate;
    wasPlaying_ = false;
}

void AIProducerProcessor::releaseResources() {}

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
        if (wasPlaying_)
            heldNotes_.releaseAll(midiMessages, 0); // Transport stopped: silence what is sounding
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

Styles::Settings AIProducerProcessor::currentSettings() const {
    auto value = [this](const char* id) { return juce::roundToInt(apvts_.getRawParameterValue(id)->load()); };

    Styles::Settings settings;
    settings.genre = value("genre");
    settings.keyRoot = value("key");
    settings.minor = value("scale") == 0;
    settings.length = value("length");
    settings.drums = value("drums") != 0;
    settings.bass = value("bass") != 0;
    settings.chords = value("chords") != 0;
    settings.lead = value("lead") != 0;
    settings.variation = value("variation");
    return settings;
}

void AIProducerProcessor::regenerateIfChanged() {
    const juce::ScopedLock lock(buildLock_);

    const auto settings = currentSettings();
    if (hasBuilt_ && settings == builtSettings_) return;

    const auto structure = Styles::build(settings);
    auto sequence = MIDIGenerator::generateFromStructure(structure);
    description_ = Styles::describe(structure, sequence->totalBars);
    publishSequence(std::move(sequence));

    builtSettings_ = settings;
    hasBuilt_ = true;
}

void AIProducerProcessor::nextVariation() {
    if (auto* variation = apvts_.getParameter("variation")) {
        const int next = currentSettings().variation % kMaxVariation + 1;
        variation->setValueNotifyingHost(variation->convertTo0to1(static_cast<float>(next)));
    }
    regenerateIfChanged();
}

juce::String AIProducerProcessor::getDescription() const {
    const juce::ScopedLock lock(buildLock_);
    return description_;
}

juce::AudioProcessorEditor* AIProducerProcessor::createEditor() {
    return new AIProducerEditor(*this);
}

void AIProducerProcessor::getStateInformation(juce::MemoryBlock& destData) {
    if (auto xml = apvts_.copyState().createXml())
        copyXmlToBinary(*xml, destData);
}

void AIProducerProcessor::setStateInformation(const void* data, int sizeInBytes) {
    // Anything that is not our own saved state is ignored. Parameter values
    // are clamped to their ranges as they are read in.
    auto xml = getXmlFromBinary(data, sizeInBytes);
    if (xml == nullptr || !xml->hasTagName(apvts_.state.getType())) return;

    apvts_.replaceState(juce::ValueTree::fromXml(*xml));
    regenerateIfChanged();
}

// Create plugin instance
#ifndef AUDIOFORGE_TESTS
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new AIProducerProcessor();
}
#endif
