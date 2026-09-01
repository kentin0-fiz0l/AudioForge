#pragma once

#include <synth/ADSREnvelope.h>
#include <synth/MultiModeFilter.h>
#include "WavetableOscillator.h"

/**
 * Wavetable Synth Voice
 *
 * Single voice with:
 * - Wavetable oscillator
 * - Multi-mode filter
 * - Amplitude ADSR
 * - Filter ADSR
 */
class Voice
{
public:
    Voice();

    void noteOn(int midiNote, float velocity, double sampleRate);
    void noteOff();
    bool isActive() const;
    int getMidiNote() const { return currentNote; }
    float getLevel() const;
    void reset();

    /**
     * Process one sample.
     */
    float processSample(double sampleRate,
                       float wavetablePosition,
                       float filterCutoff,
                       float filterResonance,
                       int filterType,
                       float filterEnvAmount);

    /**
     * Update envelopes.
     */
    void updateEnvelopes(float ampAttack, float ampDecay, float ampSustain, float ampRelease,
                        float filtAttack, float filtDecay, float filtSustain, float filtRelease);

private:
    bool active = false;
    int currentNote = -1;
    float velocity = 0.0f;

    WavetableOscillator oscillator;
    MultiModeFilter filter;
    ADSREnvelope ampEnvelope;
    ADSREnvelope filterEnvelope;
};
