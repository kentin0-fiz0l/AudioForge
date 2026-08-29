#pragma once

#include <dsp/AudioForgeDSP.h>

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
        Square = 2,
        Pulse = 3  // Square wave with adjustable pulse width
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
     * Set the pulse width for pulse waveforms.
     *
     * @param width Pulse width (0.05 to 0.95), default 0.5 (square wave)
     */
    void setPulseWidth(float width);

    /**
     * Reset the oscillator phase to 0.
     */
    void reset();

private:
    float phase = 0.0f;        // Current phase (0.0 to 1.0)
    float phaseIncrement = 0.0f; // Phase increment per sample
    float pulseWidth = 0.5f;   // Pulse width for pulse waveforms (0.05-0.95)
};
