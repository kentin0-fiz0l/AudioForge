#include "PluginProcessor.h"
#include "PluginEditor.h"

static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "xPosition", "X Position", 0.0f, 1.0f, 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "yPosition", "Y Position", 0.0f, 1.0f, 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "xCC", "X CC", 1, 127, 1));  // Default CC 1 (Mod Wheel)

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "yCC", "Y CC", 1, 127, 11)); // Default CC 11 (Expression)

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "channel", "MIDI Channel", 1, 16, 1));

    return {params.begin(), params.end()};
}

XYPadControllerProcessor::XYPadControllerProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()),
      presetManager_(apvts_, "XYPadController") {

    // Scan for presets on startup
    presetManager_.scanPresets();
}

XYPadControllerProcessor::~XYPadControllerProcessor() {}

void XYPadControllerProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    // No preparation needed
}

void XYPadControllerProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                             juce::MidiBuffer& midiMessages) {
    // Process any pending CC messages from UI
    {
        juce::ScopedLock lock(ccLock_);

        if (!pendingCCs_.empty()) {
            int channel = (int)apvts_.getRawParameterValue("channel")->load();

            for (const auto& pending : pendingCCs_) {
                midiMessages.addEvent(
                    juce::MidiMessage::controllerEvent(channel, pending.cc, pending.value),
                    0);  // Send at start of block
            }

            pendingCCs_.clear();
        }
    }

    // Clear audio buffer (MIDI controller, no audio processing)
    buffer.clear();
}

void XYPadControllerProcessor::queueCCMessage(int cc, float value) {
    juce::ScopedLock lock(ccLock_);

    // Convert normalized value (0.0-1.0) to MIDI CC range (0-127)
    int ccValue = (int)(value * 127.0f);
    ccValue = juce::jlimit(0, 127, ccValue);

    // Check if we already have a pending message for this CC
    bool found = false;
    for (auto& pending : pendingCCs_) {
        if (pending.cc == cc) {
            pending.value = ccValue;
            found = true;
            break;
        }
    }

    if (!found) {
        pendingCCs_.push_back({cc, ccValue});
    }
}

juce::AudioProcessorEditor* XYPadControllerProcessor::createEditor() {
    return new XYPadControllerEditor(*this);
}

void XYPadControllerProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto state = apvts_.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    xml->addChildElement(midiLearnManager_.saveToXml().release());
    xml->addChildElement(presetManager_.saveToXml().release());
    copyXmlToBinary(*xml, destData);
}

void XYPadControllerProcessor::setStateInformation(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(apvts_.state.getType())) {
        apvts_.replaceState(juce::ValueTree::fromXml(*xml));
        if (auto* midiXml = xml->getChildByName("MIDILearnMappings"))
            midiLearnManager_.loadFromXml(*midiXml);
        if (auto* presetXml = xml->getChildByName("PresetManagerState"))
            presetManager_.loadFromXml(*presetXml);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new XYPadControllerProcessor();
}
