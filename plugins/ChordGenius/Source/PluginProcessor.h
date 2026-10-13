/*
  PluginProcessor.h - ChordGenius Audio Processor
*/

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <array>
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
    static constexpr int kNumGenres = 8;
    static constexpr int kChordsPerProgression = 4;

    juce::AudioProcessorValueTreeState& getValueTreeState() { return parameters_; }

    // The selected genre's progression in the selected key and mode (message thread only)
    std::vector<ChordEngine::Chord> getCurrentProgression();

    // MIDI export
    void exportToMidiFile(const juce::File& file);

private:
    using ChordNotes = std::array<int, 3>;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void startChord (juce::MidiBuffer& midi, juce::int64 bar, int samplePosition);
    void releaseChord (juce::MidiBuffer& midi, int samplePosition);

    juce::AudioProcessorValueTreeState parameters_;
    juce::AudioParameterChoice* keyParam_ = nullptr;
    juce::AudioParameterChoice* modeParam_ = nullptr;
    juce::AudioParameterChoice* genreParam_ = nullptr;

    ChordEngine chordEngine_;

    // Every genre's progression in C, [major/minor][genre][chord], built once
    // so the audio thread only has to look it up and transpose it
    std::array<std::array<std::array<ChordNotes, kChordsPerProgression>, kNumGenres>, 2> progressionsInC_ {};

    // Audio thread playback state
    double sampleRate_ = 44100.0;
    bool wasPlaying_ = false;
    double nextBlockStartPpq_ = 0.0;
    juce::int64 nextBar_ = 0;
    ChordNotes soundingNotes_ {};
    bool chordSounding_ = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChordGeniusProcessor)
};
