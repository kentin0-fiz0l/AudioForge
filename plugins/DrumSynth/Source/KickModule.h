#pragma once

#include <cmath>

/**
 * Kick Drum Module
 *
 * Synthesis:
 * - Sine wave oscillator
 * - Pitch envelope (high to low sweep)
 * - Amplitude envelope
 * - Optional click/attack transient
 */
class KickModule
{
public:
    KickModule();

    /**
     * Trigger the kick drum.
     */
    void trigger();

    /**
     * Process one sample.
     */
    float processSample(double sampleRate, float pitch, float decay,
                       float click, float tone, float drive);

    /**
     * Check if the module is active.
     */
    bool isActive() const { return active; }

    /**
     * Reset the module.
     */
    void reset();

private:
    bool active = false;
    float phase = 0.0f;
    float envLevel = 0.0f;
    float pitchEnvLevel = 0.0f;

    float generateClickSample();
};
