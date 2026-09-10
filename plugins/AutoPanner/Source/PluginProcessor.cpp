#include "PluginProcessor.h"
#include "PluginEditor.h"

static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterChoice>("waveform", "Waveform", juce::StringArray{"Sine", "Triangle", "Square"}, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("rate", "Rate", juce::NormalisableRange<float>(0.01f, 20.0f, 0.01f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("depth", "Depth", 0.0f, 1.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("phaseOffset", "Phase Offset", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("stereoWidth", "Stereo Width", 0.0f, 2.0f, 1.0f));
    return {params.begin(), params.end()};
}

AutoPannerProcessor::AutoPannerProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()) {}

AutoPannerProcessor::~AutoPannerProcessor() {}

void AutoPannerProcessor::prepareToPlay(double sr, int sb) { engine_.prepareToPlay(sr, sb); }

void AutoPannerProcessor::processBlock(juce::AudioBuffer<float>& buf, juce::MidiBuffer&) {
    juce::ScopedNoDenormals noDenormals;
    updateEngineParameters();
    engine_.processBlock(buf);
}

void AutoPannerProcessor::updateEngineParameters() {
    engine_.setWaveform(apvts_.getRawParameterValue("waveform")->load());
    engine_.setRate(apvts_.getRawParameterValue("rate")->load());
    engine_.setDepth(apvts_.getRawParameterValue("depth")->load());
    engine_.setPhaseOffset(apvts_.getRawParameterValue("phaseOffset")->load());
    engine_.setStereoWidth(apvts_.getRawParameterValue("stereoWidth")->load());
}

juce::AudioProcessorEditor* AutoPannerProcessor::createEditor() { return new AutoPannerEditor(*this); }

void AutoPannerProcessor::getStateInformation(juce::MemoryBlock& d) {
    auto s = apvts_.copyState(); std::unique_ptr<juce::XmlElement> x(s.createXml()); copyXmlToBinary(*x, d);
}

void AutoPannerProcessor::setStateInformation(const void* d, int sz) {
    std::unique_ptr<juce::XmlElement> x(getXmlFromBinary(d, sz));
    if (x && x->hasTagName(apvts_.state.getType())) apvts_.replaceState(juce::ValueTree::fromXml(*x));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new AutoPannerProcessor(); }
