/*
  ==============================================================================

    LimiterEngine.cpp
    Created: 1 Sep 2026
    Author:  AudioForge Team

  ==============================================================================
*/

#include "LimiterEngine.h"

namespace audioforge
{

LimiterEngine::LimiterEngine()
{
}

void LimiterEngine::prepare(double sampleRate, int maxBlockSize)
{
    currentSampleRate = sampleRate;
    maxSamplesPerBlock = maxBlockSize;

    // Prepare true peak detector
    truePeakDetector.prepare(sampleRate, maxBlockSize);

    // Allocate lookahead buffer (stereo, 10ms max)
    int maxLookaheadSamples = static_cast<int>(sampleRate * 0.010); // 10ms
    lookaheadBuffer.setSize(2, maxLookaheadSamples + maxBlockSize, false, true, false);
    lookaheadBuffer.clear();

    // Calculate release coefficient
    setRelease(releaseTimeMs);

    reset();
}

void LimiterEngine::setRelease(float releaseMs)
{
    releaseTimeMs = releaseMs;

    // Exponential release coefficient
    // Formula: coeff = exp(-1 / (time_ms * sample_rate / 1000))
    float timeInSamples = (releaseMs / 1000.0f) * static_cast<float>(currentSampleRate);
    releaseCoeff = std::exp(-1.0f / timeInSamples);
}

void LimiterEngine::setLookahead(float lookaheadMs)
{
    // Convert milliseconds to samples
    lookaheadSamples = static_cast<int>((lookaheadMs / 1000.0f) * currentSampleRate);

    // Clamp to valid range
    int maxLookahead = static_cast<int>(currentSampleRate * 0.010); // 10ms max
    lookaheadSamples = juce::jlimit(0, maxLookahead, lookaheadSamples);

    // Reset read/write positions
    lookaheadWritePosition = 0;
    lookaheadReadPosition = 0;
}

void LimiterEngine::process(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    if (numSamples == 0 || numChannels == 0)
        return;

    // Get channel pointers
    auto* leftChannel = buffer.getWritePointer(0);
    auto* rightChannel = numChannels > 1 ? buffer.getWritePointer(1) : leftChannel;

    // Process each sample
    for (int i = 0; i < numSamples; ++i)
    {
        // Read input samples
        float leftIn = leftChannel[i];
        float rightIn = rightChannel[i];

        // Measure input peak
        float inputLevel = std::max(std::abs(leftIn), std::abs(rightIn));
        inputPeak = std::max(inputPeak * 0.999f, inputLevel);

        // === LOOKAHEAD BUFFER ===
        // Write current sample to lookahead buffer
        if (lookaheadSamples > 0)
        {
            lookaheadBuffer.setSample(0, lookaheadWritePosition, leftIn);
            lookaheadBuffer.setSample(1, lookaheadWritePosition, rightIn);

            // Read delayed sample from buffer
            leftIn = lookaheadBuffer.getSample(0, lookaheadReadPosition);
            rightIn = lookaheadBuffer.getSample(1, lookaheadReadPosition);

            // Advance circular buffer pointers
            lookaheadWritePosition = (lookaheadWritePosition + 1) % lookaheadBuffer.getNumSamples();
            lookaheadReadPosition = (lookaheadReadPosition + 1) % lookaheadBuffer.getNumSamples();
        }

        // === GAIN REDUCTION CALCULATION ===
        // Calculate target gain reduction based on input level
        targetGainReduction = calculateGainReduction(inputLevel);

        // Smooth gain reduction with exponential release
        currentGainReduction = smoothGainReduction(targetGainReduction, currentGainReduction);

        // === APPLY GAIN REDUCTION ===
        float leftOut = leftIn * currentGainReduction;
        float rightOut = rightIn * currentGainReduction;

        // === AUTO MAKEUP GAIN ===
        if (autoMakeupEnabled)
        {
            float makeupGain = calculateMakeupGain();
            leftOut *= makeupGain;
            rightOut *= makeupGain;
        }

        // === OUTPUT TRIM ===
        leftOut *= outputTrim;
        rightOut *= outputTrim;

        // Measure output peak
        float outputLevel = std::max(std::abs(leftOut), std::abs(rightOut));
        outputPeak = std::max(outputPeak * 0.999f, outputLevel);

        // Write output samples
        leftChannel[i] = leftOut;
        rightChannel[i] = rightOut;
    }

    // === TRUE PEAK DETECTION ===
    // Detect true peak across the entire buffer (expensive, so do once per buffer)
    truePeak = truePeakDetector.detectTruePeakStereo(
        buffer.getReadPointer(0),
        numChannels > 1 ? buffer.getReadPointer(1) : buffer.getReadPointer(0),
        numSamples
    );
}

float LimiterEngine::calculateGainReduction(float inputLevel)
{
    // No reduction below threshold
    if (inputLevel <= threshold)
        return 1.0f;

    // Calculate how much the input exceeds the threshold
    float overThreshold = inputLevel - threshold;

    // Brickwall limiting (infinite ratio)
    // We want: output = threshold + (overshoot compressed to zero)
    // So: output = ceiling when input > threshold
    // Required gain = ceiling / inputLevel

    float requiredGain = ceiling / inputLevel;

    // Clamp gain reduction between 0 and 1
    return juce::jlimit(0.0f, 1.0f, requiredGain);
}

float LimiterEngine::smoothGainReduction(float target, float current)
{
    // Attack: instant (follow target immediately when reducing more)
    if (target < current)
        return target;

    // Release: exponential envelope (smooth recovery)
    // current = current + (target - current) * (1 - releaseCoeff)
    return current + (target - current) * (1.0f - releaseCoeff);
}

float LimiterEngine::calculateMakeupGain()
{
    // Auto makeup gain compensates for the gain reduction
    // Formula: makeupGain = 1 / threshold (approximately)
    // This brings the average level back up to near ceiling

    if (threshold > 0.0f)
        return juce::jlimit(1.0f, 4.0f, ceiling / threshold);

    return 1.0f;
}

void LimiterEngine::reset()
{
    truePeakDetector.reset();
    lookaheadBuffer.clear();
    lookaheadWritePosition = 0;
    lookaheadReadPosition = lookaheadSamples; // Read lags behind write by lookahead amount

    currentGainReduction = 1.0f;
    targetGainReduction = 1.0f;

    inputPeak = 0.0f;
    outputPeak = 0.0f;
    truePeak = 0.0f;
}

} // namespace audioforge
