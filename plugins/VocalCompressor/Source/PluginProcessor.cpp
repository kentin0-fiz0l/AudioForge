#include "PluginProcessor.h"
#include "PluginEditor.h"

VocalCompressorProcessor::VocalCompressorProcessor()
    : AudioProcessor(BusesProperties()
                        .withInput("Input", juce::AudioChannelSet::stereo(), true)
                        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "Parameters", createParameterLayout()),
      presetManager_(apvts_, "VocalCompressor") {
}

VocalCompressorProcessor::~VocalCompressorProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout VocalCompressorProcessor::createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_THRESHOLD,
        "Threshold",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f),
        -18.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_RATIO,
        "Ratio",
        juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f),
        4.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_ATTACK,
        "Attack",
        juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f, 0.5f),
        5.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_RELEASE,
        "Release",
        juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f, 0.5f),
        100.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_KNEE,
        "Knee",
        juce::NormalisableRange<float>(0.0f, 12.0f, 0.1f),
        3.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_MAKEUP,
        "Makeup Gain",
        juce::NormalisableRange<float>(-12.0f, 24.0f, 0.1f),
        0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_PARALLEL_MIX,
        "Parallel Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        0.0f));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        PARAM_DETECTION,
        "Detection",
        juce::StringArray{"RMS", "Peak"},
        0)); // 0 = RMS

    return { params.begin(), params.end() };
}

void VocalCompressorProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    compressor_.prepare(sampleRate, samplesPerBlock);
}

void VocalCompressorProcessor::releaseResources() {
    compressor_.reset();
}

void VocalCompressorProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) {
    juce::ScopedNoDenormals noDenormals;

    // Update parameters
    compressor_.setThreshold(*apvts_.getRawParameterValue(PARAM_THRESHOLD));
    compressor_.setRatio(*apvts_.getRawParameterValue(PARAM_RATIO));
    compressor_.setAttack(*apvts_.getRawParameterValue(PARAM_ATTACK));
    compressor_.setRelease(*apvts_.getRawParameterValue(PARAM_RELEASE));
    compressor_.setKnee(*apvts_.getRawParameterValue(PARAM_KNEE));
    compressor_.setMakeupGain(*apvts_.getRawParameterValue(PARAM_MAKEUP));
    compressor_.setParallelMix(*apvts_.getRawParameterValue(PARAM_PARALLEL_MIX) / 100.0f);

    // Detection mode (0 = RMS, 1 = Peak)
    int detectionMode = static_cast<int>(*apvts_.getRawParameterValue(PARAM_DETECTION));
    compressor_.setDetectionMode(detectionMode == 1); // true = Peak

    // Process
    compressor_.process(buffer);
}

juce::AudioProcessorEditor* VocalCompressorProcessor::createEditor() {
    return new VocalCompressorEditor(*this);
}

void VocalCompressorProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto state = apvts_.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void VocalCompressorProcessor::setStateInformation(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState && xmlState->hasTagName(apvts_.state.getType()))
        apvts_.replaceState(juce::ValueTree::fromXml(*xmlState));
}

// Create plugin instance
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new VocalCompressorProcessor();
}
