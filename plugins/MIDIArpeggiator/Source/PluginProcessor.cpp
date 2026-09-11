#include "PluginProcessor.h"
#include "PluginEditor.h"

static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "pattern", "Pattern",
        juce::StringArray{"Up", "Down", "Up-Down", "Down-Up", "Random", "Played Order"},
        0));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "rate", "Rate",
        juce::StringArray{"1/1", "1/2", "1/4", "1/8", "1/16", "1/32", "1/4T", "1/8T", "1/16T"},
        3)); // Default to 1/8

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "octaves", "Octaves", 1, 4, 1));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "gate", "Gate", 0.0f, 1.0f, 0.8f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "swing", "Swing", 0.0f, 1.0f, 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "latch", "Latch", false));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "useInputVel", "Use Input Velocity", true));

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "fixedVel", "Fixed Velocity", 1, 127, 100));

    return {params.begin(), params.end()};
}

MIDIArpeggiatorProcessor::MIDIArpeggiatorProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()),
      presetManager_(apvts_, "MIDIArpeggiator") {

    // Scan for presets on startup
    presetManager_.scanPresets();
}

MIDIArpeggiatorProcessor::~MIDIArpeggiatorProcessor() {}

void MIDIArpeggiatorProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    engine_.prepareToPlay(sampleRate, samplesPerBlock);
}

void MIDIArpeggiatorProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                             juce::MidiBuffer& midiMessages) {
    // Process MIDI learn
    for (const auto metadata : midiMessages)
        midiLearnManager_.processMidiMessage(metadata.getMessage());

    // Update engine with current parameters
    updateEngineParameters();

    // Get playhead info for tempo sync
    auto posInfo = getPlayHead()->getPosition();
    if (posInfo.hasValue()) {
        engine_.processBlock(midiMessages, buffer.getNumSamples(), *posInfo);
    }

    // Clear audio buffer (MIDI effect, no audio processing)
    buffer.clear();
}

void MIDIArpeggiatorProcessor::updateEngineParameters() {
    int patternIdx = apvts_.getRawParameterValue("pattern")->load();
    engine_.setPattern(static_cast<ArpEngine::Pattern>(patternIdx));

    int rateIdx = apvts_.getRawParameterValue("rate")->load();
    engine_.setRate(static_cast<ArpEngine::Rate>(rateIdx));

    engine_.setOctaves((int)apvts_.getRawParameterValue("octaves")->load());
    engine_.setGate(apvts_.getRawParameterValue("gate")->load());
    engine_.setSwing(apvts_.getRawParameterValue("swing")->load());
    engine_.setLatch(apvts_.getRawParameterValue("latch")->load() > 0.5f);
    engine_.setVelocityMode(apvts_.getRawParameterValue("useInputVel")->load() > 0.5f);
    engine_.setFixedVelocity((int)apvts_.getRawParameterValue("fixedVel")->load());
}

juce::AudioProcessorEditor* MIDIArpeggiatorProcessor::createEditor() {
    return new MIDIArpeggiatorEditor(*this);
}

void MIDIArpeggiatorProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto state = apvts_.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    xml->addChildElement(midiLearnManager_.saveToXml().release());
    xml->addChildElement(presetManager_.saveToXml().release());
    copyXmlToBinary(*xml, destData);
}

void MIDIArpeggiatorProcessor::setStateInformation(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(apvts_.state.getType())) {
        apvts_.replaceState(juce::ValueTree::fromXml(*xml));
        if (auto* midiXml = xml->getChildByName("MIDILearnMappings"))
            midiLearnManager_.loadFromXml(*midiXml);
        if (auto* presetXml = xml->getChildByName("PresetManagerState"))
            presetManager_.loadFromXml(*presetXml);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new MIDIArpeggiatorProcessor();
}
