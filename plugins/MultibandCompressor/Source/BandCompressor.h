/*
  ==============================================================================

    BandCompressor.h
    Created: 1 Sep 2026
    Author:  AudioForge Team

    Per-band compressor for multiband processing.
    Reuses EnvelopeFollower from shared DSP library.

  ==============================================================================
*/

#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>
#include "../../../shared/dsp/EnvelopeFollower.h"

namespace audioforge
{

/**
 * @brief Single-band compressor for multiband processing
 *
 * Features:
 * - Threshold, ratio, attack, release controls
 * - Makeup gain (auto or manual)
 * - RMS/Peak detection (via EnvelopeFollower)
 * - Smooth gain reduction (exponential curves)
 *
 * Used by: MultibandCompressor (4 instances per plugin)
 */
class BandCompressor
{
public:
    BandCompressor();
    ~BandCompressor() = default;

    //==============================================================================
    // Parameters

    /** Set threshold in dB (-60 to 0) */
    void setThreshold(float thresholdDb);

    /** Set compression ratio (1:1 to 20:1) */
    void setRatio(float ratio);

    /** Set attack time in milliseconds (0.1 to 100) */
    void setAttack(float attackMs);

    /** Set release time in milliseconds (10 to 1000) */
    void setRelease(float releaseMs);

    /** Set makeup gain in dB (-12 to +12) */
    void setMakeupGain(float gainDb);

    //==============================================================================
    // Processing

    /**
     * @brief Prepare the compressor for processing
     *
     * @param sampleRate Sample rate in Hz
     * @param maxBlockSize Maximum samples per block
     */
    void prepare(double sampleRate, int maxBlockSize);

    /**
     * @brief Process audio samples
     *
     * @param samples Audio samples to compress (modified in-place)
     * @param numSamples Number of samples to process
     */
    void process(float* samples, int numSamples);

    /**
     * @brief Reset internal state
     */
    void reset();

    //==============================================================================
    // Metering

    /** Get current gain reduction in dB */
    float getGainReductionDb() const;

    //==============================================================================
    // Detection Mode

    /** Set detection mode (RMS or Peak) */
    void setDetectionMode(EnvelopeFollower::DetectionMode mode);

    /** Set RMS window size in milliseconds (1 to 50) */
    void setRMSWindow(float windowMs);

private:
    //==============================================================================
    // DSP Components

    EnvelopeFollower envelopeFollower;

    //==============================================================================
    // Parameters (linear gain where applicable)

    float threshold = 0.316f;        // Threshold (linear, default -10 dB)
    float ratio = 4.0f;              // Compression ratio (default 4:1)
    float attackTimeMs = 10.0f;      // Attack time in ms
    float releaseTimeMs = 100.0f;    // Release time in ms
    float makeupGain = 1.0f;         // Makeup gain (linear, default 0 dB)

    // Envelope coefficients
    float attackCoeff = 0.99f;       // Exponential attack coefficient
    float releaseCoeff = 0.99f;      // Exponential release coefficient

    //==============================================================================
    // State

    double currentSampleRate = 44100.0;
    int maxSamplesPerBlock = 512;

    float currentGainReduction = 1.0f;  // Current gain reduction (linear, 0-1)

    //==============================================================================
    // Helper Methods

    /**
     * @brief Calculate gain reduction for a given input level
     *
     * @param inputLevel Input level (linear, 0-1)
     * @return Gain reduction to apply (linear, 0-1)
     */
    float calculateGainReduction(float inputLevel);

    /**
     * @brief Smooth gain reduction with exponential envelope
     *
     * @param targetGain Target gain reduction
     * @return Smoothed gain reduction
     */
    float smoothGainReduction(float targetGain);

    /**
     * @brief Calculate time constant coefficient for exponential smoothing
     *
     * @param timeMs Time constant in milliseconds
     * @return Smoothing coefficient (0-1)
     */
    float timeConstantToCoeff(float timeMs);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BandCompressor)
};

} // namespace audioforge
