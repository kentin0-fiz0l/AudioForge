#pragma once

#include <cmath>
#include <algorithm>

namespace AudioForge {
namespace DSP {

/**
 * Saturation algorithms modeling analog circuitry characteristics.
 *
 * Each algorithm produces different harmonic content:
 * - Even harmonics (2nd, 4th, 6th): Warm, musical character
 * - Odd harmonics (3rd, 5th, 7th): Bright, edgy character
 */
class SaturationAlgorithms
{
public:
    /**
     * Tape saturation - Soft saturation with even harmonics.
     * Models magnetic tape saturation with smooth compression.
     * Characteristics: Gentle, warm, primarily even harmonics.
     *
     * @param input Input sample
     * @return Saturated output
     */
    static float tape(float input)
    {
        // Soft knee tape saturation with even harmonic emphasis
        // Uses modified tanh with asymmetry for even harmonics
        const float x = input * 0.7f;  // Reduce drive for softer onset

        if (std::abs(x) < 0.1f)
            return input;  // Linear region for very small signals

        // Asymmetric soft clipping for even harmonics
        const float absX = std::abs(x);
        const float sign = (x > 0.0f) ? 1.0f : -1.0f;

        // Tape formula: smooth compression with bias
        const float compressed = (absX / (1.0f + absX * 0.6f));
        return sign * compressed * 1.4f;  // Restore level
    }

    /**
     * Tube saturation - Asymmetric with warm character.
     * Models vacuum tube saturation with strong even harmonics.
     * Characteristics: Asymmetric clipping (positive different from negative),
     * rich harmonics, warm and musical.
     *
     * @param input Input sample
     * @return Saturated output
     */
    static float tube(float input)
    {
        // Asymmetric tube saturation
        // Positive and negative halves behave differently (Class A/B behavior)

        if (input > 0.0f)
        {
            // Positive: harder saturation (grid clipping)
            const float x = input * 1.2f;
            return std::tanh(x) * 0.9f;
        }
        else
        {
            // Negative: softer saturation (cathode clipping)
            const float x = input * 0.8f;
            return std::tanh(x) * 1.05f;
        }
    }

    /**
     * Transistor saturation - Harder clipping with odd harmonics.
     * Models solid-state transistor saturation.
     * Characteristics: Sharper clipping, more odd harmonics, edgier sound.
     *
     * @param input Input sample
     * @return Saturated output
     */
    static float transistor(float input)
    {
        // Hard transistor clipping with softer knee
        const float x = input * 1.5f;

        // Polynomial approximation for transistor curve
        if (std::abs(x) < 0.7f)
            return input;  // Linear region
        else if (x > 0.7f)
        {
            // Positive clipping
            const float excess = x - 0.7f;
            return 0.7f + std::tanh(excess * 3.0f) * 0.3f;
        }
        else
        {
            // Negative clipping
            const float excess = x + 0.7f;
            return -0.7f + std::tanh(excess * 3.0f) * 0.3f;
        }
    }

    /**
     * Transformer saturation - Gentle even harmonics.
     * Models magnetic transformer core saturation.
     * Characteristics: Very smooth, subtle, mostly 2nd harmonic,
     * gentle compression.
     *
     * @param input Input sample
     * @return Saturated output
     */
    static float transformer(float input)
    {
        // Gentle transformer saturation
        // Based on B-H curve (magnetization) with soft knee

        const float x = input * 0.5f;  // Very gentle drive

        // Cubic soft clipping for smooth even harmonics
        const float x2 = x * x;
        const float x3 = x * x2;

        // Polynomial model of transformer saturation
        float output = input - 0.1f * x3;

        // Add slight asymmetry for even harmonic richness
        if (input > 0.0f)
            output *= 0.98f;
        else
            output *= 1.02f;

        // Gentle compression at extremes
        return std::tanh(output * 1.2f) * 0.9f;
    }

    /**
     * Diode saturation - Asymmetric hard clipping.
     * Models diode clipper circuit (common in guitar pedals).
     * Characteristics: Very asymmetric, rich harmonics,
     * aggressive clipping, fizzy top end.
     *
     * @param input Input sample
     * @return Saturated output
     */
    static float diode(float input)
    {
        // Diode clipper model with strong asymmetry
        // Forward bias (positive): ~0.7V threshold, soft knee
        // Reverse bias (negative): ~50V threshold, essentially linear

        if (input > 0.0f)
        {
            // Forward bias: soft clipping at low threshold
            const float threshold = 0.3f;
            if (input < threshold)
                return input;
            else
            {
                const float excess = input - threshold;
                return threshold + std::tanh(excess * 4.0f) * 0.4f;
            }
        }
        else
        {
            // Reverse bias: much higher threshold, nearly linear
            const float threshold = -2.0f;
            if (input > threshold)
                return input;
            else
            {
                const float excess = input - threshold;
                return threshold + std::tanh(excess * 0.5f) * 1.5f;
            }
        }
    }

    /**
     * Apply saturation with the specified mode.
     *
     * @param input Input sample
     * @param mode Saturation mode (0=Tape, 1=Tube, 2=Transistor, 3=Transformer, 4=Diode)
     * @return Saturated output
     */
    static float saturate(float input, int mode)
    {
        switch (mode)
        {
            case 0: return tape(input);
            case 1: return tube(input);
            case 2: return transistor(input);
            case 3: return transformer(input);
            case 4: return diode(input);
            default: return input;
        }
    }
};

} // namespace DSP
} // namespace AudioForge
