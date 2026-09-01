#include "DrawbarEngine.h"
#include <cstring>
#include <algorithm>

DrawbarEngine::DrawbarEngine()
    : baseFrequency(440.0f)
{
    reset();

    // Initialize all drawbars to 0
    for (int i = 0; i < 9; ++i)
        drawbarLevels[i] = 0.0f;
}

void DrawbarEngine::setFrequency(float frequency)
{
    baseFrequency = frequency;
}

void DrawbarEngine::setDrawbars(const float drawbars[9])
{
    std::memcpy(drawbarLevels, drawbars, 9 * sizeof(float));
}

float DrawbarEngine::processSample(double sampleRate)
{
    float output = 0.0f;
    const float invSampleRate = 1.0f / static_cast<float>(sampleRate);

    // Sum all 9 harmonics
    for (int i = 0; i < 9; ++i)
    {
        if (drawbarLevels[i] > 0.0f)
        {
            // Generate sine wave at harmonic frequency
            float harmonicFreq = baseFrequency * HARMONIC_RATIOS[i];
            output += std::sin(phases[i] * 2.0f * M_PI) * drawbarLevels[i];

            // Advance phase
            phases[i] += harmonicFreq * invSampleRate;
            if (phases[i] >= 1.0f)
                phases[i] -= 1.0f;
        }
    }

    // Normalize by number of active drawbars to prevent clipping
    float normalization = 0.0f;
    for (int i = 0; i < 9; ++i)
        normalization += drawbarLevels[i];

    if (normalization > 0.0f)
        output /= std::max(1.0f, normalization);

    return output;
}

void DrawbarEngine::reset()
{
    for (int i = 0; i < 9; ++i)
        phases[i] = 0.0f;
}
