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
    /**
     * What a voice plays and how: the sample, its pitch and level, which part
     * of it, and any envelope settings of its own (an SFZ region's).
     */
    struct Playback
    {
        std::shared_ptr<Sample> sample;
        double pitchRatio = 1.0;        // Note frequency over the sample's recorded pitch
        float gainLeft = 1.0f;          // Level, velocity and pan together
        float gainRight = 1.0f;
        int startFrame = 0;
        int endFrame = -1;              // Last frame to play; -1 for the whole sample
        Sample::LoopMode loopMode = Sample::LoopMode::None;
        int loopStart = 0;              // Frames; loopEnd is the last frame of the loop
        int loopEnd = 0;
        bool oneShot = false;           // Ignores note-off and plays to the end

        // Negative: use the plugin's Amp envelope parameter
        float attack = -1.0f;
        float decay = -1.0f;
        float sustain = -1.0f;
        float release = -1.0f;
    };

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
     * Start playing a note with what to play.
     */
    void start(int midiNote, const Playback& playback, double sampleRate);

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
     * Whether the note has been let go and the voice is in its release.
     */
    bool isReleased() const { return released; }

    /**
     * Get current MIDI note (VoiceManager interface).
     */
    int getMidiNote() const { return currentNote; }

    /**
     * Get current level for voice stealing (VoiceManager interface).
     */
    float getLevel() const;

    /**
     * Render one sample of audio, adding it into left and right.
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
     */
    void processSample(double sampleRate,
                       float attack, float decay, float sustain, float release,
                       float filterCutoff, float filterResonance, int filterType,
                       float filterEnvAmount,
                       float filtAttack, float filtDecay, float filtSustain, float filtRelease,
                       float& left, float& right);

private:
    bool active = false;
    bool released = false;
    int currentNote = -1;
    float noteVelocity = 0.0f;

    Playback playback;

    double playbackPosition = 0.0;  // Current position in sample (fractional)
    double playbackRate = 1.0;      // Pitch multiplier
    int loopDirection = 1;          // 1=forward, -1=reverse (for pingpong)

    ADSREnvelope ampEnvelope;
    ADSREnvelope filterEnvelope;
    MultiModeFilter filterLeft;
    MultiModeFilter filterRight;

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
};
