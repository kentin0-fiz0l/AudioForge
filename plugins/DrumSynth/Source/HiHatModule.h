#pragma once

#include <cmath>

/**
 * Hi-Hat Module
 *
 * Synthesis:
 * - Highpass filtered noise
 * - Very short amplitude envelope
 * - Tone control for filter cutoff
 * - Click control for attack sharpness
 */
class HiHatModule
{
public:
    HiHatModule();

    /**
     * Trigger the hi-hat.
     */
    void trigger();

    /**
     * Process one sample.
     */
    float processSample(double sampleRate, float tune, float decay,
                       float tone, float click);

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
    float filterState1 = 0.0f;
    float filterState2 = 0.0f;

    float generateNoise();
    float applyHighpassFilter(float input, float cutoff, double sampleRate);
};
