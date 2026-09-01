#pragma once

#include <juce_audio_formats/juce_audio_formats.h>
#include <memory>

/**
 * Sample
 *
 * Encapsulates a single audio sample with metadata and loop settings.
 * Loads audio data from AudioFormatReader and stores it in memory.
 */
class Sample
{
public:
    enum class LoopMode
    {
        None,       // No looping (one-shot playback)
        Forward,    // Loop from start to end repeatedly
        PingPong    // Loop forward then backward
    };

    /**
     * Construct a sample from an audio file reader.
     *
     * @param reader AudioFormatReader containing the sample data
     * @param maxLengthSecs Maximum sample length in seconds (default: 30.0)
     */
    Sample(juce::AudioFormatReader& reader, double maxLengthSecs = 30.0);

    /**
     * Get the audio data buffer.
     */
    const juce::AudioBuffer<float>& getAudioData() const { return audioData; }

    /**
     * Get the sample rate of the original audio.
     */
    double getSampleRate() const { return sampleRate; }

    /**
     * Get the length in samples.
     */
    int getLengthInSamples() const { return lengthInSamples; }

    /**
     * Get the number of channels (1=mono, 2=stereo).
     */
    int getNumChannels() const { return numChannels; }

    /**
     * Get the size in bytes (for memory budget tracking).
     */
    size_t getSizeInBytes() const;

    /**
     * Set loop points in seconds.
     *
     * @param startSec Loop start time in seconds
     * @param endSec Loop end time in seconds
     */
    void setLoopPoints(double startSec, double endSec);

    /**
     * Get loop start in samples.
     */
    int getLoopStartSamples() const { return loopStartSamples; }

    /**
     * Get loop end in samples.
     */
    int getLoopEndSamples() const { return loopEndSamples; }

    /**
     * Set loop mode.
     */
    void setLoopMode(LoopMode mode) { loopMode = mode; }

    /**
     * Get loop mode.
     */
    LoopMode getLoopMode() const { return loopMode; }

    /**
     * Set the root note (MIDI note number for original pitch).
     * Default is 60 (middle C).
     */
    void setRootNote(int midiNote) { rootNote = midiNote; }

    /**
     * Get the root note.
     */
    int getRootNote() const { return rootNote; }

private:
    juce::AudioBuffer<float> audioData;
    double sampleRate = 44100.0;
    int lengthInSamples = 0;
    int numChannels = 2;

    // Loop settings
    int loopStartSamples = 0;
    int loopEndSamples = 0;
    LoopMode loopMode = LoopMode::None;

    // Musical settings
    int rootNote = 60;  // Middle C by default
};
