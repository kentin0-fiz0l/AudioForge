#include "PluginProcessor.h"
#include "PluginEditor.h"

AIProducerProcessor::AIProducerProcessor()
    : AudioProcessor(BusesProperties()
                        .withInput("Input", juce::AudioChannelSet::stereo(), true)
                        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      aiClient_(std::make_unique<AIClient>(AIClient::Provider::OllamaLocal)),
      midiGenerator_(std::make_unique<MIDIGenerator>()) {
}

AIProducerProcessor::~AIProducerProcessor() {}

void AIProducerProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    lastSampleRate_ = sampleRate;
}

void AIProducerProcessor::releaseResources() {}

void AIProducerProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    buffer.clear();

    // If we have generated MIDI, output it
    if (midiGenerator_->isReady()) {
        midiGenerator_->fillMIDIBuffer(midiMessages,
                                      0,
                                      buffer.getNumSamples(),
                                      lastSampleRate_,
                                      currentBPM_);
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

    currentBPM_ = structure.bpm;
    midiGenerator_->generateFromStructure(structure);

    isGenerating_ = false;
    statusMessage_ = "Track generated! Press play in your DAW.";
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
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new AIProducerProcessor();
}
