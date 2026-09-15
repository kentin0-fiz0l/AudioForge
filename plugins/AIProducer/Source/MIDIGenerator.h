#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "MusicTheory.h"
#include "AIClient.h"

class MIDIGenerator {
public:
    MIDIGenerator();
    ~MIDIGenerator() = default;

    // Generate MIDI sequence from track structure
    void generateFromStructure(const TrackStructure& structure);

    // Get generated MIDI buffer
    // Returns MIDI events for a specific time range
    void fillMIDIBuffer(juce::MidiBuffer& buffer,
                       int startSample,
                       int numSamples,
                       double sampleRate,
                       double bpm);

    // Reset playback position
    void reset();

    // Check if generation is complete
    bool isReady() const { return generatedSequence_ != nullptr; }

    // Get total length in bars
    int getTotalBars() const { return totalBars_; }

private:
    struct MIDINote {
        int channel;        // MIDI channel (0-15)
        int noteNumber;     // MIDI note (0-127)
        int velocity;       // Velocity (0-127)
        double startTime;   // Start time in quarter notes
        double duration;    // Duration in quarter notes
    };

    struct GeneratedSequence {
        std::vector<MIDINote> notes;
        int totalBars;
        double bpm;
    };

    void generateDrums(const TrackStructure& structure,
                      std::vector<MIDINote>& notes);

    void generateBass(const TrackStructure& structure,
                     std::vector<MIDINote>& notes);

    void generateChords(const TrackStructure& structure,
                       std::vector<MIDINote>& notes);

    void generateLead(const TrackStructure& structure,
                     std::vector<MIDINote>& notes);

    double currentPlaybackPosition_ = 0.0; // In quarter notes
    std::unique_ptr<GeneratedSequence> generatedSequence_;
    int totalBars_ = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDIGenerator)
};
