#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

/**
 * DeEsser DSP Engine
 *
 * Multiband compression focused on sibilance frequencies (4-10 kHz).
 * Uses frequency-selective sidechain for natural-sounding de-essing.
 */
class DeEsserEngine {
public:
    DeEsserEngine();

    void prepare(double sampleRate, int samplesPerBlock);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();

    // Parameters
    void setFrequency(float freqHz);      // Center frequency (4-10 kHz)
    void setBandwidth(float octaves);     // Bandwidth (0.5-4 octaves)
    void setThreshold(float thresholdDb); // Threshold (-60 to 0 dB)
    void setRatio(float ratio);           // Compression ratio (1-10)
    void setAttack(float attackMs);       // Attack time (0.1-10 ms)
    void setRelease(float releaseMs);     // Release time (10-500 ms)
    void setMix(float mix);               // Dry/wet mix (0-1)

    // Metering
    float getGainReduction() const { return currentGainReduction_; }

private:
    // DSP components
    using Filter = juce::dsp::IIR::Filter<float>;
    using Coefficients = juce::dsp::IIR::Coefficients<float>;

    juce::dsp::ProcessorDuplicator<Filter, Coefficients> highPassFilter_;
    juce::dsp::ProcessorDuplicator<Filter, Coefficients> lowPassFilter_;
    juce::dsp::ProcessorDuplicator<Filter, Coefficients> bandPassFilter_; // For sidechain

    // Envelope follower for sidechain
    float envelope_;
    float attackCoeff_;
    float releaseCoeff_;

    // Parameters
    float frequency_;
    float bandwidth_;
    float threshold_;
    float ratio_;
    float mix_;

    // State
    double sampleRate_;
    float currentGainReduction_;

    // Helper functions
    void updateFilters();
    float calculateGainReduction(float sidechainLevel);
};
