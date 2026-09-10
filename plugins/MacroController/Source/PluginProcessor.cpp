#include "PluginProcessor.h"
#include "PluginEditor.h"

static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Main macro knob
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "macro", "Macro", 0.0f, 1.0f, 0.0f));

    // MIDI channel
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "channel", "MIDI Channel", 1, 16, 1));

    // 8 targets - each with enabled, CC, min, max, curve
    for (int i = 0; i < MacroEngine::NUM_TARGETS; ++i) {
        juce::String idx = juce::String(i + 1);

        params.push_back(std::make_unique<juce::AudioParameterBool>(
            "target" + idx + "Enabled", "Target " + idx + " Enable", i < 4));

        params.push_back(std::make_unique<juce::AudioParameterInt>(
            "target" + idx + "CC", "Target " + idx + " CC", 1, 127, i + 1));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            "target" + idx + "Min", "Target " + idx + " Min", 0.0f, 1.0f, 0.0f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            "target" + idx + "Max", "Target " + idx + " Max", 0.0f, 1.0f, 1.0f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            "target" + idx + "Curve", "Target " + idx + " Curve", 0.1f, 10.0f, 1.0f));
    }

    return {params.begin(), params.end()};
}

MacroControllerProcessor::MacroControllerProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()),
      midiLearnManager_(apvts_) {}

MacroControllerProcessor::~MacroControllerProcessor() {}

void MacroControllerProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    // No preparation needed
}

void MacroControllerProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                             juce::MidiBuffer& midiMessages) {
    // Update engine from parameters
    updateEngine();

    // Generate CC messages if macro changed
    float currentMacro = apvts_.getRawParameterValue("macro")->load();
    if (std::abs(currentMacro - lastMacroValue_) > 0.001f) {
        generateCCMessages(midiMessages);
        lastMacroValue_ = currentMacro;
    }

    // Clear audio buffer
    buffer.clear();
}

void MacroControllerProcessor::updateEngine() {
    for (int i = 0; i < MacroEngine::NUM_TARGETS; ++i) {
        juce::String idx = juce::String(i + 1);

        bool enabled = apvts_.getRawParameterValue("target" + idx + "Enabled")->load() > 0.5f;
        int cc = (int)apvts_.getRawParameterValue("target" + idx + "CC")->load();
        float min = apvts_.getRawParameterValue("target" + idx + "Min")->load();
        float max = apvts_.getRawParameterValue("target" + idx + "Max")->load();
        float curve = apvts_.getRawParameterValue("target" + idx + "Curve")->load();

        engine_.setTargetEnabled(i, enabled);
        engine_.setTargetCC(i, cc);
        engine_.setTargetRange(i, min, max);
        engine_.setTargetCurve(i, curve);
    }
}

void MacroControllerProcessor::generateCCMessages(juce::MidiBuffer& midiMessages) {
    float macroValue = apvts_.getRawParameterValue("macro")->load();
    int channel = (int)apvts_.getRawParameterValue("channel")->load();

    auto targetValues = engine_.getTargetValues(macroValue);

    for (int i = 0; i < MacroEngine::NUM_TARGETS; ++i) {
        const auto& target = engine_.getTarget(i);
        if (target.enabled) {
            int ccValue = (int)(targetValues[i] * 127.0f);
            ccValue = juce::jlimit(0, 127, ccValue);

            midiMessages.addEvent(
                juce::MidiMessage::controllerEvent(channel, target.cc, ccValue),
                0);
        }
    }
}

juce::AudioProcessorEditor* MacroControllerProcessor::createEditor() {
    return new MacroControllerEditor(*this);
}

void MacroControllerProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto state = apvts_.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    xml->addChildElement(midiLearnManager_.saveToXml().release());
    copyXmlToBinary(*xml, destData);
}

void MacroControllerProcessor::setStateInformation(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(apvts_.state.getType())) {
        apvts_.replaceState(juce::ValueTree::fromXml(*xml));
        if (auto* midiXml = xml->getChildByName("MIDILearnMappings"))
            midiLearnManager_.loadFromXml(*midiXml);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new MacroControllerProcessor();
}
