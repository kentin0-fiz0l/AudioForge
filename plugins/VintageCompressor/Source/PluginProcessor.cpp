#include "PluginProcessor.h"
#include "PluginEditor.h"

static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterChoice>("mode", "Mode", juce::StringArray{"VCA", "FET", "Opto"}, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("threshold", "Threshold", juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), -20.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("ratio", "Ratio", juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f), 4.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("attack", "Attack", juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f, 0.3f), 10.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("release", "Release", juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f, 0.3f), 100.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("knee", "Knee", juce::NormalisableRange<float>(0.0f, 12.0f, 0.1f), 3.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("makeup", "Makeup", juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("mix", "Mix", 0.0f, 1.0f, 1.0f));
    return {params.begin(), params.end()};
}

CompressorProcessor::CompressorProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()),
      midiLearnManager_(apvts_),
      presetManager_(apvts_, "VintageCompressor") {

    // Scan for presets on startup
    presetManager_.scanPresets();
}

CompressorProcessor::~CompressorProcessor() {}

void CompressorProcessor::prepareToPlay(double sr, int sb) {
    engine_.prepareToPlay(sr, sb);
}

void CompressorProcessor::processBlock(juce::AudioBuffer<float>& buf, juce::MidiBuffer& midi) {
    juce::ScopedNoDenormals noDenormals;
    for (const auto metadata : midi)
        midiLearnManager_.processMidiMessage(metadata.getMessage());
    updateEngineParameters();
    engine_.processBlock(buf);
}

void CompressorProcessor::updateEngineParameters() {
    int modeIdx = apvts_.getRawParameterValue("mode")->load();
    engine_.setMode(static_cast<CompressorEngine::Mode>(modeIdx));
    engine_.setThreshold(apvts_.getRawParameterValue("threshold")->load());
    engine_.setRatio(apvts_.getRawParameterValue("ratio")->load());
    engine_.setAttack(apvts_.getRawParameterValue("attack")->load());
    engine_.setRelease(apvts_.getRawParameterValue("release")->load());
    engine_.setKnee(apvts_.getRawParameterValue("knee")->load());
    engine_.setMakeupGain(apvts_.getRawParameterValue("makeup")->load());
    engine_.setMix(apvts_.getRawParameterValue("mix")->load());
}

juce::AudioProcessorEditor* CompressorProcessor::createEditor() {
    return new CompressorEditor(*this);
}

void CompressorProcessor::getStateInformation(juce::MemoryBlock& d) {
    auto s = apvts_.copyState();
    std::unique_ptr<juce::XmlElement> x(s.createXml());
    x->addChildElement(midiLearnManager_.saveToXml().release());
    xml->addChildElement(presetManager_.saveToXml().release());
    copyXmlToBinary(*x, d);
}

void CompressorProcessor::setStateInformation(const void* d, int sz) {
    std::unique_ptr<juce::XmlElement> x(getXmlFromBinary(d, sz));
    if (x && x->hasTagName(apvts_.state.getType()))
        apvts_.replaceState(juce::ValueTree::fromXml(*x));
        if (auto* midiXml = x->getChildByName("MIDILearnMappings"))
            midiLearnManager_.loadFromXml(*midiXml);
        if (auto* presetXml = xml->getChildByName("PresetManagerState"))
            presetManager_.loadFromXml(*presetXml);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new CompressorProcessor();
}
