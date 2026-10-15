/*
  PluginProcessor.h - DrumMachine Audio Processor
*/

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "DrumEngine.h"
#include "DrumSynth.h"

class DrumMachineProcessor : public juce::AudioProcessor
{
public:
    DrumMachineProcessor();
    ~DrumMachineProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "DrumMachine"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    bool isSynth() const { return true; }

    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // DrumMachine specific
    DrumEngine& getDrumEngine() { return drumEngine_; }
    // A copy, taken under the edit lock: the pattern may be replaced from another thread
    DrumEngine::DrumPattern getCurrentPattern() const;
    void setPattern (DrumEngine::DrumPattern pattern);

    // Grid edits. setHit returns whether anything changed.
    bool setHit (int voice, int step, bool on);
    void toggleHit (int voice, int step);
    void clearPattern();

    void generateNewPattern(const std::string& style);
    void setPatternEnabled(bool enabled) { patternEnabled_ = enabled; }
    bool isPatternEnabled() const { return patternEnabled_; }

    void setComplexity(int complexity) { complexity_ = juce::jlimit (1, 10, complexity); }
    int getComplexity() const { return complexity_; }

    void setSwing(float swing) { swing_ = swing; }
    float getSwing() const { return swing_; }

    void setFillsEnabled(bool enabled) { fillsEnabled_ = enabled; }
    bool getFillsEnabled() const { return fillsEnabled_; }

    // MIDI export
    void exportToMidiFile(const juce::File& file);

private:
    DrumEngine drumEngine_;
    DrumSynth drumSynth_;
    DrumEngine::DrumPattern currentPattern_;
    juce::SpinLock patternLock_;  // processBlock only try-locks it; held while the pattern is swapped
    // Held by everything outside the audio thread that replaces or reads the pattern (saving,
    // export, the window): a host can save or load from a background thread
    juce::CriticalSection patternEditLock_;

    static constexpr double beatsPerStep = 0.25;  // 16th notes: 16 steps = 1 bar of 4/4

    void resetSequencer();

    double sampleRate_ = 44100.0;
    std::atomic<bool> patternEnabled_ { true };

    std::atomic<int> complexity_ { 5 }; // 1-10
    std::atomic<float> swing_ { 0.0f }; // 0.0-1.0, delays off-beat 16ths at playback
    std::atomic<bool> fillsEnabled_ { false };

    // Sequencer state
    juce::int64 lastTriggeredStep_ = -1;   // Counted from the start of the song
    double expectedBlockStartBeat_ = 0.0;  // Where the next block starts if the transport runs on
    bool resync_ = true;                   // After a start or a jump: steps already passed do not fire

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DrumMachineProcessor)
};
