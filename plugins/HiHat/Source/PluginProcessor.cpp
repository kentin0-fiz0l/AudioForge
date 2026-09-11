#include "PluginProcessor.h"
#include "PluginEditor.h"
static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterFloat>("tone", "Tone", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("decay", "Decay", juce::NormalisableRange<float>(0.01f, 0.5f, 0.001f, 0.3f), 0.08f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("metallic", "Metallic", 0.0f, 1.0f, 0.7f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("openness", "Openness", 0.0f, 1.0f, 0.3f));
    return {params.begin(), params.end()};
}
HiHatProcessor::HiHatProcessor() : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()),
      midiLearnManager_(apvts_),
      presetManager_(apvts_, "HiHat") {

    // Scan for presets on startup
    presetManager_.scanPresets();
    for (int i = 0; i < 4; ++i) synth_.addVoice(new HiHatVoice());
    synth_.addSound(new HiHatSound());
}
HiHatProcessor::~HiHatProcessor() {}
void HiHatProcessor::prepareToPlay(double sr, int sb) {
    synth_.setCurrentPlaybackSampleRate(sr);
    for (int i = 0; i < synth_.getNumVoices(); ++i)
        if (auto* v = dynamic_cast<HiHatVoice*>(synth_.getVoice(i))) v->prepareToPlay(sr, sb);
}
void HiHatProcessor::processBlock(juce::AudioBuffer<float>& buf, juce::MidiBuffer& midi) {
    for (const auto metadata : midi)
        midiLearnManager_.processMidiMessage(metadata.getMessage());
    buf.clear(); updateVoiceParameters(); synth_.renderNextBlock(buf, midi, 0, buf.getNumSamples());
}
void HiHatProcessor::updateVoiceParameters() {
    float tone = apvts_.getRawParameterValue("tone")->load();
    float decay = apvts_.getRawParameterValue("decay")->load();
    float metallic = apvts_.getRawParameterValue("metallic")->load();
    float openness = apvts_.getRawParameterValue("openness")->load();
    for (int i = 0; i < synth_.getNumVoices(); ++i)
        if (auto* v = dynamic_cast<HiHatVoice*>(synth_.getVoice(i))) {
            v->setTone(tone); v->setDecay(decay); v->setMetallic(metallic); v->setOpenness(openness);
        }
}
juce::AudioProcessorEditor* HiHatProcessor::createEditor() { return new HiHatEditor(*this); }
void HiHatProcessor::getStateInformation(juce::MemoryBlock& d) {
    auto s = apvts_.copyState(); std::unique_ptr<juce::XmlElement> x(s.createXml()); copyXmlToBinary(*x, d);
}
void HiHatProcessor::setStateInformation(const void* d, int sz) {
    std::unique_ptr<juce::XmlElement> x(getXmlFromBinary(d, sz));
    if (x && x->hasTagName(apvts_.state.getType())) apvts_.replaceState(juce::ValueTree::fromXml(*x));
        if (auto* midiXml = x->getChildByName("MIDILearnMappings"))
            midiLearnManager_.loadFromXml(*midiXml);
        if (auto* presetXml = xmlState->getChildByName("PresetManagerState"))
            presetManager_.loadFromXml(*presetXml);
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new HiHatProcessor(); }
