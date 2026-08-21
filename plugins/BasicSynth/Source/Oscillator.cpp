#include "Oscillator.h"
#include <juce_core/juce_core.h>

Oscillator::Oscillator()
{
}

void Oscillator::setFrequency(float frequency, double sampleRate)
{
    // Calculate phase increment: frequency / sampleRate
    phaseIncrement = frequency / static_cast<float>(sampleRate);
}

float Oscillator::getNextSample(Waveform waveform)
{
    float sample = 0.0f;

    switch (waveform)
    {
        case Waveform::Sine:
            sample = generateSine();
            break;

        case Waveform::Sawtooth:
            sample = generateSawtooth();
            break;

        case Waveform::Square:
            sample = generateSquare();
            break;
    }

    // Advance phase and wrap to [0, 1]
    phase += phaseIncrement;
    while (phase >= 1.0f)
        phase -= 1.0f;

    return sample;
}

void Oscillator::reset()
{
    phase = 0.0f;
}

float Oscillator::generateSine()
{
    // Use JUCE's fast sine approximation
    return std::sin(phase * 2.0f * juce::MathConstants<float>::pi);
}

float Oscillator::generateSawtooth()
{
    // Sawtooth: ramp from -1 to +1
    // Naive sawtooth (will add polyBLEP anti-aliasing later if needed)
    return (phase * 2.0f) - 1.0f;
}

float Oscillator::generateSquare()
{
    // Square wave: -1 for first half, +1 for second half
    // Naive square (will add polyBLEP anti-aliasing later if needed)
    return (phase < 0.5f) ? -1.0f : 1.0f;
}
