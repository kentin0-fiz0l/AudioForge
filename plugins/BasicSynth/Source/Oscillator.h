#pragma once

#include <cmath>

/**
 * Audio Oscillator
 *
 * Generates waveforms for synthesis.
 * Supports sine, sawtooth, and square waves.
 */
class Oscillator
{
public:
    enum class Waveform
    {
        Sine = 0,
        Sawtooth = 1,
        Square = 2
    };

    Oscillator();

    /**
     * Set the oscillator frequency.
     *
     * @param frequency Frequency in Hz
     * @param sampleRate Current sample rate
     */
    void setFrequency(float frequency, double sampleRate);

    /**
     * Get the next sample from the oscillator.
     *
     * @param waveform Waveform to generate
     * @return Sample value in range [-1.0, 1.0]
     */
    float getNextSample(Waveform waveform);

    /**
     * Reset the oscillator phase to 0.
     */
    void reset();

private:
    float phase = 0.0f;        // Current phase (0.0 to 1.0)
    float phaseIncrement = 0.0f; // Phase increment per sample

    // Waveform generators
    float generateSine();
    float generateSawtooth();
    float generateSquare();
};
