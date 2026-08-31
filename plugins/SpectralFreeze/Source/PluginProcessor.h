#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "SpectralProcessor.h"

/**
 * SpectralFreeze Plugin Processor
 *
 * Real-time spectral freezing and manipulation plugin.
 */
class SpectralFreezeProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    SpectralFreezeProcessor();
    ~SpectralFreezeProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    //==============================================================================
    const juce::String getName() const override { return "SpectralFreeze"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    //==============================================================================
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    // Phase 4: Preset Management
    bool savePreset(const juce::String& presetName);
    bool loadPreset(const juce::String& presetName);
    juce::StringArray getAvailablePresets() const;
    void loadFactoryPreset(int index);

    //==============================================================================
    // Parameter IDs
    static constexpr const char* PARAM_FREEZE = "freeze";
    static constexpr const char* PARAM_BLUR = "blur";
    static constexpr const char* PARAM_LOW_CUT = "lowCut";
    static constexpr const char* PARAM_HIGH_CUT = "highCut";
    static constexpr const char* PARAM_DRY_WET = "dryWet";

    // Phase 2: Advanced Effects
    static constexpr const char* PARAM_STRETCH = "stretch";
    static constexpr const char* PARAM_SHIFT = "shift";
    static constexpr const char* PARAM_MORPH = "morph";
    static constexpr const char* PARAM_GATE = "gate";
    static constexpr const char* PARAM_RANDOMIZE = "randomize";

    //==============================================================================
    // DSP (public for visualization access)
    SpectralProcessor spectralProcessor;

private:

    //==============================================================================
    // Parameters
    juce::AudioParameterBool* freezeParam;
    juce::AudioParameterFloat* blurParam;
    juce::AudioParameterFloat* lowCutParam;
    juce::AudioParameterFloat* highCutParam;
    juce::AudioParameterFloat* dryWetParam;

    // Phase 2: Advanced Effect Parameters
    juce::AudioParameterFloat* stretchParam;
    juce::AudioParameterFloat* shiftParam;
    juce::AudioParameterFloat* morphParam;
    juce::AudioParameterFloat* gateParam;
    juce::AudioParameterFloat* randomizeParam;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectralFreezeProcessor)
};
