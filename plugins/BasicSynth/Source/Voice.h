#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "Oscillator.h"

/**
 * Synthesizer Voice
 *
 * Represents a single voice in the polyphonic synthesizer.
 * Each voice has its own oscillator, envelope, and filter state.
 */
class SynthVoice
{
public:
    SynthVoice();

    /**
     * Start playing a note.
     *
     * @param midiNoteNumber MIDI note number (0-127)
     * @param velocity Note velocity (0.0-1.0)
     * @param sampleRate Current sample rate
     */
    void noteOn(int midiNoteNumber, float velocity, double sampleRate);

    /**
     * Stop playing the current note (enter release phase).
     */
    void noteOff();

    /**
     * Process one sample of audio for this voice.
     *
     * @param sampleRate Current sample rate
     * @param waveform Oscillator waveform (0=sine, 1=saw, 2=square)
     * @param attack Attack time in seconds
     * @param decay Decay time in seconds
     * @param sustain Sustain level (0.0-1.0)
     * @param release Release time in seconds
     * @return Output sample value
     */
    float processSample(double sampleRate,
                       int waveform,
                       float attack,
                       float decay,
                       float sustain,
                       float release);

    /**
     * Check if this voice is currently playing.
     */
    bool isActive() const { return active; }

    /**
     * Get the MIDI note number this voice is playing.
     */
    int getMidiNote() const { return currentNote; }

    /**
     * Force the voice to stop immediately.
     */
    void reset();

private:
    // Voice state
    bool active = false;
    int currentNote = -1;
    float velocity = 0.0f;

    // Oscillator
    Oscillator oscillator;

    // ADSR envelope
    enum class EnvelopeStage
    {
        Off,
        Attack,
        Decay,
        Sustain,
        Release
    };

    EnvelopeStage envelopeStage = EnvelopeStage::Off;
    float envelopeLevel = 0.0f;
    float envelopeTime = 0.0f; // Time in current stage (seconds)
    float releaseStartLevel = 0.0f; // Level at start of release phase

    // Helper methods
    void updateEnvelope(float deltaTime, float attack, float decay,
                       float sustain, float release);
};
