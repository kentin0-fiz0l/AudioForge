#pragma once

#include "SfzParser.h"
#include "SampleLibrary.h"
#include "SamplerVoice.h"
#include <vector>

/**
 * SFZ Sample Map
 *
 * An SFZ instrument with its samples loaded: decides which regions a note
 * plays (key and velocity ranges, round robin, keyswitches) and what each
 * plays them with (pitch, level, loop, envelope).
 */
class SfzSampleMap
{
public:
    struct Region
    {
        SfzRegion opcodes;
        std::shared_ptr<Sample> sample;
        int seqCounter = 0;         // Times it could have played, for round robin
    };

    /**
     * Read an .sfz file and load every sample it names. Regions whose sample
     * can't be loaded are left out and noted in getWarnings(); it fails only
     * when the file can't be read or no region has a sample.
     */
    bool load(const juce::File& sfzFile, juce::String& error);

    const juce::File& getFile() const { return file; }
    const std::vector<Region>& getRegions() const { return regions; }
    const juce::StringArray& getWarnings() const { return warnings; }
    bool isEmpty() const { return regions.empty(); }

    /** Whether a note picks an articulation rather than plays. */
    bool isKeyswitch(int note) const { return note >= keyswitchLow && note <= keyswitchHigh; }
    int getCurrentKeyswitch() const { return currentKeyswitch; }

    /**
     * The regions a note-on plays, with what to play each with. Moves the
     * round robin on, and takes a keyswitch note as the new articulation
     * (it plays nothing).
     *
     * @param note MIDI note
     * @param velocity MIDI velocity, 1-127
     * @param random A number in [0, 1) for lorand/hirand
     * @param semitones Extra tuning (the plugin's Master Tune)
     */
    std::vector<SamplerVoice::Playback> noteOn(int note, int velocity, float random, float semitones);

    /** What a region plays a note with. */
    static SamplerVoice::Playback playbackFor(const Region& region, int note, int velocity, float semitones);

private:
    juce::File file;
    std::vector<Region> regions;
    juce::StringArray warnings;
    SampleLibrary library;

    int keyswitchLow = 128;
    int keyswitchHigh = -1;
    int currentKeyswitch = -1;
};
