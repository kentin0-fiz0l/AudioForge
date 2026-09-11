#include "PluginProcessor.h"
#include "PluginEditor.h"
static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterFloat>("time", "Time", juce::NormalisableRange<float>(10.0f, 2000.0f, 1.0f, 0.3f), 500.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("feedback", "Feedback", 0.0f, 0.95f, 0.4f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("wow", "Wow", 0.0f, 1.0f, 0.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("flutter", "Flutter", 0.0f, 1.0f, 0.1f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("saturation", "Saturation", 0.0f, 1.0f, 0.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("mix", "Mix", 0.0f, 1.0f, 0.5f));
    return {params.begin(), params.end()};
}
DelayProcessor::DelayProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()),
      midiLearnManager_(apvts_),
      presetManager_(apvts_, "TapeDelay") {

    // Scan for presets on startup
    presetManager_.scanPresets();
}
DelayProcessor::~DelayProcessor() {}
void DelayProcessor::prepareToPlay(double sr, int sb) { engine_.prepareToPlay(sr, sb); }
void DelayProcessor::processBlock(juce::AudioBuffer<float>& buf, juce::MidiBuffer& midi) {
    juce::ScopedNoDenormals noDenormals;
    for (const auto metadata : midi)
        midiLearnManager_.processMidiMessage(metadata.getMessage());
    updateEngineParameters();
    engine_.processBlock(buf);
}
void DelayProcessor::updateEngineParameters() {
    engine_.setTime(apvts_.getRawParameterValue("time")->load());
    engine_.setFeedback(apvts_.getRawParameterValue("feedback")->load());
    engine_.setWow(apvts_.getRawParameterValue("wow")->load());
    engine_.setFlutter(apvts_.getRawParameterValue("flutter")->load());
    engine_.setSaturation(apvts_.getRawParameterValue("saturation")->load());
    engine_.setMix(apvts_.getRawParameterValue("mix")->load());
}
juce::AudioProcessorEditor* DelayProcessor::createEditor() { return new DelayEditor(*this); }
void DelayProcessor::getStateInformation(juce::MemoryBlock& d) {
    auto s = apvts_.copyState(); std::unique_ptr<juce::XmlElement> x(s.createXml()); copyXmlToBinary(*x, d);
}
void DelayProcessor::setStateInformation(const void* d, int sz) {
    std::unique_ptr<juce::XmlElement> x(getXmlFromBinary(d, sz));
    if (x && x->hasTagName(apvts_.state.getType())) apvts_.replaceState(juce::ValueTree::fromXml(*x));
        if (auto* midiXml = x->getChildByName("MIDILearnMappings"))
            midiLearnManager_.loadFromXml(*midiXml);
        if (auto* presetXml = x->getChildByName("PresetManagerState"))
            presetManager_.loadFromXml(*presetXml);
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new DelayProcessor(); }
