#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "GrainBuffer.h"
#include "GrainExtractor.h"
#include "GrainScheduler.h"

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
    // Parameter IDs
    static constexpr const char* PARAM_GRAIN_SIZE = "grainSize";
    static constexpr const char* PARAM_GRAIN_DENSITY = "grainDensity";
    static constexpr const char* PARAM_TIME_STRETCH = "timeStretch";
    static constexpr const char* PARAM_POSITION = "position";
    static constexpr const char* PARAM_PITCH_SHIFT = "pitchShift";
    static constexpr const char* PARAM_SPRAY = "spray";
    static constexpr const char* PARAM_REVERSE = "reverse";
    static constexpr const char* PARAM_STEREO_WIDTH = "stereoWidth";
    static constexpr const char* PARAM_DRY_WET = "dryWet";

private:
    //==============================================================================
    // DSP Components
    GrainBuffer grainBuffer;
    GrainExtractor grainExtractor;
    GrainScheduler grainScheduler;

    // Output buffers
    std::vector<float> granularLeftOutput;
    std::vector<float> granularRightOutput;

    //==============================================================================
    // Parameters
    juce::AudioParameterFloat* grainSizeParam;
    juce::AudioParameterFloat* grainDensityParam;
    juce::AudioParameterFloat* timeStretchParam;
    juce::AudioParameterFloat* positionParam;
    juce::AudioParameterFloat* pitchShiftParam;
    juce::AudioParameterFloat* sprayParam;
    juce::AudioParameterFloat* reverseParam;
    juce::AudioParameterFloat* stereoWidthParam;
    juce::AudioParameterFloat* dryWetParam;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GranularEngineProcessor)
};
