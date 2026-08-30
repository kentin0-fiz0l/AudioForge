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
    void setGrainSize(int samples);     // Grain size in samples
    void setWindowType(int type);       // 0=Hann, 1=Gaussian, 2=Triangle, 3=Tukey, 4=Blackman, 5=Kaiser
    void setWindowShape(float shape);   // Window shape parameter (0-1)

    int getGrainSize() const { return grainSize; }
    int getWindowType() const { return windowType; }
    float getWindowShape() const { return windowShape; }

    //==============================================================================
    // Grain Extraction
    void extractGrain(const GrainBuffer& buffer, int startPosition, float* destination);

private:
    //==============================================================================
    // Window Functions
    void createHannWindow();
    void createGaussianWindow();
    void createTriangleWindow();
    void createTukeyWindow();
    void createBlackmanWindow();
    void createKaiserWindow();

    //==============================================================================
    // Parameters
    double sampleRate = 44100.0;
    int grainSize = 2048;      // Default: ~46ms at 44.1kHz
    int windowType = 0;        // 0=Hann, 1=Gaussian, 2=Triangle, 3=Tukey, 4=Blackman, 5=Kaiser
    float windowShape = 0.5f;  // Window shape parameter (Gaussian: width, Tukey: taper)

    //==============================================================================
    // Window Buffer
    std::vector<float> window;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrainExtractor)
};
