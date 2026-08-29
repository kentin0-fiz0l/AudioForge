#include "Chorus.h"
#include <juce_core/juce_core.h>
#include <algorithm>

Chorus::Chorus()
{
}

void Chorus::prepare(double sampleRate, int maxBlockSize)
{
    currentSampleRate = sampleRate;

    // Allocate delay buffers (need to hold MAX_DELAY_MS worth of samples)
    delayBufferSize = static_cast<int>((MAX_DELAY_MS / 1000.0f) * sampleRate) + 1;
    delayBufferLeft.resize(delayBufferSize, 0.0f);
    delayBufferRight.resize(delayBufferSize, 0.0f);

    reset();
}

void Chorus::processSample(float input, float& leftOut, float& rightOut)
{
    // Write input to both delay buffers
    delayBufferLeft[writePosition] = input;
    delayBufferRight[writePosition] = input;

    // Generate LFO values for each channel
    float lfoLeft = generateLFO(lfoPhaseLeft);
    float lfoRight = generateLFO(lfoPhaseRight);

    // Calculate modulated delay times (in samples)
    float centerDelaySamples = (CENTER_DELAY_MS / 1000.0f) * static_cast<float>(currentSampleRate);
    float modulationRange = (MAX_DELAY_MS - MIN_DELAY_MS) / 2.0f / 1000.0f * static_cast<float>(currentSampleRate);

    float delayLeft = centerDelaySamples + lfoLeft * modulationRange * modulationDepth;
    float delayRight = centerDelaySamples + lfoRight * modulationRange * modulationDepth;

    // Read from delay buffers with interpolation
    float delayedLeft = readDelayBuffer(delayBufferLeft, delayLeft);
    float delayedRight = readDelayBuffer(delayBufferRight, delayRight);

    // Mix dry and wet signals
    leftOut = input * (1.0f - dryWetMix) + delayedLeft * dryWetMix;
    rightOut = input * (1.0f - dryWetMix) + delayedRight * dryWetMix;

    // Advance write position
    writePosition = (writePosition + 1) % delayBufferSize;

    // Advance LFO phases
    float lfoIncrement = lfoRate / static_cast<float>(currentSampleRate);
    lfoPhaseLeft += lfoIncrement;
    lfoPhaseRight += lfoIncrement;

    // Wrap phases
    if (lfoPhaseLeft >= 1.0f) lfoPhaseLeft -= 1.0f;
    if (lfoPhaseRight >= 1.0f) lfoPhaseRight -= 1.0f;
}

void Chorus::setParameters(float rate, float depth, float mix)
{
    lfoRate = std::max(0.1f, std::min(10.0f, rate));
    modulationDepth = std::max(0.0f, std::min(1.0f, depth));
    dryWetMix = std::max(0.0f, std::min(1.0f, mix));
}

void Chorus::reset()
{
    std::fill(delayBufferLeft.begin(), delayBufferLeft.end(), 0.0f);
    std::fill(delayBufferRight.begin(), delayBufferRight.end(), 0.0f);
    writePosition = 0;
    lfoPhaseLeft = 0.0f;
    lfoPhaseRight = 0.25f;
}

float Chorus::readDelayBuffer(const std::vector<float>& buffer, float delaySamples)
{
    // Calculate read position (wrap around using modulo)
    float readPositionFloat = static_cast<float>(writePosition) - delaySamples;

    // Handle negative wrap-around
    while (readPositionFloat < 0.0f)
        readPositionFloat += static_cast<float>(delayBufferSize);

    // Integer and fractional parts for interpolation
    int readPos1 = static_cast<int>(readPositionFloat) % delayBufferSize;
    int readPos2 = (readPos1 + 1) % delayBufferSize;
    float fraction = readPositionFloat - std::floor(readPositionFloat);

    // Linear interpolation
    float sample1 = buffer[readPos1];
    float sample2 = buffer[readPos2];

    return sample1 + fraction * (sample2 - sample1);
}

float Chorus::generateLFO(float phase) const
{
    // Sine wave LFO
    const float pi = juce::MathConstants<float>::pi;
    return std::sin(2.0f * pi * phase);
}
