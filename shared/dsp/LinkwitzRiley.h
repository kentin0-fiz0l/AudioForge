/*
  ==============================================================================

    LinkwitzRiley.h
    Created: 1 Sep 2026
    Author:  AudioForge Team

    Linkwitz-Riley 4th-order crossover network for multiband processing.

    Theory:
    - LR4 = 2x cascaded Butterworth 2nd-order filters
    - At crossover frequency: both outputs are -6dB
    - When summed: flat magnitude response (phase-coherent)
    - No phase distortion at crossover points

    Used by: Multiband Compressor, future multiband plugins

  ==============================================================================
*/

#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>
#include <array>

namespace audioforge
{

/**
 * @brief Linkwitz-Riley 4th-order crossover network
 *
 * Splits audio into 4 frequency bands with phase-coherent reconstruction.
 *
 * Bands:
 * - Band 0: Low (20 Hz - crossover1)
 * - Band 1: Low-Mid (crossover1 - crossover2)
 * - Band 2: High-Mid (crossover2 - crossover3)
 * - Band 3: High (crossover3 - 20 kHz)
 *
 * Properties:
 * - Phase-coherent: summed bands = original signal
 * - 24 dB/octave rolloff per crossover
 * - Industry standard (FabFilter, iZotope, Waves)
 */
class LinkwitzRileyCrossover
{
public:
    static constexpr int NumBands = 4;
    static constexpr int NumCrossovers = 3;

    LinkwitzRileyCrossover();
    ~LinkwitzRileyCrossover() = default;

    //==============================================================================
    // Configuration

    /**
     * @brief Prepare the crossover for processing
     *
     * @param sampleRate Sample rate in Hz
     * @param maxBlockSize Maximum samples per block
     */
    void prepare(double sampleRate, int maxBlockSize);

    /**
     * @brief Set crossover frequency
     *
     * @param index Crossover index (0-2)
     * @param frequency Frequency in Hz
     */
    void setCrossoverFrequency(int index, float frequency);

    /**
     * @brief Get crossover frequency
     *
     * @param index Crossover index (0-2)
     * @return Frequency in Hz
     */
    float getCrossoverFrequency(int index) const;

    /**
     * @brief Reset internal state
     */
    void reset();

    //==============================================================================
    // Processing

    /**
     * @brief Split input into 4 frequency bands
     *
     * @param input Input audio samples
     * @param numSamples Number of samples to process
     * @param bandOutputs Array of 4 band output buffers (pre-allocated)
     */
    void processSplit(const float* input, int numSamples, float** bandOutputs);

    /**
     * @brief Sum 4 frequency bands back into one signal
     *
     * @param bandInputs Array of 4 band input buffers
     * @param output Output audio samples
     * @param numSamples Number of samples to process
     */
    void processSum(const float** bandInputs, float* output, int numSamples);

private:
    //==============================================================================
    // Filter Structure

    // Each crossover = 2x cascaded Butterworth 2nd-order filters
    // Crossover 0: Low / Low-Mid split
    // Crossover 1: Low-Mid / High-Mid split
    // Crossover 2: High-Mid / High split

    struct CrossoverFilters
    {
        // Low-pass side (2 stages for LR4)
        juce::dsp::IIR::Filter<float> lowPass1;
        juce::dsp::IIR::Filter<float> lowPass2;

        // High-pass side (2 stages for LR4)
        juce::dsp::IIR::Filter<float> highPass1;
        juce::dsp::IIR::Filter<float> highPass2;
    };

    std::array<CrossoverFilters, NumCrossovers> crossovers;

    //==============================================================================
    // State

    std::array<float, NumCrossovers> crossoverFreqs = {250.0f, 2000.0f, 8000.0f};
    double currentSampleRate = 44100.0;
    int maxSamplesPerBlock = 512;

    // Temporary buffers for cascaded filtering
    juce::AudioBuffer<float> tempBuffer1;
    juce::AudioBuffer<float> tempBuffer2;

    //==============================================================================
    // Helper Methods

    /**
     * @brief Update filter coefficients for a crossover
     *
     * @param index Crossover index (0-2)
     */
    void updateCrossoverCoefficients(int index);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LinkwitzRileyCrossover)
};

} // namespace audioforge
