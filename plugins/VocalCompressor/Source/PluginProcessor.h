#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "CompressorEngine.h"
#include "../../../shared/preset/PresetManager.h"

class VocalCompressorProcessor : public juce::AudioProcessor {
public:
    VocalCompressorProcessor();
    ~VocalCompressorProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Vocal Compressor"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Parameter IDs
    static constexpr const char* PARAM_THRESHOLD = "threshold";
    static constexpr const char* PARAM_RATIO = "ratio";
    static constexpr const char* PARAM_ATTACK = "attack";
    static constexpr const char* PARAM_RELEASE = "release";
    static constexpr const char* PARAM_KNEE = "knee";
    static constexpr const char* PARAM_MAKEUP = "makeup";
    static constexpr const char* PARAM_PARALLEL_MIX = "parallel_mix";
    static constexpr const char* PARAM_DETECTION = "detection";

    // Access
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts_; }
    AudioForge::PresetManager& getPresetManager() { return presetManager_; }
    float getGainReduction() const { return compressor_.getGainReduction(); }
    float getOutputLevel() const { return compressor_.getOutputLevel(); }

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::AudioProcessorValueTreeState apvts_;
    AudioForge::PresetManager presetManager_;
    CompressorEngine compressor_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VocalCompressorProcessor)
};
