#include "UnisonVoice.h"
#include <cmath>

UnisonVoice::UnisonVoice()
{
}

void UnisonVoice::setFrequency(float baseFrequency, float detuneAmount, float panValue)
{
    // Convert detune from cents to frequency multiplier
    // cents = 1200 * log2(f2/f1)
    // f2 = f1 * 2^(cents/1200)
    float detuneMultiplier = std::pow(2.0f, detuneAmount / 1200.0f);
    frequency = baseFrequency * detuneMultiplier;
    pan = panValue;
}

void UnisonVoice::setWaveform(int waveformType)
{
    waveform = waveformType;
}

void UnisonVoice::reset()
{
    phase = 0.0f;
}

float UnisonVoice::processSample(double sampleRate)
{
    float sample = generateSample();

    // Advance phase
    float phaseIncrement = frequency / static_cast<float>(sampleRate);
    phase += phaseIncrement;

    // Wrap phase to [0, 1]
    while (phase >= 1.0f)
        phase -= 1.0f;

    return sample;
}

float UnisonVoice::generateSample()
{
    switch (waveform)
    {
        case 0: // Sine
            return std::sin(phase * 2.0f * M_PI);

        case 1: // Saw
            return 2.0f * phase - 1.0f;

        case 2: // Square
            return (phase < 0.5f) ? 1.0f : -1.0f;

        case 3: // Triangle
            return (phase < 0.5f) ? (4.0f * phase - 1.0f) : (3.0f - 4.0f * phase);

        default:
            return 0.0f;
    }
}
