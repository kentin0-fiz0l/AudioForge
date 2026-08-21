#pragma once

#include <juce_core/juce_core.h>
#include <cmath>

namespace AudioForge {
namespace DSP {

/**
 * Parameter smoothing for click-free parameter changes.
 *
 * Uses exponential smoothing to gradually transition from current
 * to target value. This prevents audio clicks/pops when parameters
 * change during audio processing.
 *
 * Template parameter T is the value type (typically float or double).
 *
 * Example:
 * @code
 * ParameterSmoothing<float> gainSmoother;
 * gainSmoother.reset(sampleRate, 0.05); // 50ms ramp time
 * gainSmoother.setTargetValue(newGain);
 *
 * for (int i = 0; i < numSamples; ++i)
 * {
 *     float smoothGain = gainSmoother.getNextValue();
 *     output[i] = input[i] * smoothGain;
 * }
 * @endcode
 */
template <typename T>
class ParameterSmoothing
{
public:
    /**
     * Constructor. Creates an uninitialized smoother.
     * Call reset() before use.
     */
    ParameterSmoothing() = default;

    /**
     * Constructor with initialization.
     *
     * @param sampleRate Sample rate in Hz
     * @param rampTimeSeconds Smoothing time in seconds (default: 0.05 = 50ms)
     */
    ParameterSmoothing(double sampleRate, double rampTimeSeconds = 0.05)
    {
        reset(sampleRate, rampTimeSeconds);
    }

    /**
     * Reset the smoother with new sample rate and ramp time.
     *
     * @param sampleRate Sample rate in Hz
     * @param rampTimeSeconds Smoothing time in seconds
     */
    void reset(double sampleRate, double rampTimeSeconds)
    {
        jassert(sampleRate > 0.0 && rampTimeSeconds >= 0.0);

        this->sampleRate = sampleRate;
        this->rampTimeSeconds = rampTimeSeconds;

        // Calculate smoothing coefficient for exponential decay
        // coefficient = exp(-1 / (rampTime * sampleRate))
        if (rampTimeSeconds > 0.0)
        {
            double numSamples = rampTimeSeconds * sampleRate;
            coefficient = static_cast<T>(std::exp(-1.0 / numSamples));
        }
        else
        {
            coefficient = static_cast<T>(0.0);
        }

        currentValue = targetValue;
    }

    /**
     * Set both current and target value immediately (no smoothing).
     * Use this during initialization.
     *
     * @param value Value to set
     */
    void setCurrentAndTargetValue(T value)
    {
        currentValue = value;
        targetValue = value;
    }

    /**
     * Set target value. Current value will gradually approach this.
     *
     * @param value New target value
     */
    void setTargetValue(T value)
    {
        targetValue = value;
    }

    /**
     * Get current value without advancing.
     *
     * @return Current smoothed value
     */
    T getCurrentValue() const
    {
        return currentValue;
    }

    /**
     * Get target value.
     *
     * @return Target value
     */
    T getTargetValue() const
    {
        return targetValue;
    }

    /**
     * Get next smoothed value and advance state.
     * Call this once per sample during audio processing.
     *
     * @return Next smoothed value
     */
    T getNextValue()
    {
        // Exponential smoothing: y[n] = c * y[n-1] + (1 - c) * target
        currentValue = coefficient * currentValue + (static_cast<T>(1.0) - coefficient) * targetValue;
        return currentValue;
    }

    /**
     * Skip ahead by specified number of samples.
     * Useful for non-realtime processing.
     *
     * @param numSamples Number of samples to skip
     */
    void skip(int numSamples)
    {
        for (int i = 0; i < numSamples; ++i)
            getNextValue();
    }

    /**
     * Check if current value is approximately equal to target.
     *
     * @param threshold Tolerance for equality (default: 0.001)
     * @return true if current is within threshold of target
     */
    bool isSmoothing(T threshold = static_cast<T>(0.001)) const
    {
        return std::abs(currentValue - targetValue) > threshold;
    }

    /**
     * Set ramp time without changing sample rate.
     *
     * @param rampTimeSeconds New smoothing time in seconds
     */
    void setRampTime(double rampTimeSeconds)
    {
        reset(sampleRate, rampTimeSeconds);
    }

    /**
     * Get current ramp time.
     *
     * @return Ramp time in seconds
     */
    double getRampTime() const
    {
        return rampTimeSeconds;
    }

private:
    T currentValue = static_cast<T>(0);
    T targetValue = static_cast<T>(0);
    T coefficient = static_cast<T>(0);
    double sampleRate = 44100.0;
    double rampTimeSeconds = 0.05;
};

} // namespace DSP
} // namespace AudioForge
