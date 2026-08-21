#pragma once

#include <juce_core/juce_core.h>
#include <cmath>
#include <algorithm>

namespace AudioForge {
namespace DSP {

/**
 * Common DSP utility functions for audio processing.
 *
 * Provides conversion functions, range mapping, and saturation
 * utilities used throughout the AudioForge DSP library.
 */
class Utilities
{
public:
    /**
     * Convert linear gain value to decibels.
     *
     * @param linear Linear gain value (0.0 = -inf dB, 1.0 = 0 dB)
     * @param minusInfinityDb Value to return for zero input (default: -100.0)
     * @return Gain in decibels
     */
    static float linearToDb(float linear, float minusInfinityDb = -100.0f)
    {
        return linear > 0.0f ? std::log10(linear) * 20.0f : minusInfinityDb;
    }

    /**
     * Convert decibel value to linear gain.
     *
     * @param db Gain in decibels
     * @return Linear gain value (1.0 = 0 dB)
     */
    static float dbToLinear(float db)
    {
        return std::pow(10.0f, db * 0.05f);
    }

    /**
     * Map normalized parameter (0-1) to specified range.
     *
     * @param normalizedValue Input value in range [0, 1]
     * @param minValue Minimum output value
     * @param maxValue Maximum output value
     * @param skew Skew factor for non-linear mapping (1.0 = linear, <1.0 = logarithmic, >1.0 = exponential)
     * @return Mapped value in range [minValue, maxValue]
     */
    static float mapNormalizedToRange(float normalizedValue, float minValue, float maxValue, float skew = 1.0f)
    {
        normalizedValue = juce::jlimit(0.0f, 1.0f, normalizedValue);

        if (skew != 1.0f && normalizedValue > 0.0f)
            normalizedValue = std::pow(normalizedValue, skew);

        return minValue + normalizedValue * (maxValue - minValue);
    }

    /**
     * Map value from specified range to normalized (0-1).
     *
     * @param value Input value in range [minValue, maxValue]
     * @param minValue Minimum input value
     * @param maxValue Maximum input value
     * @param skew Skew factor for non-linear mapping (1.0 = linear, <1.0 = logarithmic, >1.0 = exponential)
     * @return Normalized value in range [0, 1]
     */
    static float mapRangeToNormalized(float value, float minValue, float maxValue, float skew = 1.0f)
    {
        float normalized = (value - minValue) / (maxValue - minValue);
        normalized = juce::jlimit(0.0f, 1.0f, normalized);

        if (skew != 1.0f && normalized > 0.0f)
            normalized = std::pow(normalized, 1.0f / skew);

        return normalized;
    }

    /**
     * Hard clip signal to range [-1, +1].
     *
     * @param value Input value
     * @return Clipped value
     */
    static float hardClip(float value)
    {
        return juce::jlimit(-1.0f, 1.0f, value);
    }

    /**
     * Soft clip signal using tanh saturation.
     *
     * @param value Input value
     * @param threshold Threshold where saturation begins (0-1)
     * @return Saturated value
     */
    static float softClip(float value, float threshold = 0.8f)
    {
        if (std::abs(value) < threshold)
            return value;

        return std::tanh(value);
    }

    /**
     * Linear interpolation between two values.
     *
     * @param a Start value
     * @param b End value
     * @param t Interpolation factor [0, 1]
     * @return Interpolated value
     */
    static float lerp(float a, float b, float t)
    {
        return a + t * (b - a);
    }

    /**
     * Check if value is denormal (very small floating point number).
     *
     * @param value Value to check
     * @return true if denormal
     */
    static bool isDenormal(float value)
    {
        return std::abs(value) < 1.0e-15f;
    }

    /**
     * Flush denormals to zero.
     *
     * @param value Input value
     * @return Value with denormals flushed to zero
     */
    static float flushDenormal(float value)
    {
        return isDenormal(value) ? 0.0f : value;
    }

    /**
     * Convert frequency to normalized value (0-1) with logarithmic scaling.
     * Useful for frequency parameters (20 Hz - 20 kHz).
     *
     * @param frequency Frequency in Hz
     * @param minFreq Minimum frequency (default: 20 Hz)
     * @param maxFreq Maximum frequency (default: 20000 Hz)
     * @return Normalized value [0, 1]
     */
    static float frequencyToNormalized(float frequency, float minFreq = 20.0f, float maxFreq = 20000.0f)
    {
        frequency = juce::jlimit(minFreq, maxFreq, frequency);
        return std::log(frequency / minFreq) / std::log(maxFreq / minFreq);
    }

    /**
     * Convert normalized value (0-1) to frequency with logarithmic scaling.
     *
     * @param normalized Normalized value [0, 1]
     * @param minFreq Minimum frequency (default: 20 Hz)
     * @param maxFreq Maximum frequency (default: 20000 Hz)
     * @return Frequency in Hz
     */
    static float normalizedToFrequency(float normalized, float minFreq = 20.0f, float maxFreq = 20000.0f)
    {
        normalized = juce::jlimit(0.0f, 1.0f, normalized);
        return minFreq * std::pow(maxFreq / minFreq, normalized);
    }
};

} // namespace DSP
} // namespace AudioForge
