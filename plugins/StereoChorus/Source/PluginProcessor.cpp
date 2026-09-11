#include "PluginProcessor.h"
#include "PluginEditor.h"

static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterFloat>("rate", "Rate", juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, 0.3f), 1.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("depth", "Depth", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("centreDelay", "Center Delay", juce::NormalisableRange<float>(1.0f, 100.0f, 0.1f), 7.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("feedback", "Feedback", -0.95f, 0.95f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("mix", "Mix", 0.0f, 1.0f, 0.5f));
    return {params.begin(), params.end()};
}

ChorusProcessor::ChorusProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()),
      presetManager_(apvts_, "StereoChorus") {

    // Scan for presets on startup
    presetManager_.scanPresets();
}

ChorusProcessor::~ChorusProcessor() {}

void ChorusProcessor::prepareToPlay(double sr, int sb) {
    juce::dsp::ProcessSpec spec{sr, static_cast<juce::uint32>(sb), 2};
    chorus_.prepare(spec);
}

void ChorusProcessor::processBlock(juce::AudioBuffer<float>& buf, juce::MidiBuffer& midi) {
    juce::ScopedNoDenormals noDenormals;
    for (const auto metadata : midi)
        midiLearnManager_.processMidiMessage(metadata.getMessage());
    updateChorusParameters();
    juce::dsp::AudioBlock<float> block(buf);
    juce::dsp::ProcessContextReplacing<float> context(block);
    chorus_.process(context);
}

void ChorusProcessor::updateChorusParameters() {
    chorus_.setRate(apvts_.getRawParameterValue("rate")->load());
    chorus_.setDepth(apvts_.getRawParameterValue("depth")->load());
    chorus_.setCentreDelay(apvts_.getRawParameterValue("centreDelay")->load());
    chorus_.setFeedback(apvts_.getRawParameterValue("feedback")->load());
    chorus_.setMix(apvts_.getRawParameterValue("mix")->load());
}

juce::AudioProcessorEditor* ChorusProcessor::createEditor() {
    return new ChorusEditor(*this);
}

void ChorusProcessor::getStateInformation(juce::MemoryBlock& d) {
    auto s = apvts_.copyState(); std::unique_ptr<juce::XmlElement> x(s.createXml()); copyXmlToBinary(*x, d);
}

void ChorusProcessor::setStateInformation(const void* d, int sz) {
    std::unique_ptr<juce::XmlElement> x(getXmlFromBinary(d, sz));
    if (x && x->hasTagName(apvts_.state.getType())) apvts_.replaceState(juce::ValueTree::fromXml(*x));
        if (auto* midiXml = x->getChildByName("MIDILearnMappings"))
            midiLearnManager_.loadFromXml(*midiXml);
        if (auto* presetXml = xml->getChildByName("PresetManagerState"))
            presetManager_.loadFromXml(*presetXml);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new ChorusProcessor();
}
