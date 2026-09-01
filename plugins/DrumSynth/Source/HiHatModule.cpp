#include "HiHatModule.h"
#include <algorithm>

HiHatModule::HiHatModule()
{
}

void HiHatModule::trigger()
{
    active = true;
    envLevel = 1.0f;
    filterState1 = 0.0f;
    filterState2 = 0.0f;
}

void HiHatModule::reset()
{
    active = false;
    envLevel = 0.0f;
    filterState1 = 0.0f;
    filterState2 = 0.0f;
}

float HiHatModule::processSample(double sampleRate, float tune, float decay,
                                 float tone, float click)
{
    if (!active)
        return 0.0f;

    // Update envelope (very fast decay for hi-hat)
    float deltaTime = static_cast<float>(1.0 / sampleRate);
    float decayRate = 30.0f / decay; // Very fast decay
    envLevel *= std::exp(-decayRate * deltaTime);

    // Stop if envelope is very quiet
    if (envLevel < 0.001f)
    {
        active = false;
        return 0.0f;
    }

    // Generate noise
    float noise = generateNoise();

    // Apply highpass filter (hi-hats are bright)
    float filterCutoff = 6000.0f + (tune * 6000.0f); // 6-12kHz range
    float filteredNoise = applyHighpassFilter(noise, filterCutoff, sampleRate);

    // Apply additional tone shaping
    if (tone < 0.5f)
    {
        // Darker tone: apply some lowpass filtering
        float darkening = (0.5f - tone) * 2.0f;
        filteredNoise *= (1.0f - darkening * 0.5f);
    }

    // Add click (sharper attack)
    float clickEnv = (envLevel > 0.95f) ? (envLevel - 0.95f) * 20.0f : 0.0f;
    float output = filteredNoise * (envLevel + clickEnv * click);

    return output * 0.5f; // Scale down to prevent clipping
}

float HiHatModule::generateNoise()
{
    static unsigned int seed = 98765;
    seed = seed * 1103515245 + 12345;
    return ((float)(seed >> 16) / 32768.0f) - 1.0f;
}

float HiHatModule::applyHighpassFilter(float input, float cutoff, double sampleRate)
{
    // Simple 2-pole highpass filter
    float q = 0.707f; // Butterworth response
    float w0 = 2.0f * M_PI * cutoff / static_cast<float>(sampleRate);
    float alpha = std::sin(w0) / (2.0f * q);

    // Coefficients for highpass
    float b0 = (1.0f + std::cos(w0)) / 2.0f;
    float b1 = -(1.0f + std::cos(w0));
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * std::cos(w0) / a0;
    float a2 = (1.0f - alpha) / a0;

    // Apply filter (direct form II)
    float output = b0 * input + b1 * filterState1 - a1 * filterState1 - a2 * filterState2;
    filterState2 = filterState1;
    filterState1 = output;

    return output;
}
