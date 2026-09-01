#include "KickModule.h"
#include <algorithm>

KickModule::KickModule()
{
}

void KickModule::trigger()
{
    active = true;
    phase = 0.0f;
    envLevel = 1.0f;
    pitchEnvLevel = 1.0f;
}

void KickModule::reset()
{
    active = false;
    phase = 0.0f;
    envLevel = 0.0f;
    pitchEnvLevel = 0.0f;
}

float KickModule::processSample(double sampleRate, float pitch, float decay,
                                float click, float tone, float drive)
{
    if (!active)
        return 0.0f;

    // Update envelopes
    float deltaTime = static_cast<float>(1.0 / sampleRate);

    // Amplitude envelope (exponential decay)
    float decayRate = 10.0f / decay; // Map decay parameter to rate
    envLevel *= std::exp(-decayRate * deltaTime);

    // Pitch envelope (fast exponential decay from high to target pitch)
    float pitchEnvDecay = 20.0f; // Fixed fast decay for pitch sweep
    pitchEnvLevel *= std::exp(-pitchEnvDecay * deltaTime);

    // Stop if envelope is very quiet
    if (envLevel < 0.001f)
    {
        active = false;
        return 0.0f;
    }

    // Calculate current frequency with pitch envelope
    // Pitch sweep from 3x the target frequency down to target
    float pitchSweepAmount = 2.0f; // Sweep from 3x down to 1x
    float currentPitch = pitch * (1.0f + pitchSweepAmount * pitchEnvLevel);

    // Generate sine wave
    float sineWave = std::sin(phase * 2.0f * M_PI);

    // Advance phase
    float phaseIncrement = currentPitch / static_cast<float>(sampleRate);
    phase += phaseIncrement;
    while (phase >= 1.0f)
        phase -= 1.0f;

    // Add click transient (very short noise burst at the start)
    float clickSample = 0.0f;
    if (pitchEnvLevel > 0.8f) // Only during initial attack
    {
        clickSample = generateClickSample() * click * (pitchEnvLevel - 0.8f) * 5.0f;
    }

    // Mix tone (sine) and click
    float output = sineWave * tone + clickSample;

    // Apply drive (soft saturation)
    if (drive > 0.0f)
    {
        output *= (1.0f + drive * 2.0f);
        output = std::tanh(output);
    }

    // Apply amplitude envelope
    output *= envLevel;

    return output;
}

float KickModule::generateClickSample()
{
    // Simple noise generator
    static unsigned int seed = 1;
    seed = seed * 1103515245 + 12345;
    return ((float)(seed >> 16) / 32768.0f) - 1.0f;
}
