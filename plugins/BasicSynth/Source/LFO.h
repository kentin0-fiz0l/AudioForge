#pragma once

#include <juce_core/juce_core.h>

/**
 * Low Frequency Oscillator (LFO)
 *
 * Generates modulation signals for synthesis parameters.
 * Supports multiple waveform shapes and tempo synchronization.
 */
class LFO
{
public:
    enum class Shape
    {
        Sine = 0,           // Smooth sine wave modulation
        Triangle = 1,       // Linear up/down modulation
        Sawtooth = 2,       // Linear rising modulation
        Square = 3,         // Binary on/off modulation
        SampleAndHold = 4   // Random stepped modulation
    };

    LFO();

    /**
     * Set the LFO rate in Hz (free-running mode).
     *
     * @param rateHz Rate in Hz (0.01 to 20 Hz typical)
     */
    void setRate(float rateHz);

    /**
     * Enable tempo synchronization.
     *
     * @param sync Enable/disable tempo sync
     * @param bpm Current tempo in BPM (if sync enabled)
     * @param division Note division (e.g., 1.0 = quarter note, 0.5 = eighth note)
     */
    void setTempoSync(bool sync, float bpm = 120.0f, float division = 1.0f);

    /**
     * Set the LFO waveform shape.
     *
     * @param shape Waveform shape
     */
    void setShape(Shape shape);

    /**
     * Get the next LFO sample.
     *
     * @param sampleRate Current sample rate
     * @return Modulation value in range [-1.0, 1.0]
     */
    float getNextSample(double sampleRate);

    /**
     * Reset the LFO phase to 0.
     */
    void reset();

    /**
     * Get current LFO phase (0.0 to 1.0).
     */
    float getPhase() const { return phase; }

private:
    float phase = 0.0f;         // Current phase (0.0 to 1.0)
    float rate = 1.0f;          // Rate in Hz
    Shape shape = Shape::Sine;  // Waveform shape
    bool tempoSynced = false;   // Tempo sync enabled
    float bpm = 120.0f;         // Tempo (if synced)
    float division = 1.0f;      // Note division (if synced)

    juce::Random random;        // For sample & hold

    /**
     * Calculate phase increment per sample.
     */
    float calculatePhaseIncrement(double sampleRate) const;

    /**
     * Generate sample for current phase and shape.
     */
    float generateSample() const;
};
