#include "SnareModule.h"
#include <algorithm>

SnareModule::SnareModule()
{
}

void SnareModule::trigger()
{
    active = true;
    envLevel = 1.0f;
    bodyPhase = 0.0f;
    filterState1 = 0.0f;
    filterState2 = 0.0f;
}

void SnareModule::reset()
{
    active = false;
    envLevel = 0.0f;
    bodyPhase = 0.0f;
    filterState1 = 0.0f;
    filterState2 = 0.0f;
}

float SnareModule::processSample(double sampleRate, float tune, float snap,
                                 float tone, float decay, float mix)
{
    if (!active)
        return 0.0f;

    // Update envelope
    float deltaTime = static_cast<float>(1.0 / sampleRate);
    float decayRate = 15.0f / decay; // Faster decay than kick
    envLevel *= std::exp(-decayRate * deltaTime);

    // Stop if envelope is very quiet
    if (envLevel < 0.001f)
    {
        active = false;
        return 0.0f;
    }

    // Generate noise component (snare rattle)
    float noise = generateNoise();

    // Apply bandpass filter to noise (centered around 2-5kHz for snare character)
    float filterCutoff = 3500.0f + (tone * 3000.0f); // 3.5-6.5kHz range
    float filteredNoise = applyBandpassFilter(noise, filterCutoff, sampleRate);

    // Generate body tone (sine wave at low frequency)
    float bodyFreq = 150.0f + (tune * 100.0f); // 150-250 Hz range
    float bodyTone = std::sin(bodyPhase * 2.0f * M_PI);

    // Advance body phase
    float phaseIncrement = bodyFreq / static_cast<float>(sampleRate);
    bodyPhase += phaseIncrement;
    while (bodyPhase >= 1.0f)
        bodyPhase -= 1.0f;

    // Add snap (emphasize the attack)
    float snapEnv = (envLevel > 0.9f) ? (envLevel - 0.9f) * 10.0f : 0.0f;
    float snapAmount = snapEnv * snap * 2.0f;

    // Mix noise and body tone
    float noiseMix = mix;
    float bodyMix = 1.0f - mix;
    float output = (filteredNoise * noiseMix + bodyTone * bodyMix) * envLevel;
    output += filteredNoise * snapAmount;

    return output * 0.7f; // Scale down slightly to prevent clipping
}

float SnareModule::generateNoise()
{
    static unsigned int seed = 54321;
    seed = seed * 1103515245 + 12345;
    return ((float)(seed >> 16) / 32768.0f) - 1.0f;
}

float SnareModule::applyBandpassFilter(float input, float cutoff, double sampleRate)
{
    // Simple 2-pole bandpass filter
    float q = 2.0f; // Resonance
    float w0 = 2.0f * M_PI * cutoff / static_cast<float>(sampleRate);
    float alpha = std::sin(w0) / (2.0f * q);

    // Coefficients for bandpass
    float b0 = alpha;
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * std::cos(w0) / a0;
    float a2 = (1.0f - alpha) / a0;

    // Apply filter (direct form II)
    float output = b0 * input - a1 * filterState1 - a2 * filterState2;
    filterState2 = filterState1;
    filterState1 = output;

    return output;
}
