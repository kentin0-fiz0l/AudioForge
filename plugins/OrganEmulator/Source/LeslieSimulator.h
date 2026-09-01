#pragma once

#include <juce_dsp/juce_dsp.h>

/**
 * LeslieSimulator - Rotating speaker cabinet effect
 *
 * Simulates the Doppler shift and amplitude modulation of a Leslie speaker
 * using chorus and vibrato effects.
 */
class LeslieSimulator
{
public:
    LeslieSimulator();

    void prepare(double sampleRate, int maxBlockSize);
    void setSpeed(bool fast); // false = slow, true = fast
    void setDepth(float depth); // 0.0-1.0
    void setMix(float mix); // 0.0-1.0 (dry/wet)

    float processSample(float input);
    void reset();

private:
    // LFO for modulation
    float lfoPhase;
    float lfoRate;
    bool fastSpeed;

    // Delay line for chorus effect
    static constexpr int MAX_DELAY_SAMPLES = 8192;
    float delayBuffer[MAX_DELAY_SAMPLES];
    int delayWritePos;

    // Parameters
    float depth;
    float mix;
    double sampleRate;

    // Speed constants (Hz)
    static constexpr float SLOW_SPEED = 0.7f;
    static constexpr float FAST_SPEED = 6.0f;
};
