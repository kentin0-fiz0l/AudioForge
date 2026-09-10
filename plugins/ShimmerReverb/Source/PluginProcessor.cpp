#include "PluginProcessor.h"
#include "PluginEditor.h"
static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterFloat>("size", "Size", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("damping", "Damping", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("shimmer", "Shimmer", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("pitchMode", "Pitch", juce::StringArray{"Up", "Down", "Both"}, 2));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("feedback", "Feedback", 0.0f, 0.85f, 0.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("mix", "Mix", 0.0f, 1.0f, 0.5f));
    return {params.begin(), params.end()};
}
ShimmerProcessor::ShimmerProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()) {}
ShimmerProcessor::~ShimmerProcessor() {}
void ShimmerProcessor::prepareToPlay(double sr, int sb) { engine_.prepareToPlay(sr, sb); }
void ShimmerProcessor::processBlock(juce::AudioBuffer<float>& buf, juce::MidiBuffer&) {
    juce::ScopedNoDenormals noDenormals;
    updateEngineParameters();
    engine_.processBlock(buf);
}
void ShimmerProcessor::updateEngineParameters() {
    engine_.setSize(apvts_.getRawParameterValue("size")->load());
    engine_.setDamping(apvts_.getRawParameterValue("damping")->load());
    engine_.setShimmer(apvts_.getRawParameterValue("shimmer")->load());
    engine_.setPitchMode(apvts_.getRawParameterValue("pitchMode")->load());
    engine_.setFeedback(apvts_.getRawParameterValue("feedback")->load());
    engine_.setMix(apvts_.getRawParameterValue("mix")->load());
}
juce::AudioProcessorEditor* ShimmerProcessor::createEditor() { return new ShimmerEditor(*this); }
void ShimmerProcessor::getStateInformation(juce::MemoryBlock& d) {
    auto s = apvts_.copyState(); std::unique_ptr<juce::XmlElement> x(s.createXml()); copyXmlToBinary(*x, d);
}
void ShimmerProcessor::setStateInformation(const void* d, int sz) {
    std::unique_ptr<juce::XmlElement> x(getXmlFromBinary(d, sz));
    if (x && x->hasTagName(apvts_.state.getType())) apvts_.replaceState(juce::ValueTree::fromXml(*x));
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new ShimmerProcessor(); }
