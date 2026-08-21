#pragma once

#include <juce_core/juce_core.h>
#include <cmath>

namespace AudioForge {
namespace DSP {

/**
 * Stereo panning and width control algorithms.
 *
 * Provides various panning laws and stereo manipulation utilities
 * for professional audio applications.
 */
class PanningAlgorithms
{
public:
    /**
     * Stereo pan/width control result.
     */
    struct StereoGains
    {
        float leftGain;
        float rightGain;
    };

    /**
     * Constant-power panning (sin/cos law).
     *
     * Maintains constant perceived loudness when panning.
     * Total power remains constant: leftGain^2 + rightGain^2 = 1
     *
     * @param pan Pan position [-1.0 = full left, 0.0 = center, +1.0 = full right]
     * @return Stereo gains for left and right channels
     */
    static StereoGains constantPowerPan(float pan)
    {
        pan = juce::jlimit(-1.0f, 1.0f, pan);

        // Map pan [-1, +1] to angle [0, pi/2]
        float angle = (pan + 1.0f) * 0.25f * juce::MathConstants<float>::pi;

        StereoGains gains;
        gains.leftGain = std::cos(angle);
        gains.rightGain = std::sin(angle);

        return gains;
    }

    /**
     * Linear panning.
     *
     * Simple linear crossfade between left and right.
     * May result in perceived loudness dip in center.
     *
     * @param pan Pan position [-1.0 = full left, 0.0 = center, +1.0 = full right]
     * @return Stereo gains for left and right channels
     */
    static StereoGains linearPan(float pan)
    {
        pan = juce::jlimit(-1.0f, 1.0f, pan);

        StereoGains gains;
        gains.leftGain = (1.0f - pan) * 0.5f;
        gains.rightGain = (1.0f + pan) * 0.5f;

        return gains;
    }

    /**
     * Balance control (attenuate one side, not the other).
     *
     * Unlike panning, balance reduces one channel without boosting the other.
     * Useful for adjusting existing stereo material.
     *
     * @param balance Balance position [-1.0 = full left, 0.0 = center, +1.0 = full right]
     * @return Stereo gains for left and right channels
     */
    static StereoGains balance(float balance)
    {
        balance = juce::jlimit(-1.0f, 1.0f, balance);

        StereoGains gains;

        if (balance < 0.0f)  // Reduce right channel
        {
            gains.leftGain = 1.0f;
            gains.rightGain = 1.0f + balance;
        }
        else  // Reduce left channel
        {
            gains.leftGain = 1.0f - balance;
            gains.rightGain = 1.0f;
        }

        return gains;
    }

    /**
     * Apply stereo width control using Mid/Side processing.
     *
     * Width = 0.0: Mono (no stereo)
     * Width = 1.0: Normal stereo
     * Width > 1.0: Enhanced stereo (up to 2.0)
     *
     * @param leftIn Left input sample
     * @param rightIn Right input sample
     * @param width Width control [0.0 = mono, 1.0 = normal, 2.0 = wide]
     * @param leftOut Reference to left output sample
     * @param rightOut Reference to right output sample
     */
    static void applyStereoWidth(float leftIn, float rightIn, float width,
                                   float& leftOut, float& rightOut)
    {
        width = juce::jlimit(0.0f, 2.0f, width);

        // Convert to Mid/Side
        float mid = (leftIn + rightIn) * 0.5f;
        float side = (leftIn - rightIn) * 0.5f;

        // Scale side signal by width
        side *= width;

        // Convert back to Left/Right
        leftOut = mid + side;
        rightOut = mid - side;
    }

    /**
     * Apply pan and width in one operation.
     *
     * More efficient than calling both separately.
     * Uses constant-power panning.
     *
     * @param leftIn Left input sample
     * @param rightIn Right input sample
     * @param pan Pan position [-1.0 to +1.0]
     * @param width Width control [0.0 to 2.0]
     * @param leftOut Reference to left output sample
     * @param rightOut Reference to right output sample
     */
    static void applyPanAndWidth(float leftIn, float rightIn, float pan, float width,
                                  float& leftOut, float& rightOut)
    {
        // Apply width first
        applyStereoWidth(leftIn, rightIn, width, leftOut, rightOut);

        // Then apply panning
        StereoGains panGains = constantPowerPan(pan);
        leftOut *= panGains.leftGain;
        rightOut *= panGains.rightGain;
    }

    /**
     * Calculate stereo correlation coefficient.
     *
     * Returns correlation between left and right channels:
     * +1.0 = identical (mono)
     * 0.0 = uncorrelated (normal stereo)
     * -1.0 = phase-inverted
     *
     * @param leftSamples Left channel samples
     * @param rightSamples Right channel samples
     * @param numSamples Number of samples
     * @return Correlation coefficient [-1.0 to +1.0]
     */
    static float calculateStereoCorrelation(const float* leftSamples,
                                             const float* rightSamples,
                                             int numSamples)
    {
        if (numSamples <= 0)
            return 0.0f;

        double sumL = 0.0, sumR = 0.0, sumLR = 0.0;
        double sumL2 = 0.0, sumR2 = 0.0;

        for (int i = 0; i < numSamples; ++i)
        {
            double L = leftSamples[i];
            double R = rightSamples[i];

            sumL += L;
            sumR += R;
            sumLR += L * R;
            sumL2 += L * L;
            sumR2 += R * R;
        }

        double n = static_cast<double>(numSamples);
        double numerator = n * sumLR - sumL * sumR;
        double denominator = std::sqrt((n * sumL2 - sumL * sumL) *
                                        (n * sumR2 - sumR * sumR));

        if (denominator < 1e-10)
            return 0.0f;

        return static_cast<float>(numerator / denominator);
    }

    /**
     * Haas effect (precedence delay) for stereo widening.
     *
     * Applies a small delay to one channel to create perceived width.
     * Typical delay: 5-30 ms.
     *
     * Note: Requires delay buffer management by caller.
     *
     * @param delayMs Delay time in milliseconds (5-30 ms typical)
     * @param sampleRate Sample rate in Hz
     * @return Delay in samples
     */
    static int calculateHaasDelaySamples(float delayMs, double sampleRate)
    {
        delayMs = juce::jlimit(0.0f, 50.0f, delayMs);
        return static_cast<int>(delayMs * 0.001 * sampleRate);
    }
};

} // namespace DSP
} // namespace AudioForge
