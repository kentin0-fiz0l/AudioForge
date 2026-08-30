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
    bool acceptsMidi() const override { return true; }
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
    // Preset Management
    bool savePreset(const juce::File& file);
    bool loadPreset(const juce::File& file);
    juce::File getDefaultPresetsDirectory();
    static juce::StringArray getPresetNames();  // Get list of available presets

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
    static constexpr const char* PARAM_WINDOW_TYPE = "windowType";
    static constexpr const char* PARAM_WINDOW_SHAPE = "windowShape";

private:
    //==============================================================================
    // DSP Components
    GrainBuffer grainBuffer;
    GrainExtractor grainExtractor;
    GrainScheduler grainScheduler;

    // Output buffers
    std::vector<float> granularLeftOutput;
    std::vector<float> granularRightOutput;

    // MIDI state
    bool midiGateActive = false;
    float midiVelocity = 0.0f;

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
    juce::AudioParameterInt* windowTypeParam;
    juce::AudioParameterFloat* windowShapeParam;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GranularEngineProcessor)
};
