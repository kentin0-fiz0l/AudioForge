#include "PluginProcessor.h"
#include "PluginEditor.h"

static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterInt>("numBands", "Bands", 8, 32, 16));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("formantShift", "Formant Shift", 
        juce::NormalisableRange<float>(0.5f, 2.0f, 0.01f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("carrierLevel", "Carrier Level", 0.0f, 1.0f, 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("modulatorLevel", "Modulator Level", 0.0f, 1.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("attack", "Attack", 
        juce::NormalisableRange<float>(0.001f, 0.1f, 0.001f), 0.01f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("release", "Release", 
        juce::NormalisableRange<float>(0.01f, 0.5f, 0.01f), 0.05f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("mix", "Mix", 0.0f, 1.0f, 1.0f));
    return {params.begin(), params.end()};
}

VocoderProcessor::VocoderProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()),
      midiLearnManager_(apvts_),
      presetManager_(apvts_, "Vocoder") {

    // Scan for presets on startup
    presetManager_.scanPresets();
}

VocoderProcessor::~VocoderProcessor() {}

void VocoderProcessor::prepareToPlay(double sr, int sb) { 
    engine_.prepareToPlay(sr, sb); 
    updateEngineParameters();
}

void VocoderProcessor::processBlock(juce::AudioBuffer<float>& buf, juce::MidiBuffer& midi) {
    juce::ScopedNoDenormals noDenormals;

    // Process MIDI messages through MIDI Learn Manager
    for (const auto metadata : midi) {
        auto message = metadata.getMessage();
        midiLearnManager_.processMidiMessage(message);
    }

    updateEngineParameters();
    engine_.processBlock(buf, midi);
}

void VocoderProcessor::updateEngineParameters() {
    engine_.setNumBands(apvts_.getRawParameterValue("numBands")->load());
    engine_.setFormantShift(apvts_.getRawParameterValue("formantShift")->load());
    engine_.setCarrierLevel(apvts_.getRawParameterValue("carrierLevel")->load());
    engine_.setModulatorLevel(apvts_.getRawParameterValue("modulatorLevel")->load());
    engine_.setAttack(apvts_.getRawParameterValue("attack")->load());
    engine_.setRelease(apvts_.getRawParameterValue("release")->load());
    engine_.setMix(apvts_.getRawParameterValue("mix")->load());
}

juce::AudioProcessorEditor* VocoderProcessor::createEditor() { return new VocoderEditor(*this); }

void VocoderProcessor::getStateInformation(juce::MemoryBlock& d) {
    auto s = apvts_.copyState();
    std::unique_ptr<juce::XmlElement> x(s.createXml());

    // Add MIDI mappings to state
    x->addChildElement(midiLearnManager_.saveToXml().release());
    x->addChildElement(presetManager_.saveToXml().release());

    copyXmlToBinary(*x, d);
}

void VocoderProcessor::setStateInformation(const void* d, int sz) {
    std::unique_ptr<juce::XmlElement> x(getXmlFromBinary(d, sz));
    if (x && x->hasTagName(apvts_.state.getType())) {
        apvts_.replaceState(juce::ValueTree::fromXml(*x));

        // Load MIDI mappings from state
        if (auto* midiMappingsXml = x->getChildByName("MIDILearnMappings"))
            midiLearnManager_.loadFromXml(*midiMappingsXml);
        if (auto* presetXml = x->getChildByName("PresetManagerState"))
            presetManager_.loadFromXml(*presetXml);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new VocoderProcessor(); }
