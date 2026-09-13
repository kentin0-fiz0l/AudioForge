#include "PluginProcessor.h"
#include "PluginEditor.h"

VocalDoublerProcessor::VocalDoublerProcessor()
    : AudioProcessor(BusesProperties()
                        .withInput("Input", juce::AudioChannelSet::stereo(), true)
                        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "Parameters", createParameterLayout()),
      presetManager_(apvts_, "VocalDoubler") {
}

VocalDoublerProcessor::~VocalDoublerProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout VocalDoublerProcessor::createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        PARAM_VOICES,
        "Voices",
        1, 4,
        2));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_WIDTH,
        "Stereo Width",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        50.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_PITCH_VAR,
        "Pitch Variation",
        juce::NormalisableRange<float>(0.0f, 20.0f, 0.1f),
        10.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_TIMING,
        "Timing Shift",
        juce::NormalisableRange<float>(0.0f, 30.0f, 0.1f),
        15.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_HUMANIZE,
        "Humanize",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        50.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_HIGHPASS,
        "High Pass",
        juce::NormalisableRange<float>(20.0f, 500.0f, 1.0f, 0.3f),
        100.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_LOWPASS,
        "Low Pass",
        juce::NormalisableRange<float>(2000.0f, 20000.0f, 10.0f, 0.5f),
        12000.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_MIX,
        "Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        50.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_GAIN,
        "Output Gain",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f));

    return { params.begin(), params.end() };
}

void VocalDoublerProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    doubler_.prepare(sampleRate, samplesPerBlock);
}

void VocalDoublerProcessor::releaseResources() {
    doubler_.reset();
}

void VocalDoublerProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) {
    juce::ScopedNoDenormals noDenormals;

    // Update parameters
    doubler_.setVoiceCount(static_cast<int>(*apvts_.getRawParameterValue(PARAM_VOICES)));
    doubler_.setWidth(*apvts_.getRawParameterValue(PARAM_WIDTH));
    doubler_.setPitchVariation(*apvts_.getRawParameterValue(PARAM_PITCH_VAR));
    doubler_.setTimingShift(*apvts_.getRawParameterValue(PARAM_TIMING));
    doubler_.setHumanize(*apvts_.getRawParameterValue(PARAM_HUMANIZE));
    doubler_.setHighPass(*apvts_.getRawParameterValue(PARAM_HIGHPASS));
    doubler_.setLowPass(*apvts_.getRawParameterValue(PARAM_LOWPASS));
    doubler_.setMix(*apvts_.getRawParameterValue(PARAM_MIX) / 100.0f);
    doubler_.setOutputGain(*apvts_.getRawParameterValue(PARAM_GAIN));

    // Process
    doubler_.process(buffer);
}

juce::AudioProcessorEditor* VocalDoublerProcessor::createEditor() {
    return new VocalDoublerEditor(*this);
}

void VocalDoublerProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto state = apvts_.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void VocalDoublerProcessor::setStateInformation(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState && xmlState->hasTagName(apvts_.state.getType()))
        apvts_.replaceState(juce::ValueTree::fromXml(*xmlState));
}

// Create plugin instance
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new VocalDoublerProcessor();
}
