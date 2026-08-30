#include "GrainExtractor.h"
#include <juce_core/juce_core.h>

GrainExtractor::GrainExtractor()
{
}

void GrainExtractor::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;
    createHannWindow();
}

void GrainExtractor::setGrainSize(int samples)
{
    grainSize = juce::jlimit(64, 22050, samples);  // Min 64 samples, max 500ms @ 44.1kHz

    // Recreate window when grain size changes
    if (windowType == 0)
        createHannWindow();
    else if (windowType == 1)
        createGaussianWindow();
    else if (windowType == 2)
        createTriangleWindow();
    else if (windowType == 3)
        createTukeyWindow();
}

void GrainExtractor::setWindowType(int type)
{
    windowType = juce::jlimit(0, 3, type);

    // Recreate window
    if (windowType == 0)
        createHannWindow();
    else if (windowType == 1)
        createGaussianWindow();
    else if (windowType == 2)
        createTriangleWindow();
    else if (windowType == 3)
        createTukeyWindow();
}

void GrainExtractor::setWindowShape(float shape)
{
    windowShape = juce::jlimit(0.0f, 1.0f, shape);

    // Recreate window if using Gaussian or Tukey (shape-dependent windows)
    if (windowType == 1)
        createGaussianWindow();
    else if (windowType == 3)
        createTukeyWindow();
}

void GrainExtractor::extractGrain(const GrainBuffer& buffer, int startPosition, float* destination)
{
    // Read grain from buffer and apply window
    for (int i = 0; i < grainSize; ++i)
    {
        float sample = buffer.readSample(startPosition + i);
        destination[i] = sample * window[i];
    }
}

//==============================================================================
// Window Functions

void GrainExtractor::createHannWindow()
{
    window.resize(grainSize);

    // Hann window: 0.5 * (1 - cos(2*pi*n / (N-1)))
    for (int i = 0; i < grainSize; ++i)
    {
        float phase = (float)i / (grainSize - 1);
        window[i] = 0.5f * (1.0f - std::cos(2.0f * juce::MathConstants<float>::pi * phase));
    }
}

void GrainExtractor::createGaussianWindow()
{
    window.resize(grainSize);

    // Gaussian window: exp(-0.5 * ((n - center) / sigma)^2)
    float center = (grainSize - 1) / 2.0f;

    // windowShape controls width: 0.1-1.0 (0 = narrow/peaky, 1 = wide/smooth)
    float widthFactor = 0.1f + windowShape * 0.9f;
    float sigma = grainSize / (6.0f / widthFactor);

    for (int i = 0; i < grainSize; ++i)
    {
        float distance = (i - center) / sigma;
        window[i] = std::exp(-0.5f * distance * distance);
    }
}

void GrainExtractor::createTriangleWindow()
{
    window.resize(grainSize);

    // Triangle window: linear ramp up, linear ramp down
    int halfSize = grainSize / 2;

    for (int i = 0; i < grainSize; ++i)
    {
        if (i < halfSize)
        {
            // Ramp up
            window[i] = (float)i / halfSize;
        }
        else
        {
            // Ramp down
            window[i] = 1.0f - ((float)(i - halfSize) / halfSize);
        }
    }
}

void GrainExtractor::createTukeyWindow()
{
    window.resize(grainSize);

    // Tukey window (tapered cosine): combines flat top with cosine taper
    // windowShape controls taper amount: 0 = rectangular, 1 = Hann
    float alpha = windowShape;  // Taper fraction (0-1)

    for (int i = 0; i < grainSize; ++i)
    {
        float x = (float)i / (grainSize - 1);

        if (x < alpha / 2.0f)
        {
            // Leading taper (cosine rise)
            float phase = 2.0f * x / alpha;
            window[i] = 0.5f * (1.0f - std::cos(juce::MathConstants<float>::pi * phase));
        }
        else if (x > 1.0f - alpha / 2.0f)
        {
            // Trailing taper (cosine fall)
            float phase = 2.0f * (1.0f - x) / alpha;
            window[i] = 0.5f * (1.0f - std::cos(juce::MathConstants<float>::pi * phase));
        }
        else
        {
            // Flat top
            window[i] = 1.0f;
        }
    }
}
