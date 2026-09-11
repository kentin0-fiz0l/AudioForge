#include "PluginProcessor.h"
#include "PluginEditor.h"
static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterFloat>("pitch", "Pitch", juce::NormalisableRange<float>(40.0f, 300.0f, 1.0f), 90.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("pitchEnv", "Pitch Env", juce::NormalisableRange<float>(0.0f, 3.0f, 0.01f), 1.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("decay", "Decay", juce::NormalisableRange<float>(0.05f, 2.0f, 0.001f, 0.3f), 0.4f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("bend", "Bend", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("noise", "Noise", 0.0f, 1.0f, 0.2f));
    return {params.begin(), params.end()};
}
TomProcessor::TomProcessor() : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()),
      midiLearnManager_(apvts_) {
      presetManager_(apvts_, "TomSynth") {

    // Scan for presets on startup
    presetManager_.scanPresets();
}
    for (int i = 0; i < 4; ++i) synth_.addVoice(new TomVoice());
    synth_.addSound(new TomSound());
}
TomProcessor::~TomProcessor() {}
void TomProcessor::prepareToPlay(double sr, int sb) {
    synth_.setCurrentPlaybackSampleRate(sr);
    for (int i = 0; i < synth_.getNumVoices(); ++i)
        if (auto* v = dynamic_cast<TomVoice*>(synth_.getVoice(i))) v->prepareToPlay(sr, sb);
}
void TomProcessor::processBlock(juce::AudioBuffer<float>& buf, juce::MidiBuffer& midi) {
    for (const auto metadata : midi)
        midiLearnManager_.processMidiMessage(metadata.getMessage());
    buf.clear(); updateVoiceParameters(); synth_.renderNextBlock(buf, midi, 0, buf.getNumSamples());
}
void TomProcessor::updateVoiceParameters() {
    float pitch = apvts_.getRawParameterValue("pitch")->load();
    float pitchEnv = apvts_.getRawParameterValue("pitchEnv")->load();
    float decay = apvts_.getRawParameterValue("decay")->load();
    float bend = apvts_.getRawParameterValue("bend")->load();
    float noise = apvts_.getRawParameterValue("noise")->load();
    for (int i = 0; i < synth_.getNumVoices(); ++i)
        if (auto* v = dynamic_cast<TomVoice*>(synth_.getVoice(i)))
            { v->setPitch(pitch); v->setPitchEnv(pitchEnv); v->setDecay(decay); v->setBend(bend); v->setNoise(noise); }
}
juce::AudioProcessorEditor* TomProcessor::createEditor() { return new TomEditor(*this); }
void TomProcessor::getStateInformation(juce::MemoryBlock& d) {
    auto s = apvts_.copyState(); std::unique_ptr<juce::XmlElement> x(s.createXml()); copyXmlToBinary(*x, d);
}
void TomProcessor::setStateInformation(const void* d, int sz) {
    std::unique_ptr<juce::XmlElement> x(getXmlFromBinary(d, sz));
    if (x && x->hasTagName(apvts_.state.getType())) apvts_.replaceState(juce::ValueTree::fromXml(*x));
        if (auto* midiXml = x->getChildByName("MIDILearnMappings"))
            midiLearnManager_.loadFromXml(*midiXml);
        if (auto* presetXml = xml->getChildByName("PresetManagerState"))
            presetManager_.loadFromXml(*presetXml);
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new TomProcessor(); }
