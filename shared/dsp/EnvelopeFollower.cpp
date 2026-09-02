/*
  ==============================================================================

    EnvelopeFollower.cpp
    Created: 1 Sep 2026
    Author:  AudioForge Team

  ==============================================================================
*/

#include "EnvelopeFollower.h"

namespace audioforge
{

EnvelopeFollower::EnvelopeFollower()
{
}

void EnvelopeFollower::prepare(double sampleRate, int maxBlockSize)
{
    currentSampleRate = sampleRate;
    maxSamplesPerBlock = maxBlockSize;

    // Allocate RMS buffer
    setRMSWindow(rmsWindowMs);

    // Calculate peak decay coefficient
    setPeakHoldTime(peakHoldMs);

    reset();
}

void EnvelopeFollower::setRMSWindow(float windowMs)
{
    rmsWindowMs = juce::jlimit(1.0f, 50.0f, windowMs);
    rmsWindowSamples = static_cast<int>(rmsWindowMs * 0.001 * currentSampleRate);

    // Allocate circular buffer for RMS calculation
    rmsBuffer.setSize(1, rmsWindowSamples, false, true, false);
    rmsBuffer.clear();
    rmsBufferPosition = 0;
    rmsSum = 0.0f;
}

void EnvelopeFollower::setPeakHoldTime(float holdMs)
{
    peakHoldMs = juce::jlimit(1.0f, 50.0f, holdMs);

    // Calculate exponential decay coefficient
    // Formula: coeff = exp(-1 / (holdTime * sampleRate))
    float timeConstant = peakHoldMs * 0.001f * static_cast<float>(currentSampleRate);
    peakDecayCoeff = std::exp(-1.0f / timeConstant);
}

void EnvelopeFollower::reset()
{
    rmsBuffer.clear();
    rmsBufferPosition = 0;
    rmsSum = 0.0f;
    currentPeak = 0.0f;
}

float EnvelopeFollower::process(const float* samples, int numSamples)
{
    if (detectionMode == RMS)
        return detectRMS(samples, numSamples);
    else
        return detectPeak(samples, numSamples);
}

float EnvelopeFollower::processStereo(const float* leftSamples, const float* rightSamples, int numSamples)
{
    float leftLevel = process(leftSamples, numSamples);
    float rightLevel = process(rightSamples, numSamples);

    // Return maximum of both channels (for stereo-linked processing)
    return juce::jmax(leftLevel, rightLevel);
}

float EnvelopeFollower::detectRMS(const float* samples, int numSamples)
{
    float* bufferData = rmsBuffer.getWritePointer(0);

    // Process each sample
    for (int i = 0; i < numSamples; ++i)
    {
        float sample = samples[i];
        float squaredSample = sample * sample;

        // Remove oldest sample from sum
        float oldestSample = bufferData[rmsBufferPosition];
        rmsSum -= oldestSample;

        // Add new sample to sum
        bufferData[rmsBufferPosition] = squaredSample;
        rmsSum += squaredSample;

        // Advance circular buffer position
        rmsBufferPosition = (rmsBufferPosition + 1) % rmsWindowSamples;
    }

    // Calculate RMS: sqrt(average of squared samples)
    float rmsAverage = rmsSum / static_cast<float>(rmsWindowSamples);
    return std::sqrt(juce::jmax(0.0f, rmsAverage));  // Clamp to prevent negative sqrt
}

float EnvelopeFollower::detectPeak(const float* samples, int numSamples)
{
    // Find peak in this block
    float blockPeak = 0.0f;
    for (int i = 0; i < numSamples; ++i)
    {
        float absValue = std::abs(samples[i]);
        if (absValue > blockPeak)
            blockPeak = absValue;
    }

    // Update current peak with exponential decay
    if (blockPeak > currentPeak)
    {
        // Instant attack (new peak found)
        currentPeak = blockPeak;
    }
    else
    {
        // Exponential decay (no new peak)
        currentPeak *= peakDecayCoeff;
    }

    return currentPeak;
}

} // namespace audioforge
