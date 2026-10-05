#pragma once

#include <vector>

/**
 * FrozenSpectrum
 *
 * Manages frozen spectral states - captures and stores the magnitude spectrum.
 * The phase of a frozen sound keeps moving, so the processor looks after it.
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
    // Sizes the buffers, so that capturing does not allocate
    void prepare(size_t numBins);

    void captureSpectrum(const std::vector<float>& magnitude);

    // The frozen magnitudes with blur and frequency range applied
    void getSpectrum(std::vector<float>& magnitude);

    // The magnitudes as they were captured, before blur and range
    const std::vector<float>& getCapturedMagnitude() const { return frozenMagnitude; }

    //==============================================================================
    // Processing Parameters
    void setBlurAmount(float amount);           // 0-1: smooth magnitude spectrum
    void setFrequencyRange(float lowHz, float highHz, double sampleRate, int fftSize);

    float getBlurAmount() const { return blurAmount; }

private:
    //==============================================================================
    // Frozen State
    std::vector<float> frozenMagnitude;
    std::vector<float> blurScratch;
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
