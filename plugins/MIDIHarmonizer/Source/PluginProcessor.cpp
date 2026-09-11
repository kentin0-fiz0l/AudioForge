#include "PluginProcessor.h"
#include "PluginEditor.h"

static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "key", "Key",
        juce::StringArray{"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"},
        0));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "scale", "Scale",
        juce::StringArray{
            "Chromatic", "Major", "Minor", "Dorian", "Phrygian", "Lydian",
            "Mixolydian", "Aeolian", "Locrian", "Harmonic Minor", "Melodic Minor",
            "Pentatonic", "Blues"
        },
        1)); // Default to Major

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "thirdAbove", "3rd Above", true));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "fifthAbove", "5th Above", false));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "octaveAbove", "Octave Above", false));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "thirdBelow", "3rd Below", false));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "fifthBelow", "5th Below", false));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "octaveBelow", "Octave Below", false));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "mix", "Mix", 0.0f, 1.0f, 1.0f));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "useInputVel", "Use Input Velocity", true));

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "fixedVel", "Fixed Velocity", 1, 127, 100));

    return {params.begin(), params.end()};
}

MIDIHarmonizerProcessor::MIDIHarmonizerProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()),
      midiLearnManager_(apvts_),
      presetManager_(apvts_, "MIDIHarmonizer") {

    // Scan for presets on startup
    presetManager_.scanPresets();
}

MIDIHarmonizerProcessor::~MIDIHarmonizerProcessor() {}

void MIDIHarmonizerProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    // No preparation needed
}

void MIDIHarmonizerProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                            juce::MidiBuffer& midiMessages) {
    juce::MidiBuffer processedMidi;

    // Process MIDI learn
    for (const auto metadata : midiMessages)
        midiLearnManager_.processMidiMessage(metadata.getMessage());

    // Update engine with current parameters
    updateEngineParameters();

    // Process MIDI messages
    for (const auto metadata : midiMessages) {
        auto msg = metadata.getMessage();
        int samplePosition = metadata.samplePosition;

        if (msg.isNoteOn()) {
            engine_.processNoteOn(msg.getNoteNumber(), msg.getVelocity(),
                                msg.getChannel(), processedMidi, samplePosition);
        } else if (msg.isNoteOff()) {
            engine_.processNoteOff(msg.getNoteNumber(), msg.getChannel(),
                                 processedMidi, samplePosition);
        } else {
            // Pass through other MIDI messages
            processedMidi.addEvent(msg, samplePosition);
        }
    }

    // Replace input MIDI with harmonized MIDI
    midiMessages.swapWith(processedMidi);

    // Clear audio buffer (MIDI effect, no audio processing)
    buffer.clear();
}

void MIDIHarmonizerProcessor::updateEngineParameters() {
    int keyIdx = apvts_.getRawParameterValue("key")->load();
    engine_.setKey(static_cast<HarmonizerEngine::Key>(keyIdx));

    int scaleIdx = apvts_.getRawParameterValue("scale")->load();
    engine_.setScale(static_cast<HarmonizerEngine::Scale>(scaleIdx));

    engine_.setThirdAbove(apvts_.getRawParameterValue("thirdAbove")->load() > 0.5f);
    engine_.setFifthAbove(apvts_.getRawParameterValue("fifthAbove")->load() > 0.5f);
    engine_.setOctaveAbove(apvts_.getRawParameterValue("octaveAbove")->load() > 0.5f);
    engine_.setThirdBelow(apvts_.getRawParameterValue("thirdBelow")->load() > 0.5f);
    engine_.setFifthBelow(apvts_.getRawParameterValue("fifthBelow")->load() > 0.5f);
    engine_.setOctaveBelow(apvts_.getRawParameterValue("octaveBelow")->load() > 0.5f);
    engine_.setMix(apvts_.getRawParameterValue("mix")->load());
    engine_.setVelocityMode(apvts_.getRawParameterValue("useInputVel")->load() > 0.5f);
    engine_.setFixedVelocity((int)apvts_.getRawParameterValue("fixedVel")->load());
}

juce::AudioProcessorEditor* MIDIHarmonizerProcessor::createEditor() {
    return new MIDIHarmonizerEditor(*this);
}

void MIDIHarmonizerProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto state = apvts_.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    xml->addChildElement(midiLearnManager_.saveToXml().release());
    xml->addChildElement(presetManager_.saveToXml().release());
    copyXmlToBinary(*xml, destData);
}

void MIDIHarmonizerProcessor::setStateInformation(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(apvts_.state.getType())) {
        apvts_.replaceState(juce::ValueTree::fromXml(*xml));
        if (auto* midiXml = xml->getChildByName("MIDILearnMappings"))
            midiLearnManager_.loadFromXml(*midiXml);
        if (auto* presetXml = xmlState->getChildByName("PresetManagerState"))
            presetManager_.loadFromXml(*presetXml);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new MIDIHarmonizerProcessor();
}
