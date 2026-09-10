#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>
#include <array>

class HarmonizerEngine {
public:
    enum class Scale {
        Chromatic = 0,  // No scale (all semitones available)
        Major,
        Minor,
        Dorian,
        Phrygian,
        Lydian,
        Mixolydian,
        Aeolian,
        Locrian,
        HarmonicMinor,
        MelodicMinor,
        Pentatonic,
        Blues
    };

    enum class Key {
        C = 0, Db, D, Eb, E, F, Gb, G, Ab, A, Bb, B
    };

    HarmonizerEngine();

    // Process incoming MIDI
    void processNoteOn(int noteNumber, int velocity, int channel,
                       juce::MidiBuffer& output, int samplePosition);
    void processNoteOff(int noteNumber, int channel,
                        juce::MidiBuffer& output, int samplePosition);

    // Parameter setters
    void setKey(Key key) { key_ = key; }
    void setScale(Scale scale) { scale_ = scale; }
    void setThirdAbove(bool enabled) { thirdAbove_ = enabled; }
    void setFifthAbove(bool enabled) { fifthAbove_ = enabled; }
    void setOctaveAbove(bool enabled) { octaveAbove_ = enabled; }
    void setThirdBelow(bool enabled) { thirdBelow_ = enabled; }
    void setFifthBelow(bool enabled) { fifthBelow_ = enabled; }
    void setOctaveBelow(bool enabled) { octaveBelow_ = enabled; }
    void setMix(float mix) { mix_ = juce::jlimit(0.0f, 1.0f, mix); }
    void setVelocityMode(bool useInput) { useInputVelocity_ = useInput; }
    void setFixedVelocity(int vel) { fixedVelocity_ = juce::jlimit(1, 127, vel); }

private:
    // Active notes tracking
    struct ActiveNote {
        int originalNote;
        std::vector<int> harmonyNotes;
        int channel;
        int velocity;
    };

    std::vector<ActiveNote> activeNotes_;

    // Parameters
    Key key_ = Key::C;
    Scale scale_ = Scale::Major;
    bool thirdAbove_ = true;
    bool fifthAbove_ = false;
    bool octaveAbove_ = false;
    bool thirdBelow_ = false;
    bool fifthBelow_ = false;
    bool octaveBelow_ = false;
    float mix_ = 1.0f;  // 0 = dry only, 1 = dry + wet
    bool useInputVelocity_ = true;
    int fixedVelocity_ = 100;

    // Scale generation
    std::vector<int> generateHarmonyNotes(int rootNote);
    int getScaleDegree(int noteNumber);
    int transposeInScale(int noteNumber, int degrees);
    std::array<int, 12> getScalePattern(Scale scale);
    int getVelocityForNote(int inputVelocity) const;
};
