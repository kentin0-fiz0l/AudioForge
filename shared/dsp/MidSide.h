/*
  ==============================================================================

    MidSide.h
    Created: 1 Sep 2026
    Author:  AudioForge Team

    Mid/Side (M/S) encoding and decoding for stereo processing.

    Theory:
    - Mid = (L + R) / 2    (center/mono content)
    - Side = (L - R) / 2   (stereo/width content)

    Decode:
    - L = Mid + Side
    - R = Mid - Side

    Applications:
    - Compress center without affecting width
    - Enhance stereo field independently
    - Mastering-grade stereo control

    Used by: Multiband Compressor, Stereo Imaging, future tools

  ==============================================================================
*/

#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>

namespace audioforge
{

/**
 * @brief Mid/Side stereo processor
 *
 * Converts stereo L/R signals to Mid/Side representation and back.
 *
 * Use cases:
 * - Process center (mid) and stereo (side) independently
 * - Compress vocals (mid) without affecting stereo reverb (side)
 * - Enhance stereo width without pumping center
 * - Professional mastering stereo control
 */
class MidSideProcessor
{
public:
    MidSideProcessor() = default;
    ~MidSideProcessor() = default;

    //==============================================================================
    // Encoding (L/R → Mid/Side)

    /**
     * @brief Encode stereo L/R to Mid/Side
     *
     * Converts:
     * - Mid = (L + R) / 2    (mono sum)
     * - Side = (L - R) / 2   (stereo difference)
     *
     * @param left Left channel (input), becomes Mid (output)
     * @param right Right channel (input), becomes Side (output)
     * @param numSamples Number of samples to process
     */
    static void encode(float* left, float* right, int numSamples);

    /**
     * @brief Encode stereo L/R to separate Mid/Side buffers
     *
     * @param left Left channel input
     * @param right Right channel input
     * @param mid Mid output buffer
     * @param side Side output buffer
     * @param numSamples Number of samples to process
     */
    static void encode(const float* left, const float* right,
                      float* mid, float* side, int numSamples);

    //==============================================================================
    // Decoding (Mid/Side → L/R)

    /**
     * @brief Decode Mid/Side back to stereo L/R
     *
     * Converts:
     * - L = Mid + Side
     * - R = Mid - Side
     *
     * @param mid Mid channel (input), becomes Left (output)
     * @param side Side channel (input), becomes Right (output)
     * @param numSamples Number of samples to process
     */
    static void decode(float* mid, float* side, int numSamples);

    /**
     * @brief Decode separate Mid/Side buffers to stereo L/R
     *
     * @param mid Mid input buffer
     * @param side Side input buffer
     * @param left Left output buffer
     * @param right Right output buffer
     * @param numSamples Number of samples to process
     */
    static void decode(const float* mid, const float* side,
                      float* left, float* right, int numSamples);

    //==============================================================================
    // Utility

    /**
     * @brief Test Mid/Side encoding/decoding (null test)
     *
     * Encodes L/R → M/S, then decodes M/S → L/R.
     * Result should equal original L/R (within floating-point precision).
     *
     * @param left Left channel (modified in-place)
     * @param right Right channel (modified in-place)
     * @param numSamples Number of samples to test
     * @return true if encode/decode is reversible (max error < 1e-6)
     */
    static bool nullTest(float* left, float* right, int numSamples);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidSideProcessor)
};

} // namespace audioforge
