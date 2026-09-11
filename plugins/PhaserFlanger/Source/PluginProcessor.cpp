#include "PluginProcessor.h"
#include "PluginEditor.h"

static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterChoice>("mode", "Mode", juce::StringArray{"Phaser", "Flanger"}, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("rate", "Rate", juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("depth", "Depth", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("feedback", "Feedback", juce::NormalisableRange<float>(-0.9f, 0.9f, 0.01f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("centreFreq", "Centre Freq", juce::NormalisableRange<float>(200.0f, 2000.0f, 1.0f), 1000.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("mix", "Mix", 0.0f, 1.0f, 0.5f));
    return {params.begin(), params.end()};
}

PhaserFlangerProcessor::PhaserFlangerProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()),
      presetManager_(apvts_, "PhaserFlanger") {

    // Scan for presets on startup
    presetManager_.scanPresets();
}

PhaserFlangerProcessor::~PhaserFlangerProcessor() {}

void PhaserFlangerProcessor::prepareToPlay(double sr, int sb) { engine_.prepareToPlay(sr, sb); }

void PhaserFlangerProcessor::processBlock(juce::AudioBuffer<float>& buf, juce::MidiBuffer& midi) {
    juce::ScopedNoDenormals noDenormals;
    for (const auto metadata : midi)
        midiLearnManager_.processMidiMessage(metadata.getMessage());
    updateEngineParameters();
    engine_.processBlock(buf);
}

void PhaserFlangerProcessor::updateEngineParameters() {
    int modeIdx = apvts_.getRawParameterValue("mode")->load();
    engine_.setMode(modeIdx == 0 ? ModulationEngine::Mode::Phaser : ModulationEngine::Mode::Flanger);
    engine_.setRate(apvts_.getRawParameterValue("rate")->load());
    engine_.setDepth(apvts_.getRawParameterValue("depth")->load());
    engine_.setFeedback(apvts_.getRawParameterValue("feedback")->load());
    engine_.setCentreFrequency(apvts_.getRawParameterValue("centreFreq")->load());
    engine_.setMix(apvts_.getRawParameterValue("mix")->load());
}

juce::AudioProcessorEditor* PhaserFlangerProcessor::createEditor() { return new PhaserFlangerEditor(*this); }

void PhaserFlangerProcessor::getStateInformation(juce::MemoryBlock& d) {
    auto s = apvts_.copyState(); std::unique_ptr<juce::XmlElement> x(s.createXml()); copyXmlToBinary(*x, d);
}

void PhaserFlangerProcessor::setStateInformation(const void* d, int sz) {
    std::unique_ptr<juce::XmlElement> x(getXmlFromBinary(d, sz));
    if (x && x->hasTagName(apvts_.state.getType())) apvts_.replaceState(juce::ValueTree::fromXml(*x));
        if (auto* midiXml = x->getChildByName("MIDILearnMappings"))
            midiLearnManager_.loadFromXml(*midiXml);
        if (auto* presetXml = xml->getChildByName("PresetManagerState"))
            presetManager_.loadFromXml(*presetXml);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new PhaserFlangerProcessor(); }
