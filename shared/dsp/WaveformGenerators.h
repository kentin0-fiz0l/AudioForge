#pragma once

#include <juce_core/juce_core.h>
#include <cmath>

namespace AudioForge {
namespace DSP {

/**
 * Waveform Generation Utilities
 *
 * Provides efficient waveform generation for audio synthesis.
 * All generators output values in the range [-1.0, 1.0].
 */
class WaveformGenerators
{
public:
    /**
     * Generate a sine wave sample.
     *
     * @param phase Current phase (0.0 to 1.0)
     * @return Sample value [-1.0, 1.0]
     */
    static float sine(float phase)
    {
        return std::sin(phase * 2.0f * juce::MathConstants<float>::pi);
    }

    /**
     * Generate a sawtooth wave sample.
     *
     * Naive implementation - produces aliasing at high frequencies.
     * For anti-aliased version, use sawtoothPolyBLEP().
     *
     * @param phase Current phase (0.0 to 1.0)
     * @return Sample value [-1.0, 1.0]
     */
    static float sawtooth(float phase)
    {
        // Ramp from -1 to +1
        return (phase * 2.0f) - 1.0f;
    }

    /**
     * Generate a square wave sample.
     *
     * Naive implementation - produces aliasing at high frequencies.
     * For anti-aliased version, use squarePolyBLEP().
     *
     * @param phase Current phase (0.0 to 1.0)
     * @return Sample value [-1.0, 1.0]
     */
    static float square(float phase)
    {
        // -1 for first half, +1 for second half
        return (phase < 0.5f) ? -1.0f : 1.0f;
    }

    /**
     * Generate a triangle wave sample.
     *
     * @param phase Current phase (0.0 to 1.0)
     * @return Sample value [-1.0, 1.0]
     */
    static float triangle(float phase)
    {
        // Rise from -1 to +1 in first half, fall back to -1 in second half
        if (phase < 0.5f)
            return -1.0f + (4.0f * phase);
        else
            return 3.0f - (4.0f * phase);
    }

    /**
     * Generate white noise sample.
     *
     * @return Random sample value [-1.0, 1.0]
     */
    static float whiteNoise()
    {
        static juce::Random random;
        return random.nextFloat() * 2.0f - 1.0f;
    }

    /**
     * Convert MIDI note number to frequency in Hz.
     *
     * Uses standard tuning: A4 (MIDI 69) = 440 Hz
     * Formula: f = 440 * 2^((n-69)/12)
     *
     * @param midiNoteNumber MIDI note (0-127)
     * @return Frequency in Hz
     */
    static float midiNoteToFrequency(int midiNoteNumber)
    {
        return 440.0f * std::pow(2.0f, (midiNoteNumber - 69) / 12.0f);
    }

    /**
     * Calculate phase increment for a given frequency.
     *
     * Phase increment is the amount to advance phase per sample.
     * Phase wraps around at 1.0.
     *
     * @param frequency Frequency in Hz
     * @param sampleRate Sample rate in Hz
     * @return Phase increment per sample
     */
    static float calculatePhaseIncrement(float frequency, double sampleRate)
    {
        return frequency / static_cast<float>(sampleRate);
    }

    /**
     * Advance phase and wrap to [0.0, 1.0] range.
     *
     * @param phase Current phase (will be modified)
     * @param phaseIncrement Amount to advance
     */
    static void advancePhase(float& phase, float phaseIncrement)
    {
        phase += phaseIncrement;

        // Wrap phase to [0, 1] using fmod for safety (avoids infinite loops)
        if (phase >= 1.0f)
            phase = std::fmod(phase, 1.0f);
        else if (phase < 0.0f)
            phase = 1.0f + std::fmod(phase, 1.0f);
    }

    /**
     * PolyBLEP residual for anti-aliasing.
     *
     * PolyBLEP (Polynomial Bandlimited Step) reduces aliasing
     * in discontinuous waveforms like sawtooth and square.
     *
     * This is the residual function that should be subtracted
     * from the naive waveform at discontinuities.
     *
     * @param t Normalized distance from discontinuity
     * @return Residual value to subtract
     */
    static float polyBLEP(float t)
    {
        if (t < 1.0f)
        {
            t = 2.0f * t - 1.0f;
            return t - (t * t * t) / 3.0f;
        }
        else if (t > -1.0f)
        {
            t = 2.0f * t + 1.0f;
            return (t * t * t) / 3.0f - t;
        }
        return 0.0f;
    }

    /**
     * Generate an anti-aliased sawtooth wave using PolyBLEP.
     *
     * @param phase Current phase (0.0 to 1.0)
     * @param phaseIncrement Phase increment per sample
     * @return Sample value [-1.0, 1.0]
     */
    static float sawtoothPolyBLEP(float phase, float phaseIncrement)
    {
        float sample = sawtooth(phase);

        // Apply PolyBLEP at discontinuity (phase = 0)
        sample -= polyBLEP(phase / phaseIncrement);

        return sample;
    }

    /**
     * Generate an anti-aliased square wave using PolyBLEP.
     *
     * @param phase Current phase (0.0 to 1.0)
     * @param phaseIncrement Phase increment per sample
     * @return Sample value [-1.0, 1.0]
     */
    static float squarePolyBLEP(float phase, float phaseIncrement)
    {
        float sample = square(phase);

        // Apply PolyBLEP at both discontinuities (phase = 0 and 0.5)
        sample -= polyBLEP(phase / phaseIncrement);
        sample += polyBLEP((phase - 0.5f) / phaseIncrement);

        return sample;
    }
};

} // namespace DSP
} // namespace AudioForge
