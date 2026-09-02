/*
  ==============================================================================

    MultibandEngine.h
    Created: 1 Sep 2026
    Author:  AudioForge Team

    Core multiband compression engine integrating:
    - LinkwitzRiley crossover (4-band frequency splitting)
    - 4x BandCompressor (independent compression per band)
    - Mid/Side processing per band
    - Solo/Bypass controls per band

  ==============================================================================
*/

#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>
#include <array>
#include "../../../shared/dsp/LinkwitzRiley.h"
#include "../../../shared/dsp/MidSide.h"
#include "BandCompressor.h"

namespace audioforge
{

/**
 * @brief Multiband compression engine
 *
 * Signal flow:
 * 1. Split stereo input into 4 frequency bands (LinkwitzRiley)
 * 2. Optional Mid/Side encoding per band
 * 3. Compress each band independently (4x BandCompressor)
 * 4. Optional Mid/Side decoding per band
 * 5. Apply solo/bypass per band
 * 6. Sum bands back to stereo output (phase-coherent)
 *
 * Features:
 * - 4-band processing (20-250Hz, 250Hz-2kHz, 2kHz-8kHz, 8kHz-20kHz)
 * - Per-band compression controls
 * - Mid/Side processing per band (compress center/stereo independently)
 * - Solo/Bypass per band
 */
class MultibandEngine
{
public:
    static constexpr int NumBands = 4;

    MultibandEngine();
    ~MultibandEngine() = default;

    //==============================================================================
    // Configuration

    /**
     * @brief Prepare the engine for processing
     *
     * @param sampleRate Sample rate in Hz
     * @param maxBlockSize Maximum samples per block
     */
    void prepare(double sampleRate, int maxBlockSize);

    /**
     * @brief Reset internal state
     */
    void reset();

    //==============================================================================
    // Crossover Controls

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

    //==============================================================================
    // Band Controls

    /**
     * @brief Get band compressor for parameter control
     *
     * @param bandIndex Band index (0-3)
     * @return Reference to band compressor
     */
    BandCompressor& getBandCompressor(int bandIndex);

    /**
     * @brief Enable/disable solo for a band
     *
     * @param bandIndex Band index (0-3)
     * @param enabled True to solo this band (mute others)
     */
    void setBandSolo(int bandIndex, bool enabled);

    /**
     * @brief Check if band is soloed
     *
     * @param bandIndex Band index (0-3)
     * @return True if band is soloed
     */
    bool isBandSoloed(int bandIndex) const;

    /**
     * @brief Enable/disable bypass for a band
     *
     * @param bandIndex Band index (0-3)
     * @param enabled True to bypass compression (pass uncompressed)
     */
    void setBandBypass(int bandIndex, bool enabled);

    /**
     * @brief Check if band is bypassed
     *
     * @param bandIndex Band index (0-3)
     * @return True if band is bypassed
     */
    bool isBandBypassed(int bandIndex) const;

    /**
     * @brief Enable/disable Mid/Side processing for a band
     *
     * @param bandIndex Band index (0-3)
     * @param enabled True to process Mid and Side independently
     */
    void setBandMidSideEnabled(int bandIndex, bool enabled);

    /**
     * @brief Check if Mid/Side is enabled for a band
     *
     * @param bandIndex Band index (0-3)
     * @return True if Mid/Side processing is enabled
     */
    bool isBandMidSideEnabled(int bandIndex) const;

    //==============================================================================
    // Processing

    /**
     * @brief Process stereo audio buffer
     *
     * @param buffer Stereo audio buffer (modified in-place)
     */
    void process(juce::AudioBuffer<float>& buffer);

    //==============================================================================
    // Metering

    /**
     * @brief Get gain reduction for a band
     *
     * @param bandIndex Band index (0-3)
     * @return Gain reduction in dB
     */
    float getBandGainReductionDb(int bandIndex) const;

private:
    //==============================================================================
    // DSP Components

    LinkwitzRileyCrossover crossover;
    std::array<BandCompressor, NumBands> bandCompressors;
    MidSideProcessor midSideProcessor;

    //==============================================================================
    // Band Buffers

    // Temporary buffers for band processing
    std::array<juce::AudioBuffer<float>, NumBands> bandBuffers;

    // Band split buffers (for crossover)
    std::array<float*, NumBands> bandSplitPtrs;
    std::array<const float*, NumBands> bandSumPtrs;

    //==============================================================================
    // State

    double currentSampleRate = 44100.0;
    int maxSamplesPerBlock = 512;

    // Per-band controls
    std::array<bool, NumBands> bandSolo = {false, false, false, false};
    std::array<bool, NumBands> bandBypass = {false, false, false, false};
    std::array<bool, NumBands> bandMidSideEnabled = {false, false, false, false};

    //==============================================================================
    // Helper Methods

    /**
     * @brief Check if any band is soloed
     *
     * @return True if at least one band is soloed
     */
    bool isAnyBandSoloed() const;

    /**
     * @brief Process one band (with optional Mid/Side)
     *
     * @param bandIndex Band index (0-3)
     * @param numSamples Number of samples to process
     */
    void processBand(int bandIndex, int numSamples);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultibandEngine)
};

} // namespace audioforge
