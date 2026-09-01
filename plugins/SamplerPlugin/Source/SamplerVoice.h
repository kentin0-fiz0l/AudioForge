#pragma once

#include "Sample.h"
#include "SampleLibrary.h"
#include "KeyZone.h"
#include <synth/ADSREnvelope.h>
#include <synth/MultiModeFilter.h>
#include <memory>

/**
 * Sampler Voice
 *
 * Implements voice interface for polyphonic sample playback.
 * Each voice can play one sample at a time with pitch shifting,
 * envelope shaping, and filtering.
 */
class SamplerVoice
{
public:
    SamplerVoice();

    /**
     * Start playing a note (VoiceManager interface).
     *
     * @param midiNote MIDI note number (0-127)
     * @param velocity Note velocity (0.0-1.0)
     * @param sampleRate Current sample rate
     */
    void noteOn(int midiNote, float velocity, double sampleRate);

    /**
     * Stop playing the current note (VoiceManager interface).
     */
    void noteOff();

    /**
     * Reset voice to inactive state (VoiceManager interface).
     */
    void reset();

    /**
     * Check if voice is active (VoiceManager interface).
     */
    bool isActive() const { return active; }

    /**
     * Get current MIDI note (VoiceManager interface).
     */
    int getMidiNote() const { return currentNote; }

    /**
     * Get current level for voice stealing (VoiceManager interface).
     */
    float getLevel() const;

    /**
     * Set the current sample and zone to play.
     * Called during noteOn when zone lookup succeeds.
     *
     * @param sample Shared pointer to sample
     * @param zone Zone containing this note
     */
    void setSample(std::shared_ptr<Sample> sample, const KeyZone* zone);

    /**
     * Process one sample of audio.
     *
     * @param sampleRate Current sample rate
     * @param attack Amplitude envelope attack time (seconds)
     * @param decay Amplitude envelope decay time (seconds)
     * @param sustain Amplitude envelope sustain level (0.0-1.0)
     * @param release Amplitude envelope release time (seconds)
     * @param filterCutoff Filter cutoff frequency (Hz)
     * @param filterResonance Filter resonance/Q
     * @param filterType Filter type (0=LP, 1=HP, 2=BP, 3=Notch)
     * @param filterEnvAmount Filter envelope modulation amount (-1.0 to 1.0)
     * @param filtAttack Filter envelope attack time (seconds)
     * @param filtDecay Filter envelope decay time (seconds)
     * @param filtSustain Filter envelope sustain level (0.0-1.0)
     * @param filtRelease Filter envelope release time (seconds)
     * @return Processed audio sample (mono)
     */
    float processSample(double sampleRate,
                       float attack, float decay, float sustain, float release,
                       float filterCutoff, float filterResonance, int filterType,
                       float filterEnvAmount,
                       float filtAttack, float filtDecay, float filtSustain, float filtRelease);

private:
    bool active = false;
    int currentNote = -1;
    float noteVelocity = 0.0f;

    std::shared_ptr<Sample> currentSample;
    const KeyZone* currentZone = nullptr;

    double playbackPosition = 0.0;  // Current position in sample (fractional)
    double playbackRate = 1.0;      // Pitch multiplier
    int loopDirection = 1;          // 1=forward, -1=reverse (for pingpong)

    ADSREnvelope ampEnvelope;
    ADSREnvelope filterEnvelope;
    MultiModeFilter filter;

    /**
     * Get interpolated sample value from audio buffer.
     * Uses linear interpolation for smooth playback.
     *
     * @param buffer Audio buffer to read from
     * @param channel Channel to read (0 or 1)
     * @param position Fractional sample position
     * @return Interpolated sample value
     */
    float getInterpolatedSample(const juce::AudioBuffer<float>& buffer,
                               int channel, double position) const;

    /**
     * Calculate playback rate for pitch shifting.
     * Uses equal temperament formula: f = 440 * 2^((n-69)/12)
     *
     * @param midiNote Target MIDI note
     * @param rootNote Root note of the sample
     * @param sampleRate Original sample rate
     * @param targetSampleRate Target playback sample rate
     * @return Playback rate multiplier
     */
    static double calculatePlaybackRate(int midiNote, int rootNote,
                                        double sampleRate, double targetSampleRate);
};
