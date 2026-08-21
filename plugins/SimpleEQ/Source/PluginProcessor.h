#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <dsp/AudioForgeDSP.h>

/**
 * SimpleEQ Audio Processor
 *
 * A 3-band parametric EQ with optional high-pass and low-pass filters.
 * Features:
 * - Low shelf (20Hz - 500Hz, ±12dB)
 * - Parametric mid (200Hz - 5kHz, ±12dB, Q=1.0)
 * - High shelf (2kHz - 20kHz, ±12dB)
 * - Optional high-pass filter (20Hz - 500Hz, 12dB/octave)
 * - Optional low-pass filter (2kHz - 20kHz, 12dB/octave)
 * - Real-time input/output metering
 */
class SimpleEQProcessor : public juce::AudioProcessor
{
public:
    SimpleEQProcessor();
    ~SimpleEQProcessor() override;

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
    float getCurrentInputLevel() const { return inputLevelMeter.getLevel(); }
    float getCurrentOutputLevel() const { return outputLevelMeter.getLevel(); }

    // Parameter IDs
    static constexpr const char* PARAM_LOW_FREQ = "lowFreq";
    static constexpr const char* PARAM_LOW_GAIN = "lowGain";
    static constexpr const char* PARAM_MID_FREQ = "midFreq";
    static constexpr const char* PARAM_MID_GAIN = "midGain";
    static constexpr const char* PARAM_HIGH_FREQ = "highFreq";
    static constexpr const char* PARAM_HIGH_GAIN = "highGain";
    static constexpr const char* PARAM_HPF_ENABLED = "hpfEnabled";
    static constexpr const char* PARAM_HPF_FREQ = "hpfFreq";
    static constexpr const char* PARAM_LPF_ENABLED = "lpfEnabled";
    static constexpr const char* PARAM_LPF_FREQ = "lpfFreq";

private:
    //==============================================================================
    // Parameters
    juce::AudioParameterFloat* lowFreqParam;
    juce::AudioParameterFloat* lowGainParam;
    juce::AudioParameterFloat* midFreqParam;
    juce::AudioParameterFloat* midGainParam;
    juce::AudioParameterFloat* highFreqParam;
    juce::AudioParameterFloat* highGainParam;
    juce::AudioParameterBool* hpfEnabledParam;
    juce::AudioParameterFloat* hpfFreqParam;
    juce::AudioParameterBool* lpfEnabledParam;
    juce::AudioParameterFloat* lpfFreqParam;

    // EQ filters (per channel)
    struct ChannelFilters
    {
        AudioForge::DSP::BiquadFilter lowShelf;
        AudioForge::DSP::BiquadFilter midBand;
        AudioForge::DSP::BiquadFilter highShelf;
        AudioForge::DSP::BiquadFilter highPass;
        AudioForge::DSP::BiquadFilter lowPass;

        void reset()
        {
            lowShelf.reset();
            midBand.reset();
            highShelf.reset();
            highPass.reset();
            lowPass.reset();
        }
    };

    std::array<ChannelFilters, 2> channelFilters;  // L and R channels

    // Filter coefficient caching
    struct FilterCache
    {
        float lowFreq = -1.0f;
        float lowGain = -999.0f;
        float midFreq = -1.0f;
        float midGain = -999.0f;
        float highFreq = -1.0f;
        float highGain = -999.0f;
        float hpfFreq = -1.0f;
        float lpfFreq = -1.0f;
    };
    FilterCache filterCache;

    double currentSampleRate = 44100.0;

    // Level metering
    AudioForge::DSP::ThreadSafeMeter inputLevelMeter;
    AudioForge::DSP::ThreadSafeMeter outputLevelMeter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleEQProcessor)
};
