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
    // Zero-delay-feedback state-variable filter (trapezoidal integrators).
    // Both integrators are solved together for this sample, which keeps the
    // filter stable at any cutoff up to Nyquist. The previous Chamberlin form
    // updated them one after the other and blew up above roughly fs/6.
    const float v3 = input - ic2eq;
    const float bp = a1 * ic1eq + a2 * v3;
    const float lp = ic2eq + a2 * ic1eq + a3 * v3;

    // Update the integrator states
    ic1eq = 2.0f * bp - ic1eq;
    ic2eq = 2.0f * lp - ic2eq;

    // Select output based on filter type
    float output = 0.0f;
    switch (filterType)
    {
        case FilterType::LowPass:
            output = lp;
            break;

        case FilterType::HighPass:
            output = input - resonanceCoeff * bp - lp;
            break;

        case FilterType::BandPass:
            // Not normalised: gain at the cutoff is Q, as before
            output = bp;
            break;

        case FilterType::Notch:
            // High-pass plus low-pass
            output = input - resonanceCoeff * bp;
            break;
    }

    return output;
}

void MultiModeFilter::reset()
{
    ic1eq = 0.0f;
    ic2eq = 0.0f;
}

void MultiModeFilter::updateCoefficients(float cutoffHz, float resonance, double sampleRate)
{
    // Clamp cutoff frequency to valid range
    cutoffHz = std::max(20.0f, std::min(20000.0f, cutoffHz));

    // Clamp resonance to the supported range
    resonance = std::max(0.5f, std::min(20.0f, resonance));

    // Calculate cutoff coefficient: tan(π * f / fs)
    // The tangent pre-warps the frequency so the cutoff lands where requested
    const float pi = juce::MathConstants<float>::pi;
    float normalizedFreq = cutoffHz / static_cast<float>(sampleRate);

    // Keep the cutoff below Nyquist, where the tangent goes to infinity
    normalizedFreq = std::min(normalizedFreq, 0.49f);

    cutoffCoeff = std::tan(pi * normalizedFreq);

    // Calculate resonance coefficient: 1 / Q
    // Higher Q (lower coefficient) = more resonance
    resonanceCoeff = 1.0f / resonance;

    // Terms of the solved feedback loop, used once per sample
    a1 = 1.0f / (1.0f + cutoffCoeff * (cutoffCoeff + resonanceCoeff));
    a2 = cutoffCoeff * a1;
    a3 = cutoffCoeff * a2;
}
