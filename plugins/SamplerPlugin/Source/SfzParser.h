#pragma once

#include <juce_core/juce_core.h>
#include <vector>

/**
 * SFZ Parser
 *
 * Reads the subset of the SFZ format that sampled instrument libraries such
 * as VSCO 2 Community Edition use: the <control>, <global>, <master>, <group>
 * and <region> headers, with each region inheriting the opcodes of the
 * headers above it. Parsing is text only; the sampler loads the samples.
 *
 * Opcodes read: sample, default_path, lokey, hikey, key, pitch_keycenter,
 * pitch_keytrack, lovel, hivel, tune, transpose, volume, amplitude, pan,
 * amp_veltrack, ampeg_attack, ampeg_decay, ampeg_sustain, ampeg_release,
 * offset, end, loop_mode, loop_start, loop_end (and loopstart/loopend),
 * seq_length, seq_position, lorand, hirand, trigger, and the keyswitches
 * sw_lokey, sw_hikey, sw_last, sw_default. Keys can be numbers or note names
 * (c4 is 60, f#3 is 54, eb2 is 39). Anything else is ignored.
 */
struct SfzRegion
{
    enum class LoopMode
    {
        Unset,          // No loop_mode: loop if the sample file has loop points
        NoLoop,         // Play to the end of the sample, or until released
        OneShot,        // Play to the end of the sample whatever the note-off
        Continuous,     // Loop between loop_start and loop_end
        Sustain         // Loop while held (played as Continuous)
    };

    juce::String sample;            // As written, with default_path in front and '\' made '/'

    int lokey = 0;
    int hikey = 127;
    int keycenter = 60;
    float keytrack = 100.0f;        // Cents per key

    int lovel = 1;
    int hivel = 127;

    float tune = 0.0f;              // Cents
    int transpose = 0;              // Semitones
    float volume = 0.0f;            // dB
    float amplitude = 100.0f;       // Percent
    float pan = 0.0f;               // -100 (left) to 100 (right)
    float ampVeltrack = 100.0f;     // Percent

    // Envelope times in seconds and sustain in percent; negative means the
    // region leaves it to the plugin's own envelope
    float ampegAttack = -1.0f;
    float ampegDecay = -1.0f;
    float ampegSustain = -1.0f;
    float ampegRelease = -1.0f;

    juce::int64 offset = 0;         // Start, in sample frames
    juce::int64 end = -1;           // Last frame to play; -1 for the whole sample

    LoopMode loopMode = LoopMode::Unset;
    juce::int64 loopStart = -1;     // In sample frames; -1 for the sample file's own
    juce::int64 loopEnd = -1;

    int seqLength = 1;              // Round robin: this region plays on note
    int seqPosition = 1;            // seqPosition of every seqLength
    float lorand = 0.0f;            // Random round robin: plays when a random
    float hirand = 1.0f;            // number in [0, 1) falls in [lorand, hirand)

    bool releaseTrigger = false;    // trigger=release: sounds on note-off (not played)

    // Keyswitches: notes from swLokey to swHikey pick an articulation rather
    // than play, and a region with swLast plays only after that key was the
    // last one picked (swDefault before any is)
    int swLokey = -1;
    int swHikey = -1;
    int swLast = -1;
    int swDefault = -1;

    bool containsKey(int note) const { return note >= lokey && note <= hikey; }
    bool containsVelocity(int velocity) const { return velocity >= lovel && velocity <= hivel; }
};

struct SfzInstrument
{
    std::vector<SfzRegion> regions;
    juce::StringArray warnings;     // Opcodes it didn't understand, bad values
};

namespace Sfz
{
    /** Parses SFZ text. Sample paths stay as written (relative to the .sfz). */
    SfzInstrument parse(const juce::String& text);

    /**
     * A MIDI note from a number or a note name (c4 = 60); -2 if it's neither
     * (-1 is a number, SFZ's "no key").
     */
    int parseNote(const juce::String& text);

    /** Resolves a region's sample path against the folder the .sfz is in. */
    juce::File resolveSample(const juce::File& sfzFile, const juce::String& samplePath);
}
