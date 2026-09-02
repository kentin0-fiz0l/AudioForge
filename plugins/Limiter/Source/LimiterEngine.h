/*
  ==============================================================================

    LimiterEngine.h
    Created: 1 Sep 2026
    Author:  AudioForge Team

    Core limiting engine with lookahead buffer, gain reduction calculation,
    and smooth release envelope.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "TruePeakDetector.h"

namespace audioforge
{

/**
 * @brief Professional limiting engine with lookahead and smooth gain reduction
 *
 * Implements a brickwall limiter with:
 * - Lookahead buffer (0-10ms) for smooth gain reduction
 * - Exponential release envelope (prevents pumping)
 * - Auto makeup gain
 * - True peak detection via TruePeakDetector
 */
class LimiterEngine
{
public:
    LimiterEngine();
    ~LimiterEngine() = default;

    //==============================================================================
    // Parameters

    /** Set output ceiling in dBFS (-12 to 0) */
    void setCeiling(float ceilingDb) { ceiling = juce::Decibels::decibelsToGain(ceilingDb); }

    /** Set threshold in dB (-24 to 0) */
    void setThreshold(float thresholdDb) { threshold = juce::Decibels::decibelsToGain(thresholdDb); }

    /** Set release time in milliseconds (10 to 1000) */
    void setRelease(float releaseMs);

    /** Set lookahead time in milliseconds (0 to 10) */
    void setLookahead(float lookaheadMs);

    /** Enable/disable auto makeup gain */
    void setAutoMakeupEnabled(bool enabled) { autoMakeupEnabled = enabled; }

    /** Set manual output trim in dB (-12 to +12) */
    void setOutputTrim(float trimDb) { outputTrim = juce::Decibels::decibelsToGain(trimDb); }

    //==============================================================================
    // Processing

    /**
     * @brief Prepare the limiter for processing
     *
     * @param sampleRate Sample rate in Hz
     * @param maxBlockSize Maximum samples per block
     */
    void prepare(double sampleRate, int maxBlockSize);

    /**
     * @brief Process a stereo audio buffer
     *
     * @param buffer Audio buffer to process (modified in-place)
     */
    void process(juce::AudioBuffer<float>& buffer);

    /**
     * @brief Reset internal state
     */
    void reset();

    //==============================================================================
    // Metering

    /** Get current gain reduction in dB */
    float getGainReductionDb() const { return juce::Decibels::gainToDecibels(currentGainReduction); }

    /** Get input peak level in dB */
    float getInputLevelDb() const { return juce::Decibels::gainToDecibels(inputPeak); }

    /** Get output peak level in dB */
    float getOutputLevelDb() const { return juce::Decibels::gainToDecibels(outputPeak); }

    /** Get true peak level in dB */
    float getTruePeakDb() const { return juce::Decibels::gainToDecibels(truePeak); }

    /** Check if true peak exceeded 0 dBTP */
    bool isTruePeakClipping() const { return truePeak > 1.0f; }

private:
    //==============================================================================
    // DSP Components

    TruePeakDetector truePeakDetector;

    // Lookahead delay line (circular buffer)
    juce::AudioBuffer<float> lookaheadBuffer;
    int lookaheadWritePosition = 0;
    int lookaheadReadPosition = 0;
    int lookaheadSamples = 0;

    //==============================================================================
    // Parameters (linear gain)

    float ceiling = 1.0f;           // Output ceiling (default 0 dBFS)
    float threshold = 0.5f;         // Threshold (default -6 dB)
    float releaseTimeMs = 100.0f;   // Release time in ms
    float releaseCoeff = 0.99f;     // Exponential release coefficient
    bool autoMakeupEnabled = true;  // Auto makeup gain
    float outputTrim = 1.0f;        // Manual output trim

    //==============================================================================
    // State

    double currentSampleRate = 44100.0;
    int maxSamplesPerBlock = 512;

    float currentGainReduction = 1.0f;  // Current gain reduction (linear, 0-1)
    float targetGainReduction = 1.0f;   // Target gain reduction

    // Metering
    float inputPeak = 0.0f;
    float outputPeak = 0.0f;
    float truePeak = 0.0f;

    //==============================================================================
    // Helper Methods

    /**
     * @brief Calculate gain reduction for a given input level
     *
     * @param inputLevel Input level (linear, 0-1)
     * @return Gain reduction to apply (linear, 0-1)
     */
    float calculateGainReduction(float inputLevel);

    /**
     * @brief Smooth gain reduction with exponential release
     *
     * @param target Target gain reduction
     * @param current Current gain reduction
     * @return Smoothed gain reduction
     */
    float smoothGainReduction(float target, float current);

    /**
     * @brief Calculate auto makeup gain based on threshold
     *
     * @return Makeup gain (linear)
     */
    float calculateMakeupGain();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LimiterEngine)
};

} // namespace audioforge
