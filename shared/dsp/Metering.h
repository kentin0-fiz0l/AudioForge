#pragma once

#include <juce_core/juce_core.h>
#include <atomic>
#include <cmath>
#include "Utilities.h"

namespace AudioForge {
namespace DSP {

/**
 * Peak meter with configurable decay rate.
 *
 * Tracks the peak level of an audio signal with smooth decay,
 * suitable for VU-style metering displays.
 */
class PeakMeter
{
public:
    /**
     * Constructor.
     */
    PeakMeter() = default;

    /**
     * Reset meter state and configure decay.
     *
     * @param sampleRate Sample rate in Hz
     * @param decayTimeSeconds Time for level to decay by 60 dB (default: 1.0s)
     */
    void reset(double sampleRate, double decayTimeSeconds = 1.0)
    {
        jassert(sampleRate > 0.0 && decayTimeSeconds > 0.0);

        this->sampleRate = sampleRate;
        this->decayTimeSeconds = decayTimeSeconds;

        // Calculate decay coefficient
        // For 60 dB decay over decayTime: coefficient = 10^(-60 / (20 * decayTime * sampleRate))
        double decayDbPerSample = 60.0 / (decayTimeSeconds * sampleRate);
        decayCoefficient = static_cast<float>(std::pow(10.0, -decayDbPerSample / 20.0));

        currentPeak = 0.0f;
    }

    /**
     * Process a single sample.
     *
     * @param sample Input sample
     */
    void processSample(float sample)
    {
        float absSample = std::abs(sample);

        if (absSample > currentPeak)
        {
            currentPeak = absSample;
        }
        else
        {
            currentPeak *= decayCoefficient;
        }
    }

    /**
     * Process a buffer of samples.
     *
     * @param samples Pointer to sample buffer
     * @param numSamples Number of samples to process
     */
    void processBuffer(const float* samples, int numSamples)
    {
        for (int i = 0; i < numSamples; ++i)
            processSample(samples[i]);
    }

    /**
     * Get current peak level (linear).
     *
     * @return Peak level [0, 1+]
     */
    float getPeakLevel() const
    {
        return currentPeak;
    }

    /**
     * Get current peak level in dB.
     *
     * @param minusInfinityDb Value to return for silence (default: -100 dB)
     * @return Peak level in dB
     */
    float getPeakLevelDb(float minusInfinityDb = -100.0f) const
    {
        return Utilities::linearToDb(currentPeak, minusInfinityDb);
    }

    /**
     * Clear the current peak (reset to silence).
     */
    void clear()
    {
        currentPeak = 0.0f;
    }

private:
    float currentPeak = 0.0f;
    float decayCoefficient = 0.999f;
    double sampleRate = 44100.0;
    double decayTimeSeconds = 1.0;
};

/**
 * RMS (Root Mean Square) meter with configurable window size.
 *
 * Calculates the RMS level of an audio signal over a sliding window,
 * providing a measure of perceived loudness.
 */
class RMSMeter
{
public:
    /**
     * Constructor.
     */
    RMSMeter() = default;

    /**
     * Reset meter state and configure window.
     *
     * @param sampleRate Sample rate in Hz
     * @param windowTimeSeconds RMS averaging window in seconds (default: 0.3s)
     */
    void reset(double sampleRate, double windowTimeSeconds = 0.3)
    {
        jassert(sampleRate > 0.0 && windowTimeSeconds > 0.0);

        this->sampleRate = sampleRate;
        this->windowTimeSeconds = windowTimeSeconds;

        windowSize = static_cast<int>(windowTimeSeconds * sampleRate);
        windowSize = juce::jmax(1, windowSize);

        sumOfSquares = 0.0;
        sampleCount = 0;
        buffer.resize(windowSize, 0.0f);
        writeIndex = 0;
    }

    /**
     * Process a single sample.
     *
     * @param sample Input sample
     */
    void processSample(float sample)
    {
        // Remove old sample from sum
        float oldSample = buffer[writeIndex];
        sumOfSquares -= oldSample * oldSample;

        // Add new sample to sum
        buffer[writeIndex] = sample;
        sumOfSquares += sample * sample;

        // Advance write position
        writeIndex = (writeIndex + 1) % windowSize;

        // Track how many samples we've processed
        if (sampleCount < windowSize)
            ++sampleCount;
    }

    /**
     * Process a buffer of samples.
     *
     * @param samples Pointer to sample buffer
     * @param numSamples Number of samples to process
     */
    void processBuffer(const float* samples, int numSamples)
    {
        for (int i = 0; i < numSamples; ++i)
            processSample(samples[i]);
    }

    /**
     * Get current RMS level (linear).
     *
     * @return RMS level [0, 1+]
     */
    float getRMSLevel() const
    {
        if (sampleCount == 0)
            return 0.0f;

        double meanSquare = sumOfSquares / static_cast<double>(sampleCount);
        return static_cast<float>(std::sqrt(juce::jmax(0.0, meanSquare)));
    }

    /**
     * Get current RMS level in dB.
     *
     * @param minusInfinityDb Value to return for silence (default: -100 dB)
     * @return RMS level in dB
     */
    float getRMSLevelDb(float minusInfinityDb = -100.0f) const
    {
        return Utilities::linearToDb(getRMSLevel(), minusInfinityDb);
    }

    /**
     * Clear the meter (reset to silence).
     */
    void clear()
    {
        sumOfSquares = 0.0;
        sampleCount = 0;
        std::fill(buffer.begin(), buffer.end(), 0.0f);
        writeIndex = 0;
    }

private:
    std::vector<float> buffer;
    int windowSize = 0;
    int writeIndex = 0;
    int sampleCount = 0;
    double sumOfSquares = 0.0;
    double sampleRate = 44100.0;
    double windowTimeSeconds = 0.3;
};

/**
 * Thread-safe meter for communicating levels from audio thread to GUI.
 *
 * Uses atomic operations for lock-free communication between threads.
 */
class ThreadSafeMeter
{
public:
    /**
     * Constructor.
     */
    ThreadSafeMeter() = default;

    /**
     * Update the meter value (call from audio thread).
     *
     * @param level New level value
     */
    void updateLevel(float level)
    {
        currentLevel.store(level, std::memory_order_relaxed);
    }

    /**
     * Get the current meter value (call from GUI thread).
     *
     * @return Current level
     */
    float getLevel() const
    {
        return currentLevel.load(std::memory_order_relaxed);
    }

    /**
     * Get the current meter value in dB (call from GUI thread).
     *
     * @param minusInfinityDb Value to return for silence (default: -100 dB)
     * @return Current level in dB
     */
    float getLevelDb(float minusInfinityDb = -100.0f) const
    {
        return Utilities::linearToDb(getLevel(), minusInfinityDb);
    }

    /**
     * Clear the meter (reset to silence).
     */
    void clear()
    {
        currentLevel.store(0.0f, std::memory_order_relaxed);
    }

private:
    std::atomic<float> currentLevel { 0.0f };
};

} // namespace DSP
} // namespace AudioForge
