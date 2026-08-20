#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

/**
 * PanUtil Audio Processor
 *
 * Stereo panning and width control utility.
 *
 * Features:
 * - Pan control (-100% L to +100% R)
 * - Width control (0% = mono, 100% = normal, 200% = wide)
 * - Mode selection (Pan vs Balance)
 * - Constant-power panning algorithm
 * - Independent L/R level metering
 */
class PanUtilProcessor : public juce::AudioProcessor
{
public:
    PanUtilProcessor();
    ~PanUtilProcessor() override;

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
    float getLeftLevel() const { return leftLevel.load(); }
    float getRightLevel() const { return rightLevel.load(); }

private:
    //==============================================================================
    // Parameters
    juce::AudioParameterFloat* panParam;      // -1.0 (left) to +1.0 (right)
    juce::AudioParameterFloat* widthParam;    // 0.0 to 2.0 (1.0 = normal)
    juce::AudioParameterChoice* modeParam;    // 0 = Pan, 1 = Balance

    // Smoothed values for click-free operation
    juce::SmoothedValue<float> smoothedPan;
    juce::SmoothedValue<float> smoothedWidth;

    // Level metering (atomic for thread safety)
    std::atomic<float> leftLevel { 0.0f };
    std::atomic<float> rightLevel { 0.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PanUtilProcessor)
};
