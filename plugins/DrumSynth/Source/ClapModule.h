#pragma once

#include <cmath>

/**
 * Clap Module
 *
 * Synthesis:
 * - Band-passed noise, the body of a hand clap
 * - Three short bursts 11 ms apart, as hands that do not land together
 * - A last one that rings on as the tail, the room answering
 * - Tone control for where the band sits
 * - Decay control for the length of the tail
 */
class ClapModule
{
public:
    /**
     * Trigger the clap.
     */
    void trigger();

    /**
     * Process one sample.
     *
     * @param tone  0 to 1: the band's centre, from 700 Hz to 2.5 kHz
     * @param decay Seconds for the tail to fall away
     */
    float processSample(double sampleRate, float tone, float decay);

    /**
     * Check if the module is active.
     */
    bool isActive() const { return active; }

    /**
     * Reset the module.
     */
    void reset();

    /** Time between one burst and the next, and from the last to the tail. */
    static constexpr float burstSpacing = 0.011f;

    /** Bursts before the tail. */
    static constexpr int numBursts = 3;

private:
    bool active = false;
    int samplesSinceTrigger = 0;
    float lowState = 0.0f;      // Band-pass filter state
    float bandState = 0.0f;
    unsigned int noiseSeed = 24680;

    float generateNoise();
    float applyBandpassFilter(float input, float centre, double sampleRate);
};
