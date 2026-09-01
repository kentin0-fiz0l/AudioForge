#pragma once

#include <synth/ADSREnvelope.h>
#include "DrawbarEngine.h"
#include "LeslieSimulator.h"

/**
 * Voice - Single organ voice
 *
 * Combines drawbar engine, Leslie effect, and percussion envelope
 */
class Voice
{
public:
    Voice();

    void prepare(double sampleRate, int maxBlockSize);
    void noteOn(int midiNote, float velocity, double sampleRate);
    void noteOff();

    void processSample(double sampleRate,
                      const float drawbars[9],
                      bool leslieSpeed,
                      float leslieDepth,
                      float leslieMix,
                      float percussionClick,
                      float percussionDecay,
                      float& outLeft,
                      float& outRight);

    bool isActive() const { return active; }
    int getCurrentNote() const { return currentNote; }
    int getMidiNote() const { return currentNote; }
    float getLevel() const { return velocity; }
    void reset();

private:
    DrawbarEngine drawbarEngine;
    LeslieSimulator leslie;
    ADSREnvelope percussionEnv;

    int currentNote;
    float velocity;
    bool active;

    // Percussion settings
    float clickAmount;
    float clickDecayTime;
};
