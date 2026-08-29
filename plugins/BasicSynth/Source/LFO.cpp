#include "LFO.h"
#include <cmath>

LFO::LFO()
{
}

void LFO::setRate(float rateHz)
{
    rate = std::max(0.01f, std::min(20.0f, rateHz));
}

void LFO::setTempoSync(bool sync, float bpmValue, float divisionValue)
{
    tempoSynced = sync;
    bpm = bpmValue;
    division = divisionValue;
}

void LFO::setShape(Shape newShape)
{
    shape = newShape;
}

float LFO::getNextSample(double sampleRate)
{
    // Generate sample for current phase
    float sample = generateSample();

    // Advance phase
    float phaseIncrement = calculatePhaseIncrement(sampleRate);
    phase += phaseIncrement;

    // Wrap phase to [0.0, 1.0]
    while (phase >= 1.0f)
        phase -= 1.0f;

    return sample;
}

void LFO::reset()
{
    phase = 0.0f;
}

float LFO::calculatePhaseIncrement(double sampleRate) const
{
    float frequency = rate;

    if (tempoSynced)
    {
        // Calculate frequency from BPM and division
        // BPM = beats per minute
        // division = 1.0 (quarter note), 0.5 (eighth note), 2.0 (half note), etc.
        // frequency = (BPM / 60) * division
        frequency = (bpm / 60.0f) * division;
    }

    // Phase increment per sample = frequency / sampleRate
    return frequency / static_cast<float>(sampleRate);
}

float LFO::generateSample() const
{
    switch (shape)
    {
        case Shape::Sine:
        {
            // Sine wave: sin(2π * phase)
            return std::sin(2.0f * juce::MathConstants<float>::pi * phase);
        }

        case Shape::Triangle:
        {
            // Triangle wave: rises 0→1 (0-0.5), falls 1→0 (0.5-1.0)
            if (phase < 0.5f)
                return (phase * 4.0f) - 1.0f;  // Rise from -1 to +1
            else
                return 3.0f - (phase * 4.0f);   // Fall from +1 to -1
        }

        case Shape::Sawtooth:
        {
            // Sawtooth wave: linear rise from -1 to +1
            return (phase * 2.0f) - 1.0f;
        }

        case Shape::Square:
        {
            // Square wave: -1 for first half, +1 for second half
            return (phase < 0.5f) ? -1.0f : 1.0f;
        }

        case Shape::SampleAndHold:
        {
            // Sample & Hold: random value that changes each cycle
            // Note: This is a simplified implementation
            // For true S&H, we'd store the value and only change on phase wrap
            if (phase < 0.001f)  // Near phase reset
                const_cast<LFO*>(this)->random.setSeed(static_cast<int64>(phase * 1000000));

            return const_cast<LFO*>(this)->random.nextFloat() * 2.0f - 1.0f;
        }

        default:
            return 0.0f;
    }
}
