#pragma once

#include <cmath>

/**
 * Snare Drum Module
 *
 * Synthesis:
 * - Filtered noise (snare rattle)
 * - Sine wave body tone
 * - Amplitude envelope
 * - Snap control for attack transient
 */
class SnareModule
{
public:
    SnareModule();

    /**
     * Trigger the snare drum.
     */
    void trigger();

    /**
     * Process one sample.
     */
    float processSample(double sampleRate, float tune, float snap,
                       float tone, float decay, float mix);

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
    float envLevel = 0.0f;
    float bodyPhase = 0.0f;
    float filterState1 = 0.0f;
    float filterState2 = 0.0f;

    float generateNoise();
    float applyBandpassFilter(float input, float cutoff, double sampleRate);
};
