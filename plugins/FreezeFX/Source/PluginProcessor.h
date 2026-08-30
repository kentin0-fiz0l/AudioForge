#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "SpectralProcessor.h"
#include "FrozenSpectrum.h"
#include "PhaseEvolver.h"

/**
 * FreezeFX Plugin Processor
 *
 * Real-time spectral freezing plugin with phase evolution.
 */
class FreezeFXProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    FreezeFXProcessor();
    ~FreezeFXProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    //==============================================================================
    const juce::String getName() const override { return "FreezeFX"; }
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
    static constexpr const char* PARAM_FREEZE = "freeze";
    static constexpr const char* PARAM_FREEZE_MIX = "freezeMix";
    static constexpr const char* PARAM_FFT_SIZE = "fftSize";
    static constexpr const char* PARAM_OVERLAP = "overlap";
    static constexpr const char* PARAM_PHASE_RANDOM = "phaseRandom";
    static constexpr const char* PARAM_PHASE_SPEED = "phaseSpeed";
    static constexpr const char* PARAM_SPECTRAL_BLUR = "spectralBlur";
    static constexpr const char* PARAM_HIGH_PASS = "highPass";
    static constexpr const char* PARAM_LOW_PASS = "lowPass";

    //==============================================================================
    // Access to spectral data (for visualization)
    const SpectralProcessor& getSpectralProcessor() const { return spectralProcessor; }
    bool isCurrentlyFrozen() const { return frozenSpectrum.isFrozen(); }

private:
    //==============================================================================
    // DSP Components
    SpectralProcessor spectralProcessor;
    FrozenSpectrum frozenSpectrum;
    PhaseEvolver phaseEvolver;

    //==============================================================================
    // Parameters
    juce::AudioParameterBool* freezeParam;
    juce::AudioParameterFloat* freezeMixParam;
    juce::AudioParameterChoice* fftSizeParam;
    juce::AudioParameterChoice* overlapParam;
    juce::AudioParameterFloat* phaseRandomParam;
    juce::AudioParameterFloat* phaseSpeedParam;
    juce::AudioParameterFloat* spectralBlurParam;
    juce::AudioParameterFloat* highPassParam;
    juce::AudioParameterFloat* lowPassParam;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FreezeFXProcessor)
};
