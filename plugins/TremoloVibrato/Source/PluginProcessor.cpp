#include "PluginProcessor.h"
#include "PluginEditor.h"

static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterChoice>("mode", "Mode", juce::StringArray{"Tremolo", "Vibrato"}, 0));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("waveform", "Waveform", juce::StringArray{"Sine", "Triangle", "Square"}, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("rate", "Rate", juce::NormalisableRange<float>(0.1f, 20.0f, 0.01f), 5.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("depth", "Depth", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("mix", "Mix", 0.0f, 1.0f, 1.0f));
    return {params.begin(), params.end()};
}

TremoloVibratoProcessor::TremoloVibratoProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()),
      midiLearnManager_(apvts_) {}

TremoloVibratoProcessor::~TremoloVibratoProcessor() {}

void TremoloVibratoProcessor::prepareToPlay(double sr, int sb) { engine_.prepareToPlay(sr, sb); }

void TremoloVibratoProcessor::processBlock(juce::AudioBuffer<float>& buf, juce::MidiBuffer& midi) {
    juce::ScopedNoDenormals noDenormals;
    for (const auto metadata : midi)
        midiLearnManager_.processMidiMessage(metadata.getMessage());
    updateEngineParameters();
    engine_.processBlock(buf);
}

void TremoloVibratoProcessor::updateEngineParameters() {
    int modeIdx = apvts_.getRawParameterValue("mode")->load();
    engine_.setMode(modeIdx == 0 ? TremoloVibratoEngine::Mode::Tremolo : TremoloVibratoEngine::Mode::Vibrato);
    engine_.setWaveform(apvts_.getRawParameterValue("waveform")->load());
    engine_.setRate(apvts_.getRawParameterValue("rate")->load());
    engine_.setDepth(apvts_.getRawParameterValue("depth")->load());
    engine_.setMix(apvts_.getRawParameterValue("mix")->load());
}

juce::AudioProcessorEditor* TremoloVibratoProcessor::createEditor() { return new TremoloVibratoEditor(*this); }

void TremoloVibratoProcessor::getStateInformation(juce::MemoryBlock& d) {
    auto s = apvts_.copyState(); std::unique_ptr<juce::XmlElement> x(s.createXml()); copyXmlToBinary(*x, d);
}

void TremoloVibratoProcessor::setStateInformation(const void* d, int sz) {
    std::unique_ptr<juce::XmlElement> x(getXmlFromBinary(d, sz));
    if (x && x->hasTagName(apvts_.state.getType())) apvts_.replaceState(juce::ValueTree::fromXml(*x));
        if (auto* midiXml = x->getChildByName("MIDILearnMappings"))
            midiLearnManager_.loadFromXml(*midiXml);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new TremoloVibratoProcessor(); }
