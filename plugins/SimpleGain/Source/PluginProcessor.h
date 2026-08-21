#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <dsp/AudioForgeDSP.h>

/**
 * SimpleGain Audio Processor
 *
 * A basic gain/volume plugin to learn JUCE and DSP fundamentals.
 *
 * Features:
 * - Gain control in dB (-60 to +12 dB)
 * - Smooth parameter changes (no clicks)
 * - Level metering
 */
class SimpleGainProcessor : public juce::AudioProcessor
{
public:
    SimpleGainProcessor();
    ~SimpleGainProcessor() override;

    //==============================================================================
    // Audio processing
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    // Editor
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    //==============================================================================
    // Plugin info
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    //==============================================================================
    // Programs
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    //==============================================================================
    // State
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    // Parameter access for editor
    float getCurrentLevel() const { return levelMeter.getLevel(); }

private:
    //==============================================================================
    // Parameters
    juce::AudioParameterFloat* gainParam;

    // Smoothed gain to avoid clicks (using shared DSP library)
    AudioForge::DSP::ParameterSmoothing<float> smoothedGain;

    // Level metering (using shared DSP library)
    AudioForge::DSP::ThreadSafeMeter levelMeter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleGainProcessor)
};
