#include "PluginProcessor.h"
#include "PluginEditor.h"

static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "chordType", "Chord Type",
        juce::StringArray{
            "Major", "Minor", "Dim", "Aug", "Sus2", "Sus4",
            "Maj7", "Min7", "Dom7", "MinMaj7", "Dim7", "m7b5",
            "Maj6", "Min6", "Add9", "Maj9", "Min9", "Dom9"
        },
        0));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "voicing", "Voicing",
        juce::StringArray{"Root", "1st Inv", "2nd Inv", "3rd Inv", "Spread", "Drop2", "Drop3"},
        0));

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "octaveShift", "Octave Shift", -2, 2, 0));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "strum", "Strum", 0.0f, 100.0f, 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "useInputVel", "Use Input Velocity", true));

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "fixedVel", "Fixed Velocity", 1, 127, 100));

    return {params.begin(), params.end()};
}

MIDIChordGeneratorProcessor::MIDIChordGeneratorProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()),
      midiLearnManager_(apvts_) {}

MIDIChordGeneratorProcessor::~MIDIChordGeneratorProcessor() {}

void MIDIChordGeneratorProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    // No preparation needed for chord generator
}

void MIDIChordGeneratorProcessor::processBlock(juce::AudioBuffer<float>& buffer,
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
            // Pass through other MIDI messages (CC, pitch bend, etc.)
            processedMidi.addEvent(msg, samplePosition);
        }
    }

    // Replace input MIDI with chord-generated MIDI
    midiMessages.swapWith(processedMidi);

    // Clear audio buffer (MIDI effect, no audio processing)
    buffer.clear();
}

void MIDIChordGeneratorProcessor::updateEngineParameters() {
    int chordTypeIdx = apvts_.getRawParameterValue("chordType")->load();
    engine_.setChordType(static_cast<ChordEngine::ChordType>(chordTypeIdx));

    int voicingIdx = apvts_.getRawParameterValue("voicing")->load();
    engine_.setVoicing(static_cast<ChordEngine::Voicing>(voicingIdx));

    engine_.setOctaveShift((int)apvts_.getRawParameterValue("octaveShift")->load());
    engine_.setStrum(apvts_.getRawParameterValue("strum")->load());
    engine_.setVelocityMode(apvts_.getRawParameterValue("useInputVel")->load() > 0.5f);
    engine_.setFixedVelocity((int)apvts_.getRawParameterValue("fixedVel")->load());
}

juce::AudioProcessorEditor* MIDIChordGeneratorProcessor::createEditor() {
    return new MIDIChordGeneratorEditor(*this);
}

void MIDIChordGeneratorProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto state = apvts_.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    xml->addChildElement(midiLearnManager_.saveToXml().release());
    copyXmlToBinary(*xml, destData);
}

void MIDIChordGeneratorProcessor::setStateInformation(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(apvts_.state.getType())) {
        apvts_.replaceState(juce::ValueTree::fromXml(*xml));
        if (auto* midiXml = xml->getChildByName("MIDILearnMappings"))
            midiLearnManager_.loadFromXml(*midiXml);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new MIDIChordGeneratorProcessor();
}
