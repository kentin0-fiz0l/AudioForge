#pragma once

#include <juce_core/juce_core.h>

/**
 * Noise Generator for Enhanced BasicSynth
 *
 * Generates white and pink noise for synthesis.
 */
class NoiseGenerator
{
public:
    enum class NoiseType
    {
        White,  // Equal energy across all frequencies
        Pink    // -3dB per octave (more natural)
    };

    NoiseGenerator();

    /**
     * Get the next noise sample.
     *
     * @param type Type of noise to generate
     * @return Sample value in range [-1.0, 1.0]
     */
    float getNextSample(NoiseType type);

    /**
     * Reset the pink noise filter state.
     */
    void reset();

private:
    juce::Random random;

    // Pink noise state (Paul Kellett's refined method)
    float pinkState[7] = {0.0f};
    int pinkCounter = 0;

    /**
     * Generate white noise sample.
     */
    float generateWhite();

    /**
     * Generate pink noise sample.
     */
    float generatePink();
};
