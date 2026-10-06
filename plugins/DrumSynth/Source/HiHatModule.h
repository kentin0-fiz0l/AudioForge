#pragma once

#include <cmath>

/**
 * Hi-Hat Module
 *
 * Synthesis:
 * - Highpass filtered noise
 * - Very short amplitude envelope, or a longer one when played open
 * - Tone control for filter cutoff
 * - Click control for attack sharpness
 *
 * There is one hat, as on a real kit: a closed hit cuts off an open one
 * that is still ringing. The ringing hat fades out over a few milliseconds
 * at its own level, so a soft hit does not cut it off in one step.
 */
class HiHatModule
{
public:
    HiHatModule();

    /**
     * Trigger the hi-hat, closed or open, at a level from 0 to 1.
     */
    void trigger(bool open = false, float level = 1.0f);

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

    /** How many times longer an open hat rings than a closed one. */
    static constexpr float openDecayRatio = 10.0f;

    /**
     * Level of an open hat against a closed one. Ringing for longer gives the
     * noise more chances to peak, so at equal level the open hat would come
     * out about 5 dB hotter.
     */
    static constexpr float openLevel = 0.6f;

    /** How fast a choked hat fades out, per second: 60 dB in about 12 ms. */
    static constexpr float chokeRate = 600.0f;

private:
    bool active = false;
    bool playedOpen = false;
    float hitLevel = 1.0f;
    float envLevel = 0.0f;
    float chokedLevel = 0.0f;   // What is left of a hat cut off by a new hit
    float filterState1 = 0.0f;
    float filterState2 = 0.0f;

    float hitGain() const { return hitLevel * (playedOpen ? openLevel : 1.0f); }

    float generateNoise();
    float applyHighpassFilter(float input, float cutoff, double sampleRate);
};
