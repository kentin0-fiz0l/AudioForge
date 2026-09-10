#include "PluginProcessor.h"
#include "PluginEditor.h"

static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // 8 mappings - each with enabled, input CC, output CC, min, max, curve, invert
    for (int i = 0; i < MappingEngine::NUM_MAPPINGS; ++i) {
        juce::String idx = juce::String(i + 1);

        params.push_back(std::make_unique<juce::AudioParameterBool>(
            "map" + idx + "Enabled", "Map " + idx + " Enable", i == 0));

        params.push_back(std::make_unique<juce::AudioParameterInt>(
            "map" + idx + "InCC", "Map " + idx + " In CC", 1, 127, i + 1));

        params.push_back(std::make_unique<juce::AudioParameterInt>(
            "map" + idx + "OutCC", "Map " + idx + " Out CC", 1, 127, (i + 1) * 10));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            "map" + idx + "Min", "Map " + idx + " Min", 0.0f, 1.0f, 0.0f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            "map" + idx + "Max", "Map " + idx + " Max", 0.0f, 1.0f, 1.0f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            "map" + idx + "Curve", "Map " + idx + " Curve", 0.1f, 10.0f, 1.0f));

        params.push_back(std::make_unique<juce::AudioParameterBool>(
            "map" + idx + "Invert", "Map " + idx + " Invert", false));
    }

    return {params.begin(), params.end()};
}

MIDICCMapperProcessor::MIDICCMapperProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()),
      midiLearnManager_(apvts_) {}

MIDICCMapperProcessor::~MIDICCMapperProcessor() {}

void MIDICCMapperProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    // No preparation needed
}

void MIDICCMapperProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                          juce::MidiBuffer& midiMessages) {
    juce::MidiBuffer processedMidi;

    // Process MIDI learn
    for (const auto metadata : midiMessages)
        midiLearnManager_.processMidiMessage(metadata.getMessage());

    // Update engine from parameters
    updateEngine();

    // Process MIDI messages
    for (const auto metadata : midiMessages) {
        auto msg = metadata.getMessage();
        int samplePosition = metadata.samplePosition;

        if (msg.isController()) {
            int inputCC = msg.getControllerNumber();
            float inputValue = msg.getControllerValue() / 127.0f;

            // Get mapped outputs
            auto outputs = engine_.processCC(inputCC, inputValue);

            if (outputs.empty()) {
                // No mapping - pass through original
                processedMidi.addEvent(msg, samplePosition);
            } else {
                // Add mapped CCs
                for (const auto& output : outputs) {
                    int ccValue = (int)(output.value * 127.0f);
                    ccValue = juce::jlimit(0, 127, ccValue);

                    processedMidi.addEvent(
                        juce::MidiMessage::controllerEvent(msg.getChannel(), output.cc, ccValue),
                        samplePosition);
                }
            }
        } else {
            // Pass through non-CC messages
            processedMidi.addEvent(msg, samplePosition);
        }
    }

    // Replace input MIDI with processed MIDI
    midiMessages.swapWith(processedMidi);

    // Clear audio buffer
    buffer.clear();
}

void MIDICCMapperProcessor::updateEngine() {
    for (int i = 0; i < MappingEngine::NUM_MAPPINGS; ++i) {
        juce::String idx = juce::String(i + 1);

        bool enabled = apvts_.getRawParameterValue("map" + idx + "Enabled")->load() > 0.5f;
        int inCC = (int)apvts_.getRawParameterValue("map" + idx + "InCC")->load();
        int outCC = (int)apvts_.getRawParameterValue("map" + idx + "OutCC")->load();
        float min = apvts_.getRawParameterValue("map" + idx + "Min")->load();
        float max = apvts_.getRawParameterValue("map" + idx + "Max")->load();
        float curve = apvts_.getRawParameterValue("map" + idx + "Curve")->load();
        bool invert = apvts_.getRawParameterValue("map" + idx + "Invert")->load() > 0.5f;

        engine_.setMappingEnabled(i, enabled);
        engine_.setInputCC(i, inCC);
        engine_.setOutputCC(i, outCC);
        engine_.setRange(i, min, max);
        engine_.setCurve(i, curve);
        engine_.setInvert(i, invert);
    }
}

juce::AudioProcessorEditor* MIDICCMapperProcessor::createEditor() {
    return new MIDICCMapperEditor(*this);
}

void MIDICCMapperProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto state = apvts_.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    xml->addChildElement(midiLearnManager_.saveToXml().release());
    copyXmlToBinary(*xml, destData);
}

void MIDICCMapperProcessor::setStateInformation(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(apvts_.state.getType())) {
        apvts_.replaceState(juce::ValueTree::fromXml(*xml));
        if (auto* midiXml = xml->getChildByName("MIDILearnMappings"))
            midiLearnManager_.loadFromXml(*midiXml);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new MIDICCMapperProcessor();
}
