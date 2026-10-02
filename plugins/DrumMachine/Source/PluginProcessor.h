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
    DrumEngine::DrumPattern& getCurrentPattern() { return currentPattern_; }

    void generateNewPattern(const std::string& style);
    void setPatternEnabled(bool enabled) { patternEnabled_ = enabled; }
    bool isPatternEnabled() const { return patternEnabled_; }

    void setComplexity(int complexity) { complexity_ = complexity; }
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

    double sampleRate_ = 44100.0;
    bool patternEnabled_ = true;
    int patternLengthInBeats_ = 4;  // 1 bar at 4/4 = 16 steps

    int complexity_ = 5;         // 1-10
    float swing_ = 0.0f;         // 0.0-1.0
    bool fillsEnabled_ = false;

    // Sequencer state
    int lastProcessedStep_ = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DrumMachineProcessor)
};
