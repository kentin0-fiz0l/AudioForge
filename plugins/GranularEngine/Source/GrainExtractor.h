#pragma once

#include "GrainBuffer.h"
#include <vector>
#include <cmath>

/**
 * GrainExtractor
 *
 * Extracts grains from the GrainBuffer and applies windowing.
 * Responsible for grain envelope shaping.
 */
class GrainExtractor
{
public:
    GrainExtractor();
    ~GrainExtractor() = default;

    //==============================================================================
    // Configuration
    void prepare(double sampleRate);
    void setGrainSize(int samples);  // Grain size in samples
    void setWindowType(int type);    // 0=Hann, 1=Gaussian, 2=Triangle (Phase 4)

    int getGrainSize() const { return grainSize; }

    //==============================================================================
    // Grain Extraction
    void extractGrain(const GrainBuffer& buffer, int startPosition, float* destination);

private:
    //==============================================================================
    // Window Functions
    void createHannWindow();
    void createGaussianWindow();
    void createTriangleWindow();

    //==============================================================================
    // Parameters
    double sampleRate = 44100.0;
    int grainSize = 2048;  // Default: ~46ms at 44.1kHz
    int windowType = 0;    // 0=Hann

    //==============================================================================
    // Window Buffer
    std::vector<float> window;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrainExtractor)
};
