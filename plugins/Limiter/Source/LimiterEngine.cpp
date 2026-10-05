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

    // Allocate everything the lookahead needs at its longest (10ms)
    const int maxLookaheadSamples = static_cast<int>(std::ceil(sampleRate * 0.010));
    lookaheadBuffer.setSize(2, maxLookaheadSamples + 1, false, true, false);
    heldGains.assign(static_cast<size_t>(maxLookaheadSamples + 1), { 1.0f, 0 });
    gainWindow.assign(static_cast<size_t>(maxLookaheadSamples), 1.0f);

    // Both depend on the sample rate
    setRelease(releaseTimeMs);
    lookaheadSamples = -1;
    setLookahead(lookaheadTimeMs);

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
    lookaheadTimeMs = lookaheadMs;

    // Never more than was allocated for, which is 10ms
    const int samples = juce::jlimit(0, static_cast<int>(gainWindow.size()),
                                     juce::roundToInt(lookaheadMs * 0.001 * currentSampleRate));

    // The plugin sets this before every block, so only a change may clear
    // the delay line
    if (samples == lookaheadSamples)
        return;

    lookaheadSamples = samples;
    clearLookahead();
}

void LimiterEngine::clearLookahead()
{
    lookaheadBuffer.clear();
    lookaheadWritePosition = 0;

    heldHead = 0;
    heldCount = 0;
    sampleCount = 0;

    std::fill(gainWindow.begin(), gainWindow.end(), 1.0f);
    gainWindowPosition = 0;
    gainWindowSum = static_cast<double>(lookaheadSamples);
}

float LimiterEngine::lowestGainOverLookahead(float gain)
{
    const int capacity = static_cast<int>(heldGains.size());
    const auto at = [&] (int offset) -> HeldGain& { return heldGains[static_cast<size_t>((heldHead + offset) % capacity)]; };

    // Drop the oldest once the delayed signal has passed it. This comes
    // before the new sample goes in: the queue has room for one lookahead
    // and no more, and a level that keeps falling fills it.
    if (heldCount > 0 && at(0).sample < sampleCount - lookaheadSamples)
    {
        heldHead = (heldHead + 1) % capacity;
        --heldCount;
    }

    // A sample asking for less gain outlasts every earlier one asking for more
    while (heldCount > 0 && at(heldCount - 1).gain >= gain)
        --heldCount;

    at(heldCount) = { gain, sampleCount };
    ++heldCount;
    ++sampleCount;
    return at(0).gain;
}

float LimiterEngine::averageGainOverLookahead(float gain)
{
    auto& oldest = gainWindow[static_cast<size_t>(gainWindowPosition)];
    gainWindowSum += static_cast<double>(gain) - static_cast<double>(oldest);
    oldest = gain;

    if (++gainWindowPosition >= lookaheadSamples)
    {
        gainWindowPosition = 0;

        // Add the window up afresh once per pass, so that rounding in the
        // running sum cannot build up over hours of audio
        gainWindowSum = 0.0;
        for (int i = 0; i < lookaheadSamples; ++i)
            gainWindowSum += static_cast<double>(gainWindow[static_cast<size_t>(i)]);
    }

    return static_cast<float>(gainWindowSum / lookaheadSamples);
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

    // Makeup gain is applied after the gain reduction, so the reduction has
    // to allow for it or the result lands above the ceiling
    const float makeupGain = autoMakeupEnabled ? calculateMakeupGain() : 1.0f;

    // Process each sample
    for (int i = 0; i < numSamples; ++i)
    {
        // Read input samples
        float leftIn = leftChannel[i];
        float rightIn = rightChannel[i];

        // Measure input peak
        float inputLevel = std::max(std::abs(leftIn), std::abs(rightIn));
        inputPeak = std::max(inputPeak * 0.999f, inputLevel);

        // === GAIN REDUCTION CALCULATION ===
        // Gain reduction that keeps the level after makeup at or under the ceiling
        targetGainReduction = calculateGainReduction(inputLevel * makeupGain);

        // === LOOKAHEAD ===
        float gain;

        if (lookaheadSamples > 0)
        {
            // Delay the signal by the lookahead
            const int size = lookaheadBuffer.getNumSamples();
            const int readPosition = (lookaheadWritePosition + size - lookaheadSamples) % size;

            lookaheadBuffer.setSample(0, lookaheadWritePosition, leftIn);
            lookaheadBuffer.setSample(1, lookaheadWritePosition, rightIn);
            leftIn = lookaheadBuffer.getSample(0, readPosition);
            rightIn = lookaheadBuffer.getSample(1, readPosition);
            lookaheadWritePosition = (lookaheadWritePosition + 1) % size;

            // Hold the reduction a peak needs from the moment it is seen
            // until its delayed copy has gone by, release from there, and
            // average over the lookahead. The average reaches the full
            // reduction exactly as the peak comes out of the delay.
            const float held = lowestGainOverLookahead(targetGainReduction);
            currentGainReduction = smoothGainReduction(held, currentGainReduction);
            gain = averageGainOverLookahead(currentGainReduction);
        }
        else
        {
            // No lookahead: the gain drops at the peak itself
            currentGainReduction = smoothGainReduction(targetGainReduction, currentGainReduction);
            gain = currentGainReduction;
        }

        // === APPLY GAIN REDUCTION ===
        float leftOut = leftIn * gain;
        float rightOut = rightIn * gain;

        // === AUTO MAKEUP GAIN ===
        leftOut *= makeupGain;
        rightOut *= makeupGain;

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

float LimiterEngine::calculateGainReduction(float level)
{
    // No reduction at or under the ceiling
    if (level <= ceiling)
        return 1.0f;

    // Brickwall limiting (infinite ratio): bring the level down to the ceiling
    return ceiling / level;
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
    // Auto makeup gain raises the signal so that the threshold level comes
    // out at the ceiling. It never turns the signal down, and stops at 4x.

    if (threshold > 0.0f)
        return juce::jlimit(1.0f, 4.0f, ceiling / threshold);

    return 1.0f;
}

void LimiterEngine::reset()
{
    truePeakDetector.reset();
    clearLookahead();

    currentGainReduction = 1.0f;
    targetGainReduction = 1.0f;

    inputPeak = 0.0f;
    outputPeak = 0.0f;
    truePeak = 0.0f;
}

} // namespace audioforge
