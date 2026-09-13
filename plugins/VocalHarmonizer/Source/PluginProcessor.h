#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "HarmonizerEngine.h"
#include "../../../shared/preset/PresetManager.h"

class VocalHarmonizerProcessor : public juce::AudioProcessor {
public:
    VocalHarmonizerProcessor();
    ~VocalHarmonizerProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Vocal Harmonizer"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.1; } // 100ms max delay

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Parameter IDs
    static constexpr const char* PARAM_VOICES = "voices";
    static constexpr const char* PARAM_VOICE1_PITCH = "voice1_pitch";
    static constexpr const char* PARAM_VOICE2_PITCH = "voice2_pitch";
    static constexpr const char* PARAM_VOICE3_PITCH = "voice3_pitch";
    static constexpr const char* PARAM_VOICE4_PITCH = "voice4_pitch";
    static constexpr const char* PARAM_VOICE1_PAN = "voice1_pan";
    static constexpr const char* PARAM_VOICE2_PAN = "voice2_pan";
    static constexpr const char* PARAM_VOICE3_PAN = "voice3_pan";
    static constexpr const char* PARAM_VOICE4_PAN = "voice4_pan";
    static constexpr const char* PARAM_SCALE = "scale";
    static constexpr const char* PARAM_FORMANT = "formant_shift";
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
    HarmonizerEngine harmonizer_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VocalHarmonizerProcessor)
};
