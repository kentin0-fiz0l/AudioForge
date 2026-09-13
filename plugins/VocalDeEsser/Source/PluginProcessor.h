#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "DeEsserEngine.h"
#include "../../../shared/preset/PresetManager.h"

class VocalDeEsserProcessor : public juce::AudioProcessor {
public:
    VocalDeEsserProcessor();
    ~VocalDeEsserProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Vocal DeEsser"; }
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
    static constexpr const char* PARAM_FREQUENCY = "frequency";
    static constexpr const char* PARAM_BANDWIDTH = "bandwidth";
    static constexpr const char* PARAM_THRESHOLD = "threshold";
    static constexpr const char* PARAM_RATIO = "ratio";
    static constexpr const char* PARAM_ATTACK = "attack";
    static constexpr const char* PARAM_RELEASE = "release";
    static constexpr const char* PARAM_MIX = "mix";

    // Access
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts_; }
    AudioForge::PresetManager& getPresetManager() { return presetManager_; }
    float getGainReduction() const { return deEsser_.getGainReduction(); }

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::AudioProcessorValueTreeState apvts_;
    AudioForge::PresetManager presetManager_;
    DeEsserEngine deEsser_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VocalDeEsserProcessor)
};
