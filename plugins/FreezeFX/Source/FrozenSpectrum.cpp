#include "FrozenSpectrum.h"
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

void FrozenSpectrum::getSpectrum(std::vector<float>& magnitude, std::vector<float>& phase) const
{
    if (frozen && !frozenMagnitude.empty())
    {
        magnitude = frozenMagnitude;
        phase = frozenPhase;
    }
}

void FrozenSpectrum::setBlurAmount(float amount)
{
    blurAmount = std::clamp(amount, 0.0f, 1.0f);
}

void FrozenSpectrum::setFrequencyRange(float lowHz, float highHz, double sampleRate, int fftSize)
{
    // Convert Hz to FFT bin indices
    lowBin = static_cast<int>((lowHz / sampleRate) * fftSize);
    highBin = static_cast<int>((highHz / sampleRate) * fftSize);

    // Clamp to valid range
    int maxBin = fftSize / 2;
    lowBin = std::clamp(lowBin, 0, maxBin);
    highBin = std::clamp(highBin, lowBin, maxBin);
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
