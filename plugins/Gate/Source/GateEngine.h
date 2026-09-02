/*
  ==============================================================================

    GateEngine.h
    Created: 1 Sep 2026
    Author:  AudioForge Team

    Core gating/expansion engine with envelope shaping, lookahead buffer,
    and sidechain integration.

  ==============================================================================
*/

#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>
#include "../../../shared/dsp/EnvelopeFollower.h"
#include "../../../shared/dsp/Sidechain.h"

namespace audioforge
{

/**
 * @brief Professional gate/expander engine
 *
 * Implements:
 * - Hard gate (infinite ratio, instant silence below threshold)
 * - Soft expander (adjustable ratio, gradual reduction below threshold)
 * - Envelope shaping (attack/hold/release)
 * - Lookahead buffer (prevents cutting off transients)
 * - Sidechain triggering (external audio controls gating)
 *
 * Signal Flow:
 * Input → Lookahead Buffer → Delayed Audio ─┐
 *    ↓                                        ↓
 * Sidechain → Envelope Follower → Level      ↓
 *    ↓                                        ↓
 * Gain Reduction Calculator ← Threshold      ↓
 *    ↓                                        ↓
 * Envelope Shaper ← Attack/Hold/Release      ↓
 *    ↓                                        ↓
 * Apply Gain Reduction ←──────────────────────┘
 *    ↓
 * Output
 */
class GateEngine
{
public:
    enum ProcessingMode
    {
        Gate,      ///< Hard gate (infinite ratio, instant silence)
        Expander   ///< Soft expander (adjustable ratio, gradual reduction)
    };

    GateEngine();
    ~GateEngine() = default;

    //==============================================================================
    // Parameters

    /** Set threshold in dB (-60 to 0) */
    void setThreshold(float thresholdDb);

    /** Set ratio (1:1 to 20:1, or infinite for gate mode) */
    void setRatio(float ratio);

    /** Set range (maximum gain reduction in dB, 0 to -96) */
    void setRange(float rangeDb);

    /** Set attack time in milliseconds (0.1 to 100) */
    void setAttack(float attackMs);

    /** Set hold time in milliseconds (0 to 1000) */
    void setHold(float holdMs);

    /** Set release time in milliseconds (10 to 1000) */
    void setRelease(float releaseMs);

    /** Set lookahead time in milliseconds (0 to 10) */
    void setLookahead(float lookaheadMs);

    /** Set processing mode (Gate or Expander) */
    void setMode(ProcessingMode mode) { processingMode = mode; }

    //==============================================================================
    // Processing

    /**
     * @brief Prepare the gate for processing
     *
     * @param sampleRate Sample rate in Hz
     * @param maxBlockSize Maximum samples per block
     */
    void prepare(double sampleRate, int maxBlockSize);

    /**
     * @brief Process a stereo audio buffer
     *
     * @param buffer Audio buffer to process (modified in-place)
     * @param sidechainBuffer Optional sidechain input (nullptr to use main input)
     */
    void process(juce::AudioBuffer<float>& buffer,
                 const juce::AudioBuffer<float>* sidechainBuffer = nullptr);

    /**
     * @brief Reset internal state
     */
    void reset();

    //==============================================================================
    // Metering

    /** Get current gain reduction in dB */
    float getGainReductionDb() const;

    /** Get input peak level in dB */
    float getInputLevelDb() const;

    /** Get output peak level in dB */
    float getOutputLevelDb() const;

    //==============================================================================
    // Component Access (for UI configuration)

    EnvelopeFollower& getEnvelopeFollower() { return envelopeFollower; }
    Sidechain& getSidechain() { return sidechain; }

private:
    //==============================================================================
    // DSP Components

    EnvelopeFollower envelopeFollower;
    Sidechain sidechain;

    // Lookahead delay line (circular buffer)
    juce::AudioBuffer<float> lookaheadBuffer;
    int lookaheadWritePosition = 0;
    int lookaheadReadPosition = 0;
    int lookaheadSamples = 0;

    //==============================================================================
    // Parameters (linear gain where applicable)

    float threshold = 0.5f;          // Threshold (linear, default -6 dB)
    float ratio = 10.0f;             // Compression ratio (default 10:1)
    float range = 0.001f;            // Max gain reduction (linear, default -60 dB)
    float attackTimeMs = 1.0f;       // Attack time in ms
    float holdTimeMs = 50.0f;        // Hold time in ms
    float releaseTimeMs = 100.0f;    // Release time in ms
    ProcessingMode processingMode = Gate;

    // Envelope coefficients
    float attackCoeff = 0.99f;       // Exponential attack coefficient
    float releaseCoeff = 0.99f;      // Exponential release coefficient

    //==============================================================================
    // State

    enum EnvelopeState { Closed, Attack, Open, Hold, Release };
    EnvelopeState currentState = Closed;

    double currentSampleRate = 44100.0;
    int maxSamplesPerBlock = 512;

    float currentGainReduction = 0.0f;  // Current gain reduction (linear, 0-1)
    float targetGainReduction = 0.0f;   // Target gain reduction

    // Hold state
    int holdSamplesRemaining = 0;
    int holdSamplesTotal = 0;

    // Metering
    float inputPeak = 0.0f;
    float outputPeak = 0.0f;

    //==============================================================================
    // Helper Methods

    /**
     * @brief Calculate target gain reduction for a given input level
     *
     * @param inputLevel Input level (linear, 0-1)
     * @return Target gain reduction to apply (linear, 0-1)
     */
    float calculateGainReduction(float inputLevel);

    /**
     * @brief Update envelope state and smooth gain reduction
     *
     * @param targetGain Target gain reduction
     * @return Smoothed gain reduction
     */
    float updateEnvelope(float targetGain);

    /**
     * @brief Calculate time constant coefficient for exponential smoothing
     *
     * @param timeMs Time constant in milliseconds
     * @return Smoothing coefficient (0-1)
     */
    float timeConstantToCoeff(float timeMs);

    /**
     * @brief Update lookahead buffer and return delayed sample
     *
     * @param sample Input sample
     * @return Delayed sample (lookahead milliseconds earlier)
     */
    float processLookahead(float sample);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GateEngine)
};

} // namespace audioforge
