#pragma once

#include <cmath>

/**
 * Saturation/Distortion Effect
 *
 * Adds analog-style warmth and harmonic content using waveshaping.
 * Provides multiple saturation curves:
 * - Soft clip: Gentle tape-style saturation
 * - Hard clip: Aggressive distortion
 * - Tube: Asymmetric tube-style warmth
 *
 * Classic saturation adds odd harmonics and warmth to sterile digital sound.
 */
class Saturation
{
public:
    enum class SaturationType
    {
        SoftClip = 0,
        HardClip = 1,
        Tube = 2
    };

    Saturation();

    /**
     * Process one sample through saturation.
     *
     * @param input Input sample
     * @return Saturated output sample
     */
    float processSample(float input);

    /**
     * Set saturation parameters.
     *
     * @param drive Drive/gain amount (0.0-1.0, mapped to 1x-10x gain)
     * @param mix Dry/wet mix (0.0=dry, 1.0=wet)
     * @param type Saturation curve type
     */
    void setParameters(float drive, float mix, SaturationType type);

    /**
     * Reset effect state.
     */
    void reset();

private:
    // Parameters
    float driveAmount = 1.0f;     // 1.0 to 10.0 (internal gain)
    float dryWetMix = 0.0f;       // 0.0 to 1.0
    SaturationType saturationType = SaturationType::SoftClip;

    /**
     * Soft clipping waveshaper (tanh-based).
     * Gentle saturation, adds odd harmonics.
     *
     * @param x Input value
     * @return Shaped value
     */
    float softClip(float x) const;

    /**
     * Hard clipping waveshaper.
     * Aggressive distortion, adds many harmonics.
     *
     * @param x Input value
     * @return Clipped value (-1.0 to 1.0)
     */
    float hardClip(float x) const;

    /**
     * Tube-style asymmetric waveshaper.
     * Warmer, more musical than symmetric clipping.
     *
     * @param x Input value
     * @return Shaped value
     */
    float tubeShape(float x) const;
};
