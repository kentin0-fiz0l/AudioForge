#pragma once

#include "Oscillator.h"
#include "NoiseGenerator.h"

/**
 * Oscillator Bank for Enhanced BasicSynth
 *
 * Manages multiple oscillators with independent controls.
 * Supports 5 oscillators: 3 main, 1 sub-oscillator, 1 noise generator.
 */
class OscillatorBank
{
public:
    static constexpr int NUM_OSCILLATORS = 5;

    // Oscillator indices
    static constexpr int OSC_1 = 0;
    static constexpr int OSC_2 = 1;
    static constexpr int OSC_3 = 2;
    static constexpr int SUB_OSC = 3;  // -1 octave
    static constexpr int NOISE = 4;

    /**
     * Configuration for a single oscillator.
     */
    struct OscillatorConfig
    {
        float level = 0.5f;        // Volume (0.0-1.0)
        float detune = 0.0f;       // Semitones (-12 to +12)
        float pulseWidth = 0.5f;   // For pulse wave (0.05-0.95)
        int waveform = 0;          // 0=sine, 1=saw, 2=square, 3=pulse
        bool enabled = true;
    };

    OscillatorBank();

    /**
     * Set the base frequency for all oscillators.
     *
     * @param frequency Base frequency in Hz
     * @param sampleRate Current sample rate
     */
    void setFrequency(float frequency, double sampleRate);

    /**
     * Configure a specific oscillator.
     *
     * @param oscIndex Oscillator index (0-4)
     * @param config Configuration settings
     */
    void configure(int oscIndex, const OscillatorConfig& config);

    /**
     * Get the next sample from all enabled oscillators.
     * Automatically mixes all oscillators based on their levels.
     *
     * @return Mixed output sample in range [-1.0, 1.0]
     */
    float getNextSample();

    /**
     * Reset all oscillators to initial phase.
     */
    void reset();

private:
    Oscillator oscillators[NUM_OSCILLATORS];
    NoiseGenerator noiseGenerator;
    OscillatorConfig configs[NUM_OSCILLATORS];

    float baseFrequency = 440.0f;
    double currentSampleRate = 44100.0;

    /**
     * Convert semitones to frequency multiplier.
     *
     * @param semitones Detune amount in semitones
     * @return Frequency multiplier (e.g., 12 semitones = 2.0)
     */
    float semitonesToRatio(float semitones) const;
};
