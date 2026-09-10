#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>

class ChordEngine {
public:
    enum class ChordType {
        Major = 0,
        Minor,
        Diminished,
        Augmented,
        Sus2,
        Sus4,
        Major7,
        Minor7,
        Dominant7,
        MinorMaj7,
        Diminished7,
        HalfDiminished7,
        Major6,
        Minor6,
        Add9,
        Major9,
        Minor9,
        Dominant9
    };

    enum class Voicing {
        Root = 0,           // Root position
        First,              // 1st inversion
        Second,             // 2nd inversion
        Third,              // 3rd inversion (7th chords only)
        Spread,             // Wide voicing
        Drop2,              // Drop-2 voicing (jazz)
        Drop3               // Drop-3 voicing (jazz)
    };

    ChordEngine();

    // Process incoming MIDI
    void processNoteOn(int noteNumber, int velocity, int channel,
                       juce::MidiBuffer& output, int samplePosition);
    void processNoteOff(int noteNumber, int channel,
                        juce::MidiBuffer& output, int samplePosition);

    // Parameter setters
    void setChordType(ChordType type) { chordType_ = type; }
    void setVoicing(Voicing voicing) { voicing_ = voicing; }
    void setOctaveShift(int octaves) { octaveShift_ = juce::jlimit(-2, 2, octaves); }
    void setVelocityMode(bool useInput) { useInputVelocity_ = useInput; }
    void setFixedVelocity(int vel) { fixedVelocity_ = juce::jlimit(1, 127, vel); }
    void setStrum(float strum) { strumDelay_ = juce::jlimit(0.0f, 100.0f, strum); }

private:
    // Active notes tracking
    struct ActiveNote {
        int rootNote;
        std::vector<int> chordNotes;
        int channel;
        int velocity;
    };

    std::vector<ActiveNote> activeNotes_;

    // Parameters
    ChordType chordType_ = ChordType::Major;
    Voicing voicing_ = Voicing::Root;
    int octaveShift_ = 0;
    bool useInputVelocity_ = true;
    int fixedVelocity_ = 100;
    float strumDelay_ = 0.0f; // Milliseconds between notes (guitar strum effect)

    // Chord generation
    std::vector<int> generateChord(int rootNote);
    std::vector<int> getChordIntervals(ChordType type);
    void applyVoicing(std::vector<int>& notes, Voicing voicing);
    int getVelocityForNote(int inputVelocity) const;
};
