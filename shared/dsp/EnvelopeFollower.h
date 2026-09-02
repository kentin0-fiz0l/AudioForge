/*
  ==============================================================================

    EnvelopeFollower.h
    Created: 1 Sep 2026
    Author:  AudioForge Team

    Tracks signal amplitude using RMS or Peak detection.
    Shared across Gate, Compressor, De-Esser, and Limiter plugins.

  ==============================================================================
*/

#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>
#include <cmath>

namespace audioforge
{

/**
 * @brief Envelope follower for dynamic processing
 *
 * Tracks signal amplitude using:
 * - RMS (Root Mean Square): Smoother, better for sustained sounds
 * - Peak: Faster response, better for transients
 *
 * Used by: Gate, Compressor, De-Esser, Limiter
 */
class EnvelopeFollower
{
public:
    enum DetectionMode
    {
        RMS,    ///< Root Mean Square (averaged energy)
        Peak    ///< Instantaneous peak
    };

    EnvelopeFollower();
    ~EnvelopeFollower() = default;

    //==============================================================================
    // Configuration

    /**
     * @brief Prepare the envelope follower for processing
     *
     * @param sampleRate Sample rate in Hz
     * @param maxBlockSize Maximum samples per block
     */
    void prepare(double sampleRate, int maxBlockSize);

    /**
     * @brief Set detection mode
     *
     * @param mode RMS or Peak
     */
    void setDetectionMode(DetectionMode mode) { detectionMode = mode; }

    /**
     * @brief Set RMS averaging window
     *
     * @param windowMs Window size in milliseconds (1-50ms)
     */
    void setRMSWindow(float windowMs);

    /**
     * @brief Set peak hold time
     *
     * @param holdMs Peak hold time in milliseconds (1-50ms)
     */
    void setPeakHoldTime(float holdMs);

    /**
     * @brief Reset internal state
     */
    void reset();

    //==============================================================================
    // Processing

    /**
     * @brief Detect envelope level from mono signal
     *
     * @param samples Audio samples to analyze
     * @param numSamples Number of samples
     * @return Envelope level (linear, 0-1 range)
     */
    float process(const float* samples, int numSamples);

    /**
     * @brief Detect envelope level from stereo signal
     *
     * @param leftSamples Left channel samples
     * @param rightSamples Right channel samples
     * @param numSamples Number of samples per channel
     * @return Envelope level (linear, 0-1 range, max of both channels)
     */
    float processStereo(const float* leftSamples, const float* rightSamples, int numSamples);

    //==============================================================================
    // Getters

    DetectionMode getDetectionMode() const { return detectionMode; }
    float getRMSWindow() const { return rmsWindowMs; }
    float getPeakHoldTime() const { return peakHoldMs; }

private:
    //==============================================================================
    // Detection Methods

    /**
     * @brief RMS detection (averaged energy)
     */
    float detectRMS(const float* samples, int numSamples);

    /**
     * @brief Peak detection (instantaneous peak)
     */
    float detectPeak(const float* samples, int numSamples);

    //==============================================================================
    // State

    DetectionMode detectionMode = RMS;
    double currentSampleRate = 44100.0;
    int maxSamplesPerBlock = 512;

    // RMS state
    float rmsWindowMs = 10.0f;
    int rmsWindowSamples = 441;
    juce::AudioBuffer<float> rmsBuffer;  // Circular buffer for RMS calculation
    int rmsBufferPosition = 0;
    float rmsSum = 0.0f;  // Running sum of squared samples

    // Peak state
    float peakHoldMs = 10.0f;
    float currentPeak = 0.0f;
    float peakDecayCoeff = 0.99f;  // Exponential decay coefficient

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnvelopeFollower)
};

} // namespace audioforge
