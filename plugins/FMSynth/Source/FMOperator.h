#pragma once

#include <synth/ADSREnvelope.h>
#include <cmath>

/**
 * FM Operator
 *
 * A single FM operator consisting of a sine wave oscillator and ADSR envelope.
 * Can be used as either a carrier (audio output) or modulator (frequency modulation source).
 */
class FMOperator
{
public:
    FMOperator();

    void setFrequency(float frequencyHz);
    void setRatio(float ratio);
    void setLevel(float level);
    void setEnvelope(float attack, float decay, float sustain, float release);
    void trigger();
    void release();
    void reset();

    /**
     * Process one sample with optional phase modulation.
     */
    float processSample(double sampleRate, float phaseModulation = 0.0f);

    bool isActive() const { return envelope.isActive(); }
    float getEnvelopeLevel() const { return envelope.getLevel(); }

private:
    float baseFrequency = 440.0f;
    float ratio = 1.0f;
    float phase = 0.0f;
    float level = 1.0f;

    ADSREnvelope envelope;

    void advancePhase(double sampleRate);
};
