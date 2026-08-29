#include "Saturation.h"
#include <algorithm>

Saturation::Saturation()
{
}

float Saturation::processSample(float input)
{
    // Apply drive gain
    float driven = input * driveAmount;

    // Apply waveshaping based on type
    float saturated = 0.0f;
    switch (saturationType)
    {
        case SaturationType::SoftClip:
            saturated = softClip(driven);
            break;

        case SaturationType::HardClip:
            saturated = hardClip(driven);
            break;

        case SaturationType::Tube:
            saturated = tubeShape(driven);
            break;
    }

    // Compensate for gain increase (divide by drive)
    saturated /= driveAmount;

    // Mix dry and wet
    return input * (1.0f - dryWetMix) + saturated * dryWetMix;
}

void Saturation::setParameters(float drive, float mix, SaturationType type)
{
    // Map 0.0-1.0 to 1.0-10.0 drive range
    driveAmount = 1.0f + drive * 9.0f;
    dryWetMix = std::max(0.0f, std::min(1.0f, mix));
    saturationType = type;
}

void Saturation::reset()
{
    // No state to reset (stateless effect)
}

float Saturation::softClip(float x) const
{
    // tanh saturation: smooth, musical clipping
    return std::tanh(x);
}

float Saturation::hardClip(float x) const
{
    // Hard clipping: aggressive distortion
    return std::max(-1.0f, std::min(1.0f, x));
}

float Saturation::tubeShape(float x) const
{
    // Asymmetric tube-style saturation
    // Positive peaks compressed more than negative (tube characteristic)
    if (x > 0.0f)
    {
        // Positive: soft compression
        return x / (1.0f + x * x * 0.5f);
    }
    else
    {
        // Negative: less compression (asymmetry)
        return x / (1.0f + x * x * 0.3f);
    }
}
