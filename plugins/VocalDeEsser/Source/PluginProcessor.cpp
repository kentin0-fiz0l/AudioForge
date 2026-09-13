#include "PluginProcessor.h"
#include "PluginEditor.h"

VocalDeEsserProcessor::VocalDeEsserProcessor()
    : AudioProcessor(BusesProperties()
                        .withInput("Input", juce::AudioChannelSet::stereo(), true)
                        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "Parameters", createParameterLayout()),
      presetManager_(apvts_, "VocalDeEsser") {
}

VocalDeEsserProcessor::~VocalDeEsserProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout VocalDeEsserProcessor::createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_FREQUENCY,
        "Frequency",
        juce::NormalisableRange<float>(4000.0f, 10000.0f, 1.0f),
        7000.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_BANDWIDTH,
        "Bandwidth",
        juce::NormalisableRange<float>(0.5f, 4.0f, 0.1f),
        1.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_THRESHOLD,
        "Threshold",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f),
        -20.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_RATIO,
        "Ratio",
        juce::NormalisableRange<float>(1.0f, 10.0f, 0.1f),
        4.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_ATTACK,
        "Attack",
        juce::NormalisableRange<float>(0.1f, 10.0f, 0.1f, 0.5f),
        0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_RELEASE,
        "Release",
        juce::NormalisableRange<float>(10.0f, 500.0f, 1.0f, 0.5f),
        100.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_MIX,
        "Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        100.0f));

    return { params.begin(), params.end() };
}

void VocalDeEsserProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    deEsser_.prepare(sampleRate, samplesPerBlock);
}

void VocalDeEsserProcessor::releaseResources() {
    deEsser_.reset();
}

void VocalDeEsserProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) {
    juce::ScopedNoDenormals noDenormals;

    // Update parameters
    deEsser_.setFrequency(*apvts_.getRawParameterValue(PARAM_FREQUENCY));
    deEsser_.setBandwidth(*apvts_.getRawParameterValue(PARAM_BANDWIDTH));
    deEsser_.setThreshold(*apvts_.getRawParameterValue(PARAM_THRESHOLD));
    deEsser_.setRatio(*apvts_.getRawParameterValue(PARAM_RATIO));
    deEsser_.setAttack(*apvts_.getRawParameterValue(PARAM_ATTACK));
    deEsser_.setRelease(*apvts_.getRawParameterValue(PARAM_RELEASE));
    deEsser_.setMix(*apvts_.getRawParameterValue(PARAM_MIX) / 100.0f);

    // Process
    deEsser_.process(buffer);
}

juce::AudioProcessorEditor* VocalDeEsserProcessor::createEditor() {
    return new VocalDeEsserEditor(*this);
}

void VocalDeEsserProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto state = apvts_.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void VocalDeEsserProcessor::setStateInformation(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState && xmlState->hasTagName(apvts_.state.getType()))
        apvts_.replaceState(juce::ValueTree::fromXml(*xmlState));
}

// Create plugin instance
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new VocalDeEsserProcessor();
}
