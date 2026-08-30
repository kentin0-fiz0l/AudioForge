#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "GrainBuffer.h"
#include "GrainExtractor.h"

/**
 * GranularEngine Plugin Processor
 *
 * Real-time granular synthesis and manipulation plugin.
 */
class GranularEngineProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    GranularEngineProcessor();
    ~GranularEngineProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    //==============================================================================
    const juce::String getName() const override { return "GranularEngine"; }
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
    // Parameter IDs (Phase 1: Basic params)
    static constexpr const char* PARAM_GRAIN_SIZE = "grainSize";
    static constexpr const char* PARAM_DRY_WET = "dryWet";

private:
    //==============================================================================
    // DSP Components
    GrainBuffer grainBuffer;
    GrainExtractor grainExtractor;

    // Temporary grain storage
    std::vector<float> grainTemp;

    //==============================================================================
    // Parameters (Phase 1)
    juce::AudioParameterFloat* grainSizeParam;
    juce::AudioParameterFloat* dryWetParam;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GranularEngineProcessor)
};
