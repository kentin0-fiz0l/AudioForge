#pragma once

#include <cmath>
#include <algorithm>

namespace AudioForge {
namespace DSP {

/**
 * Waveshaping algorithms for distortion and saturation effects.
 *
 * All functions take an input sample and return a shaped output.
 * Input range is typically -1 to +1 but can exceed for drive effects.
 */
class WaveShaping
{
public:
    /**
     * Soft clip using polynomial approximation.
     * Smooth saturation that approaches ±1 asymptotically.
     *
     * @param input Input sample
     * @return Shaped output
     */
    static float softClip(float input)
    {
        // Clamp to prevent overflow
        input = std::clamp(input, -3.0f, 3.0f);
        
        // Polynomial soft clipping (approximates tanh)
        // Formula: x - (x^3)/3 for small x, approaches ±1 for large x
        if (std::abs(input) < 1.0f)
            return input;
        else if (input > 0.0f)
            return (3.0f * input - input * input * input) / (3.0f + input * input);
        else
            return (3.0f * input + input * input * input) / (3.0f + input * input);
    }

    /**
     * Hard clip (digital clipping).
     * Sharp clipping at ±1 threshold.
     *
     * @param input Input sample
     * @return Clipped output
     */
    static float hardClip(float input)
    {
        return std::clamp(input, -1.0f, 1.0f);
    }

    /**
     * Hyperbolic tangent saturation.
     * Classic smooth saturation curve.
     *
     * @param input Input sample
     * @return Shaped output
     */
    static float tanhSaturation(float input)
    {
        return std::tanh(input);
    }

    /**
     * Cubic waveshaping.
     * Polynomial distortion: x - x³/3
     * Adds odd harmonics.
     *
     * @param input Input sample
     * @return Shaped output
     */
    static float cubic(float input)
    {
        // Clamp to prevent extreme values
        input = std::clamp(input, -2.0f, 2.0f);
        
        // Cubic: x - x³/3
        const float x3 = input * input * input;
        float output = input - (x3 / 3.0f);
        
        // Normalize to prevent exceeding ±1
        return std::clamp(output, -1.0f, 1.0f);
    }

    /**
     * Asymmetric waveshaping.
     * Different curves for positive and negative samples.
     * Adds even harmonics for warmth.
     *
     * @param input Input sample
     * @return Shaped output
     */
    static float asymmetric(float input)
    {
        // Clamp to prevent overflow
        input = std::clamp(input, -3.0f, 3.0f);
        
        if (input > 0.0f)
        {
            // Positive: harder clipping
            return std::tanh(input * 1.5f) * 0.85f;
        }
        else
        {
            // Negative: softer clipping
            return std::tanh(input * 0.8f) * 0.95f;
        }
    }

    /**
     * Apply waveshaping with the specified mode.
     *
     * @param input Input sample
     * @param mode Shaping mode (0=Soft, 1=Hard, 2=Tanh, 3=Cubic, 4=Asymmetric)
     * @return Shaped output
     */
    static float shape(float input, int mode)
    {
        switch (mode)
        {
            case 0: return softClip(input);
            case 1: return hardClip(input);
            case 2: return tanhSaturation(input);
            case 3: return cubic(input);
            case 4: return asymmetric(input);
            default: return input;
        }
    }
};

} // namespace DSP
} // namespace AudioForge
