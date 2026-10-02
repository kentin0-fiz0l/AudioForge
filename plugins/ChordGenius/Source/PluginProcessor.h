/*
  PluginProcessor.h - ChordGenius Audio Processor
*/

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "ChordEngine.h"

class ChordGeniusProcessor : public juce::AudioProcessor
{
public:
    ChordGeniusProcessor();
    ~ChordGeniusProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "ChordGenius"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    bool isMidiEffect() const override { return true; }

    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // ChordGenius specific
    ChordEngine& getChordEngine() { return chordEngine_; }
    std::vector<ChordEngine::Chord>& getCurrentProgression() { return currentProgression_; }

    void generateNewProgression(const std::string& genre);
    void playChord(const ChordEngine::Chord& chord);

    // MIDI export
    void exportToMidiFile(const juce::File& file);

private:
    ChordEngine chordEngine_;
    std::vector<ChordEngine::Chord> currentProgression_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChordGeniusProcessor)
};
