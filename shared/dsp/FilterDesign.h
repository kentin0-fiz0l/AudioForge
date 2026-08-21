#pragma once

#include <juce_core/juce_core.h>
#include <cmath>
#include <array>

namespace AudioForge {
namespace DSP {

/**
 * Biquad filter coefficients.
 *
 * Standard biquad transfer function:
 * H(z) = (b0 + b1*z^-1 + b2*z^-2) / (a0 + a1*z^-1 + a2*z^-2)
 *
 * Typically a0 is normalized to 1.0, so we store:
 * y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2] - a1*y[n-1] - a2*y[n-2]
 */
struct BiquadCoefficients
{
    float b0 = 1.0f;
    float b1 = 0.0f;
    float b2 = 0.0f;
    float a1 = 0.0f;
    float a2 = 0.0f;
};

/**
 * Biquad filter processor.
 *
 * Processes audio using biquad filter coefficients.
 * Maintains separate state for each channel.
 */
class BiquadFilter
{
public:
    /**
     * Constructor.
     */
    BiquadFilter() = default;

    /**
     * Set filter coefficients.
     *
     * @param newCoeffs New coefficients
     */
    void setCoefficients(const BiquadCoefficients& newCoeffs)
    {
        coeffs = newCoeffs;
    }

    /**
     * Reset filter state (clear delay lines).
     */
    void reset()
    {
        x1 = x2 = y1 = y2 = 0.0f;
    }

    /**
     * Process a single sample.
     *
     * @param input Input sample
     * @return Filtered output sample
     */
    float processSample(float input)
    {
        // Direct Form II Transposed implementation
        float output = coeffs.b0 * input + x1;
        x1 = coeffs.b1 * input - coeffs.a1 * output + x2;
        x2 = coeffs.b2 * input - coeffs.a2 * output;

        return output;
    }

    /**
     * Process a buffer of samples in-place.
     *
     * @param samples Pointer to sample buffer
     * @param numSamples Number of samples to process
     */
    void processBuffer(float* samples, int numSamples)
    {
        for (int i = 0; i < numSamples; ++i)
            samples[i] = processSample(samples[i]);
    }

private:
    BiquadCoefficients coeffs;
    float x1 = 0.0f, x2 = 0.0f;  // Input delay line
    float y1 = 0.0f, y2 = 0.0f;  // Output delay line
};

/**
 * Biquad filter coefficient calculator.
 *
 * Calculates coefficients for various filter types.
 * All frequency parameters should be normalized to sample rate.
 */
class FilterDesign
{
public:
    /**
     * Create low-pass filter coefficients.
     *
     * @param frequency Cutoff frequency in Hz
     * @param sampleRate Sample rate in Hz
     * @param Q Quality factor (default: 0.707 = Butterworth)
     * @return Biquad coefficients
     */
    static BiquadCoefficients makeLowPass(float frequency, float sampleRate, float Q = 0.707f)
    {
        jassert(frequency > 0.0f && frequency < sampleRate * 0.5f);
        jassert(Q > 0.0f);

        float w0 = 2.0f * juce::MathConstants<float>::pi * frequency / sampleRate;
        float cosw0 = std::cos(w0);
        float sinw0 = std::sin(w0);
        float alpha = sinw0 / (2.0f * Q);

        BiquadCoefficients c;
        float a0 = 1.0f + alpha;

        c.b0 = ((1.0f - cosw0) / 2.0f) / a0;
        c.b1 = (1.0f - cosw0) / a0;
        c.b2 = ((1.0f - cosw0) / 2.0f) / a0;
        c.a1 = (-2.0f * cosw0) / a0;
        c.a2 = (1.0f - alpha) / a0;

        return c;
    }

    /**
     * Create high-pass filter coefficients.
     *
     * @param frequency Cutoff frequency in Hz
     * @param sampleRate Sample rate in Hz
     * @param Q Quality factor (default: 0.707 = Butterworth)
     * @return Biquad coefficients
     */
    static BiquadCoefficients makeHighPass(float frequency, float sampleRate, float Q = 0.707f)
    {
        jassert(frequency > 0.0f && frequency < sampleRate * 0.5f);
        jassert(Q > 0.0f);

        float w0 = 2.0f * juce::MathConstants<float>::pi * frequency / sampleRate;
        float cosw0 = std::cos(w0);
        float sinw0 = std::sin(w0);
        float alpha = sinw0 / (2.0f * Q);

        BiquadCoefficients c;
        float a0 = 1.0f + alpha;

        c.b0 = ((1.0f + cosw0) / 2.0f) / a0;
        c.b1 = -(1.0f + cosw0) / a0;
        c.b2 = ((1.0f + cosw0) / 2.0f) / a0;
        c.a1 = (-2.0f * cosw0) / a0;
        c.a2 = (1.0f - alpha) / a0;

        return c;
    }

    /**
     * Create band-pass filter coefficients.
     *
     * @param frequency Center frequency in Hz
     * @param sampleRate Sample rate in Hz
     * @param Q Quality factor (bandwidth control)
     * @return Biquad coefficients
     */
    static BiquadCoefficients makeBandPass(float frequency, float sampleRate, float Q = 1.0f)
    {
        jassert(frequency > 0.0f && frequency < sampleRate * 0.5f);
        jassert(Q > 0.0f);

        float w0 = 2.0f * juce::MathConstants<float>::pi * frequency / sampleRate;
        float cosw0 = std::cos(w0);
        float sinw0 = std::sin(w0);
        float alpha = sinw0 / (2.0f * Q);

        BiquadCoefficients c;
        float a0 = 1.0f + alpha;

        c.b0 = alpha / a0;
        c.b1 = 0.0f;
        c.b2 = -alpha / a0;
        c.a1 = (-2.0f * cosw0) / a0;
        c.a2 = (1.0f - alpha) / a0;

        return c;
    }

    /**
     * Create notch (band-stop) filter coefficients.
     *
     * @param frequency Notch frequency in Hz
     * @param sampleRate Sample rate in Hz
     * @param Q Quality factor (notch width control)
     * @return Biquad coefficients
     */
    static BiquadCoefficients makeNotch(float frequency, float sampleRate, float Q = 1.0f)
    {
        jassert(frequency > 0.0f && frequency < sampleRate * 0.5f);
        jassert(Q > 0.0f);

        float w0 = 2.0f * juce::MathConstants<float>::pi * frequency / sampleRate;
        float cosw0 = std::cos(w0);
        float sinw0 = std::sin(w0);
        float alpha = sinw0 / (2.0f * Q);

        BiquadCoefficients c;
        float a0 = 1.0f + alpha;

        c.b0 = 1.0f / a0;
        c.b1 = (-2.0f * cosw0) / a0;
        c.b2 = 1.0f / a0;
        c.a1 = (-2.0f * cosw0) / a0;
        c.a2 = (1.0f - alpha) / a0;

        return c;
    }

    /**
     * Create low-shelf filter coefficients.
     *
     * @param frequency Transition frequency in Hz
     * @param sampleRate Sample rate in Hz
     * @param gainDb Gain in dB (positive = boost, negative = cut)
     * @param Q Shelf slope (default: 0.707)
     * @return Biquad coefficients
     */
    static BiquadCoefficients makeLowShelf(float frequency, float sampleRate, float gainDb, float Q = 0.707f)
    {
        jassert(frequency > 0.0f && frequency < sampleRate * 0.5f);
        jassert(Q > 0.0f);

        float A = std::pow(10.0f, gainDb / 40.0f);
        float w0 = 2.0f * juce::MathConstants<float>::pi * frequency / sampleRate;
        float cosw0 = std::cos(w0);
        float sinw0 = std::sin(w0);
        float alpha = sinw0 / (2.0f * Q);
        float beta = std::sqrt(A) / Q;

        BiquadCoefficients c;
        float a0 = (A + 1.0f) + (A - 1.0f) * cosw0 + beta * sinw0;

        c.b0 = (A * ((A + 1.0f) - (A - 1.0f) * cosw0 + beta * sinw0)) / a0;
        c.b1 = (2.0f * A * ((A - 1.0f) - (A + 1.0f) * cosw0)) / a0;
        c.b2 = (A * ((A + 1.0f) - (A - 1.0f) * cosw0 - beta * sinw0)) / a0;
        c.a1 = (-2.0f * ((A - 1.0f) + (A + 1.0f) * cosw0)) / a0;
        c.a2 = ((A + 1.0f) + (A - 1.0f) * cosw0 - beta * sinw0) / a0;

        return c;
    }

    /**
     * Create high-shelf filter coefficients.
     *
     * @param frequency Transition frequency in Hz
     * @param sampleRate Sample rate in Hz
     * @param gainDb Gain in dB (positive = boost, negative = cut)
     * @param Q Shelf slope (default: 0.707)
     * @return Biquad coefficients
     */
    static BiquadCoefficients makeHighShelf(float frequency, float sampleRate, float gainDb, float Q = 0.707f)
    {
        jassert(frequency > 0.0f && frequency < sampleRate * 0.5f);
        jassert(Q > 0.0f);

        float A = std::pow(10.0f, gainDb / 40.0f);
        float w0 = 2.0f * juce::MathConstants<float>::pi * frequency / sampleRate;
        float cosw0 = std::cos(w0);
        float sinw0 = std::sin(w0);
        float alpha = sinw0 / (2.0f * Q);
        float beta = std::sqrt(A) / Q;

        BiquadCoefficients c;
        float a0 = (A + 1.0f) - (A - 1.0f) * cosw0 + beta * sinw0;

        c.b0 = (A * ((A + 1.0f) + (A - 1.0f) * cosw0 + beta * sinw0)) / a0;
        c.b1 = (-2.0f * A * ((A - 1.0f) + (A + 1.0f) * cosw0)) / a0;
        c.b2 = (A * ((A + 1.0f) + (A - 1.0f) * cosw0 - beta * sinw0)) / a0;
        c.a1 = (2.0f * ((A - 1.0f) - (A + 1.0f) * cosw0)) / a0;
        c.a2 = ((A + 1.0f) - (A - 1.0f) * cosw0 - beta * sinw0) / a0;

        return c;
    }

    /**
     * Create parametric EQ (peaking bell curve) filter coefficients.
     *
     * @param frequency Center frequency in Hz
     * @param sampleRate Sample rate in Hz
     * @param gainDb Gain in dB (positive = boost, negative = cut)
     * @param Q Quality factor (bandwidth control: higher = narrower)
     * @return Biquad coefficients
     */
    static BiquadCoefficients makePeakingEQ(float frequency, float sampleRate, float gainDb, float Q = 1.0f)
    {
        jassert(frequency > 0.0f && frequency < sampleRate * 0.5f);
        jassert(Q > 0.0f);

        float A = std::pow(10.0f, gainDb / 40.0f);
        float w0 = 2.0f * juce::MathConstants<float>::pi * frequency / sampleRate;
        float cosw0 = std::cos(w0);
        float sinw0 = std::sin(w0);
        float alpha = sinw0 / (2.0f * Q);

        BiquadCoefficients c;
        float a0 = 1.0f + alpha / A;

        c.b0 = (1.0f + alpha * A) / a0;
        c.b1 = (-2.0f * cosw0) / a0;
        c.b2 = (1.0f - alpha * A) / a0;
        c.a1 = (-2.0f * cosw0) / a0;
        c.a2 = (1.0f - alpha / A) / a0;

        return c;
    }

    /**
     * Create all-pass filter coefficients (phase shift only, no magnitude change).
     *
     * @param frequency Center frequency in Hz
     * @param sampleRate Sample rate in Hz
     * @param Q Quality factor
     * @return Biquad coefficients
     */
    static BiquadCoefficients makeAllPass(float frequency, float sampleRate, float Q = 0.707f)
    {
        jassert(frequency > 0.0f && frequency < sampleRate * 0.5f);
        jassert(Q > 0.0f);

        float w0 = 2.0f * juce::MathConstants<float>::pi * frequency / sampleRate;
        float cosw0 = std::cos(w0);
        float sinw0 = std::sin(w0);
        float alpha = sinw0 / (2.0f * Q);

        BiquadCoefficients c;
        float a0 = 1.0f + alpha;

        c.b0 = (1.0f - alpha) / a0;
        c.b1 = (-2.0f * cosw0) / a0;
        c.b2 = (1.0f + alpha) / a0;
        c.a1 = (-2.0f * cosw0) / a0;
        c.a2 = (1.0f - alpha) / a0;

        return c;
    }
};

} // namespace DSP
} // namespace AudioForge
