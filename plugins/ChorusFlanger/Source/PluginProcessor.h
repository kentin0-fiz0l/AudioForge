#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "dsp/Metering.h"

//==============================================================================
/**
 * Chorus/Flanger Audio Processor
 *
 * Modulation effects using LFO-modulated delay lines:
 * - Chorus mode: Slow, wide modulation for voice doubling effect
 * - Flanger mode: Fast, narrow modulation with feedback for comb filtering
 *
 * Features:
 * - Dual delay lines for stereo processing
 * - Fractional delay with linear interpolation
 * - Adjustable LFO rate, depth, and waveform
 * - Feedback control for flanger resonance
 * - Stereo width control via LFO phase offset
 * - Mix control for parallel processing
 */
class ChorusFlangerAudioProcessor : public juce::AudioProcessor
{
public:
    ChorusFlangerAudioProcessor();
    ~ChorusFlangerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    // Parameters
    juce::AudioParameterFloat* rateParam;        // LFO rate (0.1-10 Hz)
    juce::AudioParameterFloat* depthParam;       // Modulation depth (0-100%)
    juce::AudioParameterChoice* modeParam;       // Chorus/Flanger
    juce::AudioParameterFloat* feedbackParam;    // Feedback amount (0-95%)
    juce::AudioParameterFloat* mixParam;         // Dry/wet mix (0-100%)
    juce::AudioParameterChoice* waveformParam;   // Sine/Triangle
    juce::AudioParameterFloat* stereoWidthParam; // Stereo width (0-100%)

    // Metering
    AudioForge::DSP::ThreadSafeMeter inputMeter;
    AudioForge::DSP::ThreadSafeMeter outputMeter;

    // LFO state for visualization
    float getCurrentLFOPhase() const { return lfoPhaseLeft; }
    float getStereoLFOPhase() const { return lfoPhaseRight; }

private:
    //==============================================================================
    // LFO state
    float lfoPhaseLeft = 0.0f;   // Left channel LFO phase (0-1)
    float lfoPhaseRight = 0.0f;  // Right channel LFO phase (0-1)

    // Delay buffers (circular buffers)
    static constexpr int maxDelayBufferSize = 192000; // 2 seconds at 96kHz
    std::vector<float> delayBufferLeft;
    std::vector<float> delayBufferRight;
    int writePosition = 0;

    // Sample rate
    double currentSampleRate = 44100.0;

    // DC blocker
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                    juce::dsp::IIR::Coefficients<float>> dcBlocker;

    // Helper functions
    float getLFOSample(float phase, int waveform);
    float getInterpolatedDelaySample(const std::vector<float>& buffer, float delayInSamples);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChorusFlangerAudioProcessor)
};
