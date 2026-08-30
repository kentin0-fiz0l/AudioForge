#pragma once

#include <vector>

/**
 * FrozenSpectrum
 *
 * Manages frozen spectral states - captures and stores magnitude/phase spectrum.
 * Implements spectral blurring and frequency range selection.
 */
class FrozenSpectrum
{
public:
    FrozenSpectrum() = default;
    ~FrozenSpectrum() = default;

    //==============================================================================
    // Freeze Control
    void freeze();
    void unfreeze();
    bool isFrozen() const { return frozen; }

    //==============================================================================
    // Spectrum Capture/Retrieval
    void captureSpectrum(const std::vector<float>& magnitude, const std::vector<float>& phase);
    void getSpectrum(std::vector<float>& magnitude, std::vector<float>& phase) const;

    //==============================================================================
    // Processing Parameters
    void setBlurAmount(float amount);           // 0-1: smooth magnitude spectrum
    void setFrequencyRange(float lowHz, float highHz, double sampleRate, int fftSize);

    float getBlurAmount() const { return blurAmount; }

private:
    //==============================================================================
    // Frozen State
    std::vector<float> frozenMagnitude;
    std::vector<float> frozenPhase;
    bool frozen = false;

    //==============================================================================
    // Processing Parameters
    float blurAmount = 0.0f;
    int lowBin = 0;
    int highBin = 1024;

    //==============================================================================
    // Spectral Blurring
    void applyBlur(std::vector<float>& magnitude);
};
