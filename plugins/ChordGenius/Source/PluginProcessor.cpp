/*
  PluginProcessor.cpp - Implementation
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

ChordGeniusProcessor::ChordGeniusProcessor()
    : AudioProcessor (BusesProperties()
                     .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                     .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
    // Initialize with C Major
    chordEngine_.setKey(60, true);

    // Generate initial pop progression
    currentProgression_ = chordEngine_.generateProgression("pop", 4);
}

ChordGeniusProcessor::~ChordGeniusProcessor()
{
}

void ChordGeniusProcessor::prepareToPlay (double, int)
{
}

void ChordGeniusProcessor::releaseResources()
{
}

void ChordGeniusProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                         juce::MidiBuffer& midiMessages)
{
    buffer.clear();

    // Pass through MIDI (for now)
    // In a future version, we can add real-time chord generation
}

juce::AudioProcessorEditor* ChordGeniusProcessor::createEditor()
{
    return new ChordGeniusEditor (*this);
}

void ChordGeniusProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Save state (key, progression, etc.)
}

void ChordGeniusProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Restore state
}

void ChordGeniusProcessor::generateNewProgression(const std::string& genre)
{
    currentProgression_ = chordEngine_.generateProgression(genre, 4);
}

void ChordGeniusProcessor::playChord(const ChordEngine::Chord& chord)
{
    // This would send MIDI notes to the host DAW
    // Implementation depends on MIDI timing
}

void ChordGeniusProcessor::exportToMidiFile(const juce::File& file)
{
    juce::MidiFile midiFile;
    midiFile.setTicksPerQuarterNote(480);
    juce::MidiMessageSequence track;

    track.addEvent(juce::MidiMessage::tempoMetaEvent(500000), 0);
    track.addEvent(juce::MidiMessage::timeSignatureMetaEvent(4, 4), 0);
    track.addEvent(juce::MidiMessage::textMetaEvent(3, "Chords"), 0);

    int ticksPerBeat = 480;
    int currentBeat = 0;

    for (const auto& chord : currentProgression_) {
        int timeStamp = currentBeat * ticksPerBeat;

        // Add all notes in the chord
        for (int note : chord.notes) {
            track.addEvent(juce::MidiMessage::noteOn(1, note, juce::uint8(100)), timeStamp);
            track.addEvent(juce::MidiMessage::noteOff(1, note), timeStamp + (ticksPerBeat * 4) - 1);
        }

        currentBeat += 4;  // Each chord lasts 4 beats
    }

    track.updateMatchedPairs();
    midiFile.addTrack(track);

    juce::FileOutputStream stream(file);
    if (stream.openedOk()) {
        midiFile.writeTo(stream);
    }
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChordGeniusProcessor();
}
