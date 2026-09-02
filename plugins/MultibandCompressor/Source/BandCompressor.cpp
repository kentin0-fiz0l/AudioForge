/*
  ==============================================================================

    BandCompressor.cpp
    Created: 1 Sep 2026
    Author:  AudioForge Team

  ==============================================================================
*/

#include "BandCompressor.h"

namespace audioforge
{

BandCompressor::BandCompressor()
{
}

void BandCompressor::prepare(double sampleRate, int maxBlockSize)
{
    currentSampleRate = sampleRate;
    maxSamplesPerBlock = maxBlockSize;

    // Prepare envelope follower
    envelopeFollower.prepare(sampleRate, maxBlockSize);

    // Calculate envelope coefficients
    setAttack(attackTimeMs);
    setRelease(releaseTimeMs);

    reset();
}

void BandCompressor::setThreshold(float thresholdDb)
{
    threshold = juce::Decibels::decibelsToGain(thresholdDb);
}

void BandCompressor::setRatio(float newRatio)
{
    ratio = juce::jlimit(1.0f, 20.0f, newRatio);
}

void BandCompressor::setAttack(float attackMs)
{
    attackTimeMs = juce::jlimit(0.1f, 100.0f, attackMs);
    attackCoeff = timeConstantToCoeff(attackTimeMs);
}

void BandCompressor::setRelease(float releaseMs)
{
    releaseTimeMs = juce::jlimit(10.0f, 1000.0f, releaseMs);
    releaseCoeff = timeConstantToCoeff(releaseTimeMs);
}

void BandCompressor::setMakeupGain(float gainDb)
{
    makeupGain = juce::Decibels::decibelsToGain(gainDb);
}

void BandCompressor::setDetectionMode(EnvelopeFollower::DetectionMode mode)
{
    envelopeFollower.setDetectionMode(mode);
}

void BandCompressor::setRMSWindow(float windowMs)
{
    envelopeFollower.setRMSWindow(windowMs);
}

void BandCompressor::reset()
{
    envelopeFollower.reset();
    currentGainReduction = 1.0f;
}

void BandCompressor::process(float* samples, int numSamples)
{
    //==============================================================================
    // Per-band compression algorithm
    //
    // 1. Detect input level (RMS or Peak via EnvelopeFollower)
    // 2. Calculate target gain reduction based on threshold and ratio
    // 3. Smooth gain reduction with attack/release envelope
    // 4. Apply gain reduction + makeup gain to samples
    //==============================================================================

    for (int i = 0; i < numSamples; ++i)
    {
        //==============================================================================
        // 1. Detect input level

        float sample = samples[i];
        float inputLevel = envelopeFollower.process(&sample, 1);

        //==============================================================================
        // 2. Calculate target gain reduction

        float targetGain = calculateGainReduction(inputLevel);

        //==============================================================================
        // 3. Smooth gain reduction with attack/release

        float smoothedGain = smoothGainReduction(targetGain);

        //==============================================================================
        // 4. Apply gain reduction + makeup gain

        samples[i] = sample * smoothedGain * makeupGain;
    }
}

float BandCompressor::calculateGainReduction(float inputLevel)
{
    //==============================================================================
    // Standard compression formula
    //
    // If input > threshold:
    //   outputLevel = threshold + (inputLevel - threshold) / ratio
    //   gainReduction = outputLevel / inputLevel
    // Else:
    //   gainReduction = 1.0 (no compression)
    //==============================================================================

    if (inputLevel <= threshold)
    {
        // Below threshold: no compression
        return 1.0f;
    }

    // Above threshold: apply compression ratio
    // Convert to dB for calculation
    float inputDb = juce::Decibels::gainToDecibels(inputLevel);
    float thresholdDb = juce::Decibels::gainToDecibels(threshold);

    // How much is input over threshold?
    float excessDb = inputDb - thresholdDb;

    // Apply ratio: reduce excess by (ratio - 1) / ratio
    float reductionDb = excessDb * (ratio - 1.0f) / ratio;

    // Calculate output level
    float outputDb = inputDb - reductionDb;

    // Convert back to linear gain
    float outputLevel = juce::Decibels::decibelsToGain(outputDb);

    // Gain reduction = output / input
    return outputLevel / inputLevel;
}

float BandCompressor::smoothGainReduction(float targetGain)
{
    //==============================================================================
    // Exponential attack/release smoothing
    //
    // If target < current: reducing gain (attack)
    // If target > current: releasing compression (release)
    //==============================================================================

    if (targetGain < currentGainReduction)
    {
        // Attack (compression increasing)
        currentGainReduction += (targetGain - currentGainReduction) * (1.0f - attackCoeff);
    }
    else
    {
        // Release (compression decreasing)
        currentGainReduction += (targetGain - currentGainReduction) * (1.0f - releaseCoeff);
    }

    return currentGainReduction;
}

float BandCompressor::timeConstantToCoeff(float timeMs)
{
    // Convert time constant to exponential smoothing coefficient
    // Formula: coeff = exp(-1 / (timeMs * 0.001 * sampleRate))
    float timeConstant = timeMs * 0.001f * static_cast<float>(currentSampleRate);
    return std::exp(-1.0f / timeConstant);
}

float BandCompressor::getGainReductionDb() const
{
    // Convert linear gain reduction to dB
    // Note: gain reduction of 1.0 = 0 dB (no reduction)
    // gain reduction of 0.5 = -6 dB (6 dB reduction)
    float reductionLinear = 1.0f - currentGainReduction;
    return juce::Decibels::gainToDecibels(currentGainReduction);
}

} // namespace audioforge
