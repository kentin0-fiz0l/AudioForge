#include "PluginProcessor.h"
#include "PluginEditor.h"

static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "gridResolution", "Grid",
        juce::StringArray{"1/4", "1/8", "1/16", "1/32", "1/8T", "1/16T"},
        2)); // Default to 1/16

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "quantizeStrength", "Strength", 0.0f, 1.0f, 1.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "swing", "Swing", 0.0f, 1.0f, 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "humanizeTiming", "Humanize Time", 0.0f, 1.0f, 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "humanizeVelocity", "Humanize Vel", 0.0f, 1.0f, 0.0f));

    return {params.begin(), params.end()};
}

MIDIGrooveQuantizerProcessor::MIDIGrooveQuantizerProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()),
      midiLearnManager_(apvts_),
      presetManager_(apvts_, "MIDIGrooveQuantizer") {

    // Scan for presets on startup
    presetManager_.scanPresets();
}

MIDIGrooveQuantizerProcessor::~MIDIGrooveQuantizerProcessor() {}

void MIDIGrooveQuantizerProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    engine_.prepareToPlay(sampleRate);
}

void MIDIGrooveQuantizerProcessor::processBlock(juce::AudioBuffer<float>& buffer,
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

void MIDIGrooveQuantizerProcessor::updateEngineParameters() {
    int gridIdx = apvts_.getRawParameterValue("gridResolution")->load();
    engine_.setGridResolution(static_cast<QuantizerEngine::GridResolution>(gridIdx));

    engine_.setQuantizeStrength(apvts_.getRawParameterValue("quantizeStrength")->load());
    engine_.setSwing(apvts_.getRawParameterValue("swing")->load());
    engine_.setHumanizeTiming(apvts_.getRawParameterValue("humanizeTiming")->load());
    engine_.setHumanizeVelocity(apvts_.getRawParameterValue("humanizeVelocity")->load());
}

juce::AudioProcessorEditor* MIDIGrooveQuantizerProcessor::createEditor() {
    return new MIDIGrooveQuantizerEditor(*this);
}

void MIDIGrooveQuantizerProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto state = apvts_.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    xml->addChildElement(midiLearnManager_.saveToXml().release());
    xml->addChildElement(presetManager_.saveToXml().release());
    copyXmlToBinary(*xml, destData);
}

void MIDIGrooveQuantizerProcessor::setStateInformation(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(apvts_.state.getType())) {
        apvts_.replaceState(juce::ValueTree::fromXml(*xml));
        if (auto* midiXml = xml->getChildByName("MIDILearnMappings"))
            midiLearnManager_.loadFromXml(*midiXml);
        if (auto* presetXml = xmlState->getChildByName("PresetManagerState"))
            presetManager_.loadFromXml(*presetXml);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new MIDIGrooveQuantizerProcessor();
}
