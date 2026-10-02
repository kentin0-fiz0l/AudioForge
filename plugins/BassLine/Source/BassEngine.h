/*
  BassEngine.h - Bass Line Generation Engine
*/

#pragma once
#include <vector>
#include <string>
#include <random>
#include <map>

class BassEngine
{
public:
    struct Note {
        int pitch;          // MIDI note number
        float startTime;    // Beat position
        float duration;     // Note length in beats
        int velocity;       // MIDI velocity
        bool isRoot;        // Is this the chord root?
    };

    struct BassParams {
        std::string style;       // Rock, Funk, Jazz, EDM, etc.
        int complexity;          // 1-10, affects pattern density
        int octave;              // 1, 2, or 3 (C1, C2, C3)
        bool usePassingTones;    // Add chromatic passing tones
        bool useFifths;          // Include perfect fifths
        bool useOctaves;         // Octave jumps
    };

    struct ChordInfo {
        int root;           // Root note (0-11, C=0)
        bool isMajor;       // Major or minor
        float startBeat;    // When chord starts
        float duration;     // Chord duration in beats
    };

    BassEngine();

    // Generate bass line from chord progression
    std::vector<Note> generateBassLine(
        const std::vector<ChordInfo>& chords,
        const BassParams& params
    );

    // Generate bass line in a key (without chord progression)
    std::vector<Note> generateBassLineInKey(
        int keyRoot,
        bool keyIsMajor,
        const BassParams& params,
        int numBars = 4
    );

private:
    std::mt19937 rng_;

    // Style-specific generators
    std::vector<Note> generateRockBass(const std::vector<ChordInfo>& chords, const BassParams& params);
    std::vector<Note> generateFunkBass(const std::vector<ChordInfo>& chords, const BassParams& params);
    std::vector<Note> generateJazzBass(const std::vector<ChordInfo>& chords, const BassParams& params);
    std::vector<Note> generateEDMBass(const std::vector<ChordInfo>& chords, const BassParams& params);
    std::vector<Note> generateReggaeBass(const std::vector<ChordInfo>& chords, const BassParams& params);
    std::vector<Note> generateLatinBass(const std::vector<ChordInfo>& chords, const BassParams& params);
    std::vector<Note> generateBluesBass(const std::vector<ChordInfo>& chords, const BassParams& params);

    // Helper functions
    int midiNoteFromRoot(int root, int octave);
    int getFifth(int root);
    int getChordTone(int root, bool isMajor, int degree);
    std::vector<int> getPassingTones(int fromPitch, int toPitch);
    void addHumanization(std::vector<Note>& notes);

    // Scales
    static const std::vector<int> MAJOR_SCALE;
    static const std::vector<int> MINOR_SCALE;
};
