/*
  ==============================================================================

    GateEngine.cpp
    Created: 1 Sep 2026
    Author:  AudioForge Team

  ==============================================================================
*/

#include "GateEngine.h"

namespace audioforge
{

GateEngine::GateEngine()
{
}

void GateEngine::prepare(double sampleRate, int maxBlockSize)
{
    currentSampleRate = sampleRate;
    maxSamplesPerBlock = maxBlockSize;

    // Prepare sub-components
    envelopeFollower.prepare(sampleRate, maxBlockSize);
    sidechain.prepare(sampleRate, maxBlockSize);

    // Allocate lookahead buffer (stereo, max 10ms)
    int maxLookaheadSamples = static_cast<int>(0.010 * sampleRate);  // 10ms
    lookaheadBuffer.setSize(2, maxLookaheadSamples + maxBlockSize, false, true, false);

    // Set initial lookahead
    setLookahead(2.0f);  // Default 2ms

    // Calculate envelope coefficients
    setAttack(attackTimeMs);
    setRelease(releaseTimeMs);
    setHold(holdTimeMs);

    reset();
}

void GateEngine::setThreshold(float thresholdDb)
{
    threshold = juce::Decibels::decibelsToGain(thresholdDb);
}

void GateEngine::setRatio(float newRatio)
{
    ratio = juce::jlimit(1.0f, 20.0f, newRatio);
}

void GateEngine::setRange(float rangeDb)
{
    // Range is the maximum gain reduction (0 dB = no reduction, -96 dB = silence)
    range = juce::Decibels::decibelsToGain(rangeDb);
}

void GateEngine::setAttack(float attackMs)
{
    attackTimeMs = juce::jlimit(0.1f, 100.0f, attackMs);
    attackCoeff = timeConstantToCoeff(attackTimeMs);
}

void GateEngine::setHold(float holdMs)
{
    holdTimeMs = juce::jlimit(0.0f, 1000.0f, holdMs);
    holdSamplesTotal = static_cast<int>(holdTimeMs * 0.001 * currentSampleRate);
}

void GateEngine::setRelease(float releaseMs)
{
    releaseTimeMs = juce::jlimit(10.0f, 1000.0f, releaseMs);
    releaseCoeff = timeConstantToCoeff(releaseTimeMs);
}

void GateEngine::setLookahead(float lookaheadMs)
{
    lookaheadMs = juce::jlimit(0.0f, 10.0f, lookaheadMs);
    lookaheadSamples = static_cast<int>(lookaheadMs * 0.001 * currentSampleRate);

    // Update read position (write position stays at 0)
    lookaheadReadPosition = lookaheadSamples;
}

void GateEngine::reset()
{
    lookaheadBuffer.clear();
    lookaheadWritePosition = 0;
    lookaheadReadPosition = lookaheadSamples;

    envelopeFollower.reset();
    sidechain.reset();

    currentState = Closed;
    currentGainReduction = range;  // Start closed (full reduction)
    targetGainReduction = range;
    holdSamplesRemaining = 0;

    inputPeak = 0.0f;
    outputPeak = 0.0f;
}

void GateEngine::process(juce::AudioBuffer<float>& buffer,
                         const juce::AudioBuffer<float>* sidechainBuffer)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    if (numSamples == 0 || numChannels == 0)
        return;

    // Get audio pointers
    float* leftChannel = buffer.getWritePointer(0);
    float* rightChannel = numChannels > 1 ? buffer.getWritePointer(1) : nullptr;

    // Determine detection source (sidechain or main input)
    const float* detectionLeft = leftChannel;
    const float* detectionRight = rightChannel;

    if (sidechainBuffer != nullptr && sidechain.isEnabled())
    {
        // Use sidechain input for detection
        detectionLeft = sidechainBuffer->getReadPointer(0);
        detectionRight = sidechainBuffer->getNumChannels() > 1
            ? sidechainBuffer->getReadPointer(1)
            : detectionLeft;

        // Apply sidechain filtering (modifies sidechain buffer in-place)
        // Note: We'll need a temporary buffer for this in real implementation
        // For now, we'll process the main input's sidechain directly
    }

    // Process each sample
    for (int i = 0; i < numSamples; ++i)
    {
        //==============================================================================
        // 1. Detect input level from sidechain or main input

        float detectionSample = detectionLeft[i];
        if (detectionRight != nullptr)
        {
            // Stereo: use max of both channels
            detectionSample = juce::jmax(std::abs(detectionLeft[i]),
                                         std::abs(detectionRight[i]));
        }
        else
        {
            detectionSample = std::abs(detectionSample);
        }

        // Run through envelope follower
        float inputLevel = envelopeFollower.process(&detectionSample, 1);

        // Update input metering
        if (inputLevel > inputPeak)
            inputPeak = inputLevel;

        //==============================================================================
        // 2. Calculate target gain reduction

        targetGainReduction = calculateGainReduction(inputLevel);

        //==============================================================================
        // 3. Smooth gain reduction with envelope shaping

        float smoothedGain = updateEnvelope(targetGainReduction);

        //==============================================================================
        // 4. Write input to lookahead buffer

        float* lookaheadLeft = lookaheadBuffer.getWritePointer(0);
        float* lookaheadRight = lookaheadBuffer.getWritePointer(1);

        lookaheadLeft[lookaheadWritePosition] = leftChannel[i];
        if (rightChannel != nullptr)
            lookaheadRight[lookaheadWritePosition] = rightChannel[i];

        //==============================================================================
        // 5. Read delayed audio from lookahead buffer

        int bufferSize = lookaheadBuffer.getNumSamples();
        int readPos = (lookaheadWritePosition - lookaheadSamples + bufferSize) % bufferSize;

        float delayedLeft = lookaheadLeft[readPos];
        float delayedRight = rightChannel != nullptr ? lookaheadRight[readPos] : 0.0f;

        //==============================================================================
        // 6. Apply gain reduction to delayed audio

        leftChannel[i] = delayedLeft * smoothedGain;
        if (rightChannel != nullptr)
            rightChannel[i] = delayedRight * smoothedGain;

        // Sanitize output to prevent NaN/Inf propagation
        if (!std::isfinite(leftChannel[i]))
            leftChannel[i] = 0.0f;
        if (rightChannel != nullptr && !std::isfinite(rightChannel[i]))
            rightChannel[i] = 0.0f;

        // Update output metering
        float outputSample = std::abs(leftChannel[i]);
        if (rightChannel != nullptr)
            outputSample = juce::jmax(outputSample, std::abs(rightChannel[i]));

        if (outputSample > outputPeak)
            outputPeak = outputSample;

        //==============================================================================
        // 7. Advance lookahead buffer position

        lookaheadWritePosition = (lookaheadWritePosition + 1) % bufferSize;
    }

    // Store current gain reduction for metering (guard against division by zero)
    if (range > 0.00001f)
        currentGainReduction = 1.0f - (currentGainReduction / range);  // Convert to 0-1 for metering
    else
        currentGainReduction = 1.0f;  // Fully open if range is effectively zero
}

float GateEngine::calculateGainReduction(float inputLevel)
{
    if (processingMode == Gate)
    {
        //==============================================================================
        // Gate Mode: Infinite ratio (instant silence below threshold)

        if (inputLevel >= threshold)
        {
            // Above threshold: full volume (no reduction)
            return 1.0f;
        }
        else
        {
            // Below threshold: apply range (max reduction)
            return range;
        }
    }
    else
    {
        //==============================================================================
        // Expander Mode: Gradual reduction based on ratio

        if (inputLevel >= threshold)
        {
            // Above threshold: full volume (no reduction)
            return 1.0f;
        }
        else
        {
            // Below threshold: calculate expansion
            // Guard against very small values to prevent NaN from dB conversion
            const float minLevel = 0.00001f;  // -100 dB
            float safeInputLevel = juce::jmax(inputLevel, minLevel);
            float safeThreshold = juce::jmax(threshold, minLevel);

            float inputDb = juce::Decibels::gainToDecibels(safeInputLevel);
            float thresholdDb = juce::Decibels::gainToDecibels(safeThreshold);

            // How far below threshold?
            float excessDb = thresholdDb - inputDb;

            // Apply ratio to excess
            float reductionDb = excessDb * (ratio - 1.0f) / ratio;

            // Convert back to linear gain
            float gainReduction = juce::Decibels::decibelsToGain(-reductionDb);

            // Clamp to range
            return juce::jmax(range, gainReduction);
        }
    }
}

float GateEngine::updateEnvelope(float targetGain)
{
    //==============================================================================
    // State machine for envelope shaping (Attack/Hold/Release)

    if (targetGain > currentGainReduction)
    {
        // Opening gate (Attack)
        currentState = Attack;
        holdSamplesRemaining = 0;

        // Exponential smoothing (attack)
        currentGainReduction += (targetGain - currentGainReduction) * (1.0f - attackCoeff);
    }
    else if (targetGain < currentGainReduction)
    {
        // Closing gate
        if (currentState == Attack || currentState == Open)
        {
            // Transition to Hold
            currentState = Hold;
            holdSamplesRemaining = holdSamplesTotal;
        }

        if (currentState == Hold)
        {
            // Hold: keep current gain, decrement counter
            holdSamplesRemaining--;

            if (holdSamplesRemaining <= 0)
            {
                // Hold expired, transition to Release
                currentState = Release;
            }
        }

        if (currentState == Release)
        {
            // Exponential smoothing (release)
            currentGainReduction += (targetGain - currentGainReduction) * (1.0f - releaseCoeff);
        }
    }
    else
    {
        // Gate is stable (Open or Closed)
        if (currentGainReduction >= 0.99f)
            currentState = Open;
        else if (currentGainReduction <= range * 1.01f)
            currentState = Closed;
    }

    // Sanitize before returning (prevent NaN/Inf propagation)
    if (!std::isfinite(currentGainReduction))
        currentGainReduction = range;  // Failsafe: close gate if NaN detected

    return currentGainReduction;
}

float GateEngine::timeConstantToCoeff(float timeMs)
{
    // Convert time constant to exponential smoothing coefficient
    // Formula: coeff = exp(-1 / (timeMs * 0.001 * sampleRate))
    float timeConstant = timeMs * 0.001f * static_cast<float>(currentSampleRate);
    return std::exp(-1.0f / timeConstant);
}

float GateEngine::getGainReductionDb() const
{
    // Convert linear gain reduction to dB
    float gainReductionLinear = 1.0f - currentGainReduction;
    return juce::Decibels::gainToDecibels(gainReductionLinear);
}

float GateEngine::getInputLevelDb() const
{
    return juce::Decibels::gainToDecibels(inputPeak);
}

float GateEngine::getOutputLevelDb() const
{
    return juce::Decibels::gainToDecibels(outputPeak);
}

} // namespace audioforge
