#pragma once

#include <dsp/WaveformGenerators.h>

/**
 * Unison Voice
 *
 * Single detuned oscillator voice used in the unison ensemble.
 * Multiple UnisonVoices are combined to create the thick pad sound.
 */
class UnisonVoice
{
public:
    UnisonVoice();

    /**
     * Set the base frequency and detune offset.
     * @param baseFrequency Base frequency in Hz
     * @param detuneAmount Detune in cents (-100 to +100)
     * @param pan Stereo position (-1.0 = left, 1.0 = right)
     */
    void setFrequency(float baseFrequency, float detuneAmount, float pan);

    /**
     * Set the waveform type.
     * 0 = Sine, 1 = Saw, 2 = Square, 3 = Triangle
     */
    void setWaveform(int waveformType);

    /**
     * Process one sample.
     */
    float processSample(double sampleRate);

    /**
     * Reset the oscillator phase.
     */
    void reset();

    /**
     * Get the stereo pan value.
     */
    float getPan() const { return pan; }

private:
    float frequency = 440.0f;
    float phase = 0.0f;
    float pan = 0.0f; // -1.0 to 1.0
    int waveform = 1; // Default to saw wave

    float generateSample();
};
