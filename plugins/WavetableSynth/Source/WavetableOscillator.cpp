#include "WavetableOscillator.h"
#include <algorithm>

WavetableOscillator::WavetableOscillator()
{
    // Generate all wavetables on construction
    wavetables = Wavetables::WavetableGenerator::generateAllWavetables();
}

void WavetableOscillator::setFrequency(float frequencyHz)
{
    frequency = std::max(20.0f, std::min(20000.0f, frequencyHz));
}

void WavetableOscillator::setPosition(float pos)
{
    position = std::max(0.0f, std::min(1.0f, pos));
}

void WavetableOscillator::reset()
{
    phase = 0.0f;
    active = false;
}

float WavetableOscillator::processSample(double sampleRate)
{
    if (!active)
        return 0.0f;

    // Get interpolated sample
    float output = getInterpolatedSample();

    // Advance phase
    float phaseIncrement = frequency / static_cast<float>(sampleRate);
    phase += phaseIncrement;

    // Wrap phase to [0, 1]
    while (phase >= 1.0f)
        phase -= 1.0f;

    return output;
}

float WavetableOscillator::getInterpolatedSample()
{
    // Calculate which two wavetables to interpolate between
    float tablePosition = position * (wavetables.size() - 1);
    int tableIndex1 = static_cast<int>(tablePosition);
    int tableIndex2 = std::min(tableIndex1 + 1, static_cast<int>(wavetables.size()) - 1);
    float tableFrac = tablePosition - tableIndex1;

    // Calculate sample index within wavetable
    float samplePosition = phase * Wavetables::WAVETABLE_SIZE;
    int sampleIndex1 = static_cast<int>(samplePosition);
    int sampleIndex2 = (sampleIndex1 + 1) % Wavetables::WAVETABLE_SIZE;
    float sampleFrac = samplePosition - sampleIndex1;

    // Get samples from both wavetables with linear interpolation
    float sample1Table1 = wavetables[tableIndex1][sampleIndex1];
    float sample2Table1 = wavetables[tableIndex1][sampleIndex2];
    float interpolatedTable1 = lerp(sample1Table1, sample2Table1, sampleFrac);

    float sample1Table2 = wavetables[tableIndex2][sampleIndex1];
    float sample2Table2 = wavetables[tableIndex2][sampleIndex2];
    float interpolatedTable2 = lerp(sample1Table2, sample2Table2, sampleFrac);

    // Interpolate between the two wavetables
    return lerp(interpolatedTable1, interpolatedTable2, tableFrac);
}
