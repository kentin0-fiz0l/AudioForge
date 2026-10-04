#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <array>
#include <bitset>
#include <cstdint>
#include <memory>
#include <vector>
#include "MusicTheory.h"
#include "AIClient.h"

// Turns a TrackStructure into MIDI and plays it back block by block.
//
// Generation and playback are deliberately separate and stateless:
//   - generateFromStructure() runs on the message thread and returns an
//     immutable Sequence.
//   - renderRange() runs on the audio thread and only reads a Sequence.
class MIDIGenerator {
public:
    // A single note-on or note-off, positioned in quarter notes (PPQ) from
    // the start of the track.
    struct Event {
        double time;        // Quarter notes from the start of the track
        int channel;        // MIDI channel (1-16)
        int noteNumber;     // MIDI note (0-127)
        int velocity;       // Velocity (1-127); unused for note-offs
        bool isNoteOn;
    };

    // A generated track. Events are sorted by time, with note-offs ahead of
    // note-ons that share a timestamp so a re-struck pitch is released first.
    struct Sequence {
        std::uint64_t id = 0;   // Unique per generated sequence
        std::vector<Event> events;
        int totalBars = 0;
        double bpm = 120.0;
    };

    // Tracks which notes are sounding so they can be released when the
    // transport stops or jumps, or when the sequence is replaced.
    // Fixed size and allocation-free, for use on the audio thread.
    class HeldNotes {
    public:
        void noteOn(int channel, int noteNumber);

        // Returns whether the note was sounding
        bool noteOff(int channel, int noteNumber);

        // Adds a note-off for every sounding note, then forgets them all
        void releaseAll(juce::MidiBuffer& buffer, int sampleOffset);

        bool isEmpty() const;

    private:
        std::array<std::bitset<128>, 16> held_;
    };

    // Generate a MIDI sequence from a track structure (message thread)
    static std::unique_ptr<const Sequence> generateFromStructure(const TrackStructure& structure);

    // Add every event with fromPpq <= time < toPpq to the buffer (audio thread).
    //
    // The block being rendered starts at blockStartPpq and is numSamples long;
    // each event lands on the sample closest to its time, clamped to the block.
    // fromPpq may be earlier than blockStartPpq to catch up on a skipped range.
    static void renderRange(const Sequence& sequence,
                            juce::MidiBuffer& buffer,
                            HeldNotes& heldNotes,
                            double fromPpq,
                            double toPpq,
                            double blockStartPpq,
                            double ppqPerSample,
                            int numSamples);

private:
    struct MIDINote {
        int channel;        // MIDI channel (1-16)
        int noteNumber;     // MIDI note (0-127)
        int velocity;       // Velocity (0-127)
        double startTime;   // Start time in quarter notes
        double duration;    // Duration in quarter notes
    };

    static void generateDrums(const TrackStructure& structure,
                              std::vector<MIDINote>& notes);

    static void generateBass(const TrackStructure& structure,
                             std::vector<MIDINote>& notes);

    static void generateChords(const TrackStructure& structure,
                               std::vector<MIDINote>& notes);

    static void generateLead(const TrackStructure& structure,
                             std::vector<MIDINote>& notes);
};
