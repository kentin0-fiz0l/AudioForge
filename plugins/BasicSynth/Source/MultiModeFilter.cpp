#include "MultiModeFilter.h"
#include <juce_core/juce_core.h>
#include <cmath>
#include <algorithm>

MultiModeFilter::MultiModeFilter()
{
}

void MultiModeFilter::setParameters(float cutoffHz, float resonance, double sampleRate)
{
    // Only update coefficients if parameters changed
    if (cutoffHz != lastCutoff || resonance != lastResonance || sampleRate != lastSampleRate)
    {
        updateCoefficients(cutoffHz, resonance, sampleRate);
        lastCutoff = cutoffHz;
        lastResonance = resonance;
        lastSampleRate = sampleRate;
    }
}

void MultiModeFilter::setFilterType(FilterType type)
{
    filterType = type;
}

float MultiModeFilter::processSample(float input)
{
    // State-variable filter topology
    // Compute all outputs simultaneously using state equations

    // High-pass output: input - lp - Q * bp
    hp = input - lp - resonanceCoeff * bp;

    // Band-pass output: integrate high-pass
    bp = bp + cutoffCoeff * hp;

    // Low-pass output: integrate band-pass
    lp = lp + cutoffCoeff * bp;

    // Notch output: sum of high-pass and low-pass
    float notch = hp + lp;

    // Select output based on filter type
    float output = 0.0f;
    switch (filterType)
    {
        case FilterType::LowPass:
            output = lp;
            break;

        case FilterType::HighPass:
            output = hp;
            break;

        case FilterType::BandPass:
            output = bp;
            break;

        case FilterType::Notch:
            output = notch;
            break;
    }

    // Soft clipping to prevent runaway oscillation
    output = std::max(-2.0f, std::min(2.0f, output));

    return output;
}

void MultiModeFilter::reset()
{
    lp = 0.0f;
    bp = 0.0f;
    hp = 0.0f;
}

void MultiModeFilter::updateCoefficients(float cutoffHz, float resonance, double sampleRate)
{
    // Clamp cutoff frequency to valid range
    cutoffHz = std::max(20.0f, std::min(20000.0f, cutoffHz));

    // Clamp resonance to prevent instability
    resonance = std::max(0.5f, std::min(20.0f, resonance));

    // Calculate cutoff coefficient: 2 * sin(π * f / fs)
    // This maps frequency to the filter's internal frequency scale
    const float pi = juce::MathConstants<float>::pi;
    float normalizedFreq = cutoffHz / static_cast<float>(sampleRate);

    // Clamp normalized frequency to Nyquist limit
    normalizedFreq = std::min(normalizedFreq, 0.499f);

    cutoffCoeff = 2.0f * std::sin(pi * normalizedFreq);

    // Calculate resonance coefficient: 1 / Q
    // Higher Q (lower coefficient) = more resonance
    resonanceCoeff = 1.0f / resonance;
}
