#include "PluginProcessor.h"
#include "PluginEditor.h"

VocalHarmonizerProcessor::VocalHarmonizerProcessor()
    : AudioProcessor(BusesProperties()
                        .withInput("Input", juce::AudioChannelSet::stereo(), true)
                        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "Parameters", createParameterLayout()),
      presetManager_(apvts_, "VocalHarmonizer") {
}

VocalHarmonizerProcessor::~VocalHarmonizerProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout VocalHarmonizerProcessor::createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Voice count
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        PARAM_VOICES,
        "Voices",
        1, 4,
        2));

    // Voice pitch shifts
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_VOICE1_PITCH,
        "Voice 1 Pitch",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
        3.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_VOICE2_PITCH,
        "Voice 2 Pitch",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
        7.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_VOICE3_PITCH,
        "Voice 3 Pitch",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
        12.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_VOICE4_PITCH,
        "Voice 4 Pitch",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
        -12.0f));

    // Voice panning
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_VOICE1_PAN,
        "Voice 1 Pan",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 1.0f),
        -50.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_VOICE2_PAN,
        "Voice 2 Pan",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 1.0f),
        50.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_VOICE3_PAN,
        "Voice 3 Pan",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 1.0f),
        -25.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_VOICE4_PAN,
        "Voice 4 Pan",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 1.0f),
        25.0f));

    // Scale
    juce::StringArray scaleNames;
    scaleNames.add("Chromatic");
    scaleNames.add("Major");
    scaleNames.add("Minor");
    scaleNames.add("Dorian");
    scaleNames.add("Phrygian");
    scaleNames.add("Lydian");
    scaleNames.add("Mixolydian");
    scaleNames.add("Aeolian");
    scaleNames.add("Locrian");
    scaleNames.add("Harmonic Minor");
    scaleNames.add("Melodic Minor");
    scaleNames.add("Pentatonic");

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        PARAM_SCALE,
        "Scale",
        scaleNames,
        0)); // Chromatic

    // Formant shift
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_FORMANT,
        "Formant Shift",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f));

    // Filters
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

    // Mix
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_MIX,
        "Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        50.0f));

    // Output gain
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_GAIN,
        "Output Gain",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f));

    return { params.begin(), params.end() };
}

void VocalHarmonizerProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    harmonizer_.prepare(sampleRate, samplesPerBlock);
}

void VocalHarmonizerProcessor::releaseResources() {
    harmonizer_.reset();
}

void VocalHarmonizerProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) {
    juce::ScopedNoDenormals noDenormals;

    // Update parameters
    harmonizer_.setVoiceCount(static_cast<int>(*apvts_.getRawParameterValue(PARAM_VOICES)));

    harmonizer_.setVoicePitch(0, *apvts_.getRawParameterValue(PARAM_VOICE1_PITCH));
    harmonizer_.setVoicePitch(1, *apvts_.getRawParameterValue(PARAM_VOICE2_PITCH));
    harmonizer_.setVoicePitch(2, *apvts_.getRawParameterValue(PARAM_VOICE3_PITCH));
    harmonizer_.setVoicePitch(3, *apvts_.getRawParameterValue(PARAM_VOICE4_PITCH));

    harmonizer_.setVoicePan(0, *apvts_.getRawParameterValue(PARAM_VOICE1_PAN));
    harmonizer_.setVoicePan(1, *apvts_.getRawParameterValue(PARAM_VOICE2_PAN));
    harmonizer_.setVoicePan(2, *apvts_.getRawParameterValue(PARAM_VOICE3_PAN));
    harmonizer_.setVoicePan(3, *apvts_.getRawParameterValue(PARAM_VOICE4_PAN));

    int scaleIndex = static_cast<int>(*apvts_.getRawParameterValue(PARAM_SCALE));
    harmonizer_.setScale(static_cast<HarmonizerEngine::Scale>(scaleIndex));

    harmonizer_.setFormantShift(*apvts_.getRawParameterValue(PARAM_FORMANT));
    harmonizer_.setHighPass(*apvts_.getRawParameterValue(PARAM_HIGHPASS));
    harmonizer_.setLowPass(*apvts_.getRawParameterValue(PARAM_LOWPASS));
    harmonizer_.setMix(*apvts_.getRawParameterValue(PARAM_MIX) / 100.0f);
    harmonizer_.setOutputGain(*apvts_.getRawParameterValue(PARAM_GAIN));

    // Process
    harmonizer_.process(buffer);
}

juce::AudioProcessorEditor* VocalHarmonizerProcessor::createEditor() {
    return new VocalHarmonizerEditor(*this);
}

void VocalHarmonizerProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto state = apvts_.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void VocalHarmonizerProcessor::setStateInformation(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState && xmlState->hasTagName(apvts_.state.getType()))
        apvts_.replaceState(juce::ValueTree::fromXml(*xmlState));
}

// Create plugin instance
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new VocalHarmonizerProcessor();
}
