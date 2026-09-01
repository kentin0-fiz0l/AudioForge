#pragma once

#include <synth/ADSREnvelope.h>
#include <synth/MultiModeFilter.h>
#include "UnisonVoice.h"
#include <array>

/**
 * Pad Synth Voice
 *
 * Single pad voice with:
 * - Unison ensemble (3-9 detuned oscillators)
 * - Multi-mode filter
 * - ADSR envelope
 * - Stereo spreading
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
     * Process one sample and output stereo pair.
     * @param sampleRate Sample rate
     * @param waveform Oscillator waveform (0-3)
     * @param unisonVoices Number of unison voices (3-9)
     * @param detuneAmount Detune spread in cents (0-50)
     * @param stereoWidth Stereo spread amount (0.0-1.0)
     * @param filterCutoff Filter cutoff frequency
     * @param filterResonance Filter resonance
     * @param filterType Filter type (0-3)
     * @param outLeft Output left channel
     * @param outRight Output right channel
     */
    void processSample(double sampleRate,
                      int waveform,
                      int unisonVoices,
                      float detuneAmount,
                      float stereoWidth,
                      float filterCutoff,
                      float filterResonance,
                      int filterType,
                      float& outLeft,
                      float& outRight);

    /**
     * Update envelope parameters.
     */
    void updateEnvelope(float attack, float decay, float sustain, float release);

private:
    static constexpr int MAX_UNISON_VOICES = 9;

    bool active = false;
    int currentNote = -1;
    float velocity = 0.0f;
    float baseFrequency = 440.0f;

    std::array<UnisonVoice, MAX_UNISON_VOICES> unisonVoices;
    ADSREnvelope envelope;
    MultiModeFilter filterL; // Stereo filters
    MultiModeFilter filterR;

    void updateUnisonVoices(int waveform, int numVoices, float detuneAmount, float stereoWidth);
};
