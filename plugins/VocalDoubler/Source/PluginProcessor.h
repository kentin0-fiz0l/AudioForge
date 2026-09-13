#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "DoublerEngine.h"
#include "../../../shared/preset/PresetManager.h"

class VocalDoublerProcessor : public juce::AudioProcessor {
public:
    VocalDoublerProcessor();
    ~VocalDoublerProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Vocal Doubler"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.05; } // 50ms max delay

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Parameter IDs
    static constexpr const char* PARAM_VOICES = "voices";
    static constexpr const char* PARAM_WIDTH = "width";
    static constexpr const char* PARAM_PITCH_VAR = "pitch_variation";
    static constexpr const char* PARAM_TIMING = "timing_shift";
    static constexpr const char* PARAM_HUMANIZE = "humanize";
    static constexpr const char* PARAM_HIGHPASS = "highpass";
    static constexpr const char* PARAM_LOWPASS = "lowpass";
    static constexpr const char* PARAM_MIX = "mix";
    static constexpr const char* PARAM_GAIN = "output_gain";

    // Access
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts_; }
    AudioForge::PresetManager& getPresetManager() { return presetManager_; }

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::AudioProcessorValueTreeState apvts_;
    AudioForge::PresetManager presetManager_;
    DoublerEngine doubler_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VocalDoublerProcessor)
};
