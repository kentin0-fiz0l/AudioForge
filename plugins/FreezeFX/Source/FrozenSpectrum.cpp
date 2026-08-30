#include "FrozenSpectrum.h"
#include <juce_core/juce_core.h>
#include <algorithm>
#include <cmath>

void FrozenSpectrum::freeze()
{
    frozen = true;
}

void FrozenSpectrum::unfreeze()
{
    frozen = false;
}

void FrozenSpectrum::captureSpectrum(const std::vector<float>& magnitude, const std::vector<float>& phase)
{
    frozenMagnitude = magnitude;
    frozenPhase = phase;
}

void FrozenSpectrum::getSpectrum(std::vector<float>& magnitude, std::vector<float>& phase)
{
    if (frozen && !frozenMagnitude.empty())
    {
        magnitude = frozenMagnitude;
        phase = frozenPhase;

        // Apply spectral blur if enabled
        if (blurAmount > 0.0f)
        {
            applyBlur(magnitude);
        }

        // Apply frequency range filtering (zero out bins outside range)
        for (size_t i = 0; i < magnitude.size(); ++i)
        {
            if (i < static_cast<size_t>(lowBin) || i > static_cast<size_t>(highBin))
            {
                magnitude[i] = 0.0f;  // Zero magnitude outside frequency range
                phase[i] = 0.0f;      // Zero phase as well
            }
        }
    }
}

void FrozenSpectrum::setBlurAmount(float amount)
{
    blurAmount = juce::jlimit(0.0f, 1.0f, amount);
}

void FrozenSpectrum::setFrequencyRange(float lowHz, float highHz, double sampleRate, int fftSize)
{
    // Convert Hz to FFT bin indices
    lowBin = static_cast<int>((lowHz / sampleRate) * fftSize);
    highBin = static_cast<int>((highHz / sampleRate) * fftSize);

    // Clamp to valid range
    int maxBin = fftSize / 2;
    lowBin = juce::jlimit(0, maxBin, lowBin);
    highBin = juce::jlimit(lowBin, maxBin, highBin);
}

void FrozenSpectrum::applyBlur(std::vector<float>& magnitude)
{
    if (blurAmount <= 0.0f || magnitude.size() < 3)
        return;

    // Simple box blur (average with neighbors)
    std::vector<float> blurred = magnitude;

    int blurRadius = static_cast<int>(blurAmount * 5.0f) + 1;  // 1-6 bins

    for (size_t i = 1; i < magnitude.size() - 1; ++i)
    {
        float sum = 0.0f;
        int count = 0;

        for (int offset = -blurRadius; offset <= blurRadius; ++offset)
        {
            int idx = static_cast<int>(i) + offset;
            if (idx >= 0 && idx < static_cast<int>(magnitude.size()))
            {
                sum += magnitude[idx];
                count++;
            }
        }

        blurred[i] = sum / count;
    }

    magnitude = blurred;
}
