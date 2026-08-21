#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <dsp/AudioForgeDSP.h>

/**
 * SimpleComp Audio Processor
 *
 * A basic dynamic range compressor with essential controls.
 * Features:
 * - Threshold (-60dB to 0dB)
 * - Ratio (1:1 to 20:1)
 * - Attack (0.1ms to 100ms, log scale)
 * - Release (10ms to 1000ms, log scale)
 * - Hard/Soft knee mode
 * - Automatic makeup gain
 * - Real-time gain reduction metering
 */
class SimpleCompProcessor : public juce::AudioProcessor
{
public:
    SimpleCompProcessor();
    ~SimpleCompProcessor() override;

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
    float getCurrentGainReduction() const { return currentGainReductionDB.load(); }

    // Parameter IDs
    static constexpr const char* PARAM_THRESHOLD = "threshold";
    static constexpr const char* PARAM_RATIO = "ratio";
    static constexpr const char* PARAM_ATTACK = "attack";
    static constexpr const char* PARAM_RELEASE = "release";
    static constexpr const char* PARAM_KNEE = "knee";

private:
    //==============================================================================
    // Parameters
    juce::AudioParameterFloat* thresholdParam;
    juce::AudioParameterFloat* ratioParam;
    juce::AudioParameterFloat* attackParam;
    juce::AudioParameterFloat* releaseParam;
    juce::AudioParameterChoice* kneeParam;

    // Compressor state (per channel)
    struct ChannelState
    {
        float envelopeFollower = 0.0f;  // Smoothed level detector
    };
    std::array<ChannelState, 2> channelState;  // L and R

    double currentSampleRate = 44100.0;

    // Gain reduction for metering (atomic for thread safety)
    std::atomic<float> currentGainReductionDB{0.0f};

    //==============================================================================
    // Compressor DSP functions

    /**
     * Calculate gain reduction in dB based on input level
     * @param inputLevelDB Input level in dB
     * @param threshold Threshold in dB
     * @param ratio Compression ratio (e.g., 4.0 for 4:1)
     * @param knee Knee width in dB (0 = hard knee, >0 = soft knee)
     * @return Gain reduction in dB (positive value)
     */
    float calculateGainReduction(float inputLevelDB, float threshold, float ratio, float knee);

    /**
     * Calculate automatic makeup gain based on compression settings
     * @param threshold Threshold in dB
     * @param ratio Compression ratio
     * @return Makeup gain in dB
     */
    float calculateMakeupGain(float threshold, float ratio);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleCompProcessor)
};
