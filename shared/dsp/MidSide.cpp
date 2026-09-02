/*
  ==============================================================================

    MidSide.cpp
    Created: 1 Sep 2026
    Author:  AudioForge Team

  ==============================================================================
*/

#include "MidSide.h"

namespace audioforge
{

void MidSideProcessor::encode(float* left, float* right, int numSamples)
{
    //==============================================================================
    // Mid/Side Encoding (in-place)
    //
    // Mid = (L + R) / 2    (mono sum - center content)
    // Side = (L - R) / 2   (stereo difference - width content)
    //
    // After encoding:
    // - left[] contains Mid
    // - right[] contains Side
    //==============================================================================

    for (int i = 0; i < numSamples; ++i)
    {
        float L = left[i];
        float R = right[i];

        left[i] = (L + R) * 0.5f;   // Mid
        right[i] = (L - R) * 0.5f;  // Side
    }
}

void MidSideProcessor::encode(const float* left, const float* right,
                               float* mid, float* side, int numSamples)
{
    //==============================================================================
    // Mid/Side Encoding (separate buffers)
    //==============================================================================

    for (int i = 0; i < numSamples; ++i)
    {
        float L = left[i];
        float R = right[i];

        mid[i] = (L + R) * 0.5f;    // Mid
        side[i] = (L - R) * 0.5f;   // Side
    }
}

void MidSideProcessor::decode(float* mid, float* side, int numSamples)
{
    //==============================================================================
    // Mid/Side Decoding (in-place)
    //
    // L = Mid + Side
    // R = Mid - Side
    //
    // After decoding:
    // - mid[] contains Left
    // - side[] contains Right
    //==============================================================================

    for (int i = 0; i < numSamples; ++i)
    {
        float M = mid[i];
        float S = side[i];

        mid[i] = M + S;   // Left
        side[i] = M - S;  // Right
    }
}

void MidSideProcessor::decode(const float* mid, const float* side,
                               float* left, float* right, int numSamples)
{
    //==============================================================================
    // Mid/Side Decoding (separate buffers)
    //==============================================================================

    for (int i = 0; i < numSamples; ++i)
    {
        float M = mid[i];
        float S = side[i];

        left[i] = M + S;   // Left
        right[i] = M - S;  // Right
    }
}

bool MidSideProcessor::nullTest(float* left, float* right, int numSamples)
{
    //==============================================================================
    // Null Test: Encode → Decode should equal original
    //
    // This verifies that Mid/Side processing is reversible.
    //==============================================================================

    // Store original values
    juce::AudioBuffer<float> original(2, numSamples);
    juce::FloatVectorOperations::copy(original.getWritePointer(0), left, numSamples);
    juce::FloatVectorOperations::copy(original.getWritePointer(1), right, numSamples);

    // Encode
    encode(left, right, numSamples);

    // Decode
    decode(left, right, numSamples);

    // Compare with original
    float maxError = 0.0f;
    for (int i = 0; i < numSamples; ++i)
    {
        float leftError = std::abs(left[i] - original.getSample(0, i));
        float rightError = std::abs(right[i] - original.getSample(1, i));

        maxError = juce::jmax(maxError, leftError, rightError);
    }

    // Allow tiny floating-point error (< 1e-6)
    return maxError < 1e-6f;
}

} // namespace audioforge
