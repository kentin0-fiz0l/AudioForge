#include "ClapModule.h"

namespace
{
    // Each burst is gone in a few milliseconds, well before the next
    constexpr float burstDecay = 0.0025f;

    // The tail has fallen by 60 dB when this many decay times have passed
    constexpr float decaysTo60dB = 6.9f;

    // Scaled to sit with the snare
    constexpr float outputLevel = 0.9f;
}

void ClapModule::trigger()
{
    reset();
    active = true;
}

void ClapModule::reset()
{
    active = false;
    samplesSinceTrigger = 0;
    lowState = 0.0f;
    bandState = 0.0f;
}

float ClapModule::processSample(double sampleRate, float tone, float decay)
{
    if (!active)
        return 0.0f;

    const float elapsed = static_cast<float>(samplesSinceTrigger / sampleRate);
    const float tailStart = burstSpacing * static_cast<float>(numBursts);

    float envLevel;

    if (elapsed < tailStart)
    {
        // Start again at every burst
        const float sinceBurst = std::fmod(elapsed, burstSpacing);
        envLevel = std::exp(-sinceBurst / burstDecay);
    }
    else
    {
        envLevel = std::exp(-decaysTo60dB * (elapsed - tailStart) / decay);

        if (envLevel < 0.001f)
        {
            active = false;
            return 0.0f;
        }
    }

    ++samplesSinceTrigger;

    const float centre = 700.0f + tone * 1800.0f; // 700 Hz to 2.5 kHz
    const float filteredNoise = applyBandpassFilter(generateNoise(), centre, sampleRate);

    return filteredNoise * envLevel * outputLevel;
}

float ClapModule::generateNoise()
{
    noiseSeed = noiseSeed * 1103515245 + 12345;
    return ((float)(noiseSeed >> 16) / 32768.0f) - 1.0f;
}

float ClapModule::applyBandpassFilter(float input, float centre, double sampleRate)
{
    // State variable filter. This much damping leaves the band wide enough
    // to sound like skin and narrow enough not to sound like a snare.
    const float pi = 3.14159265358979f; // M_PI is not defined by every compiler
    const float f = 2.0f * std::sin(pi * centre / static_cast<float>(sampleRate));
    const float damping = 0.7f;

    lowState += f * bandState;
    const float high = input - lowState - damping * bandState;
    bandState += f * high;

    return bandState;
}
