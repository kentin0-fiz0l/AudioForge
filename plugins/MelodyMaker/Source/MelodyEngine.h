/*
  MelodyEngine.h - AI Melody Generator
  Generates melodies over chord progressions using music theory rules
*/

#pragma once
#include <vector>
#include <string>
#include <map>
#include <random>

class MelodyEngine
{
public:
    struct Note {
        int pitch;          // MIDI note number
        float startTime;    // Beat position (0.0 = downbeat)
        float duration;     // Note length in beats
        int velocity;       // MIDI velocity (0-127)
        bool isChordTone;   // True if note is in the underlying chord
    };

    struct MelodyParams {
        std::string style;       // Pop, Rock, Jazz, Blues, EDM, Ballad, Funk, Classical
        int noteDensity;         // 1-10 (sparse to dense)
        int minPitch;            // Lowest MIDI note
        int maxPitch;            // Highest MIDI note
        int rhythmComplexity;    // 1-10 (simple to complex)
        bool useChordTones;      // Prefer chord tones
        bool useSyncopation;     // Add rhythmic syncopation
    };

    MelodyEngine();
    ~MelodyEngine() = default;

    // Generate melody over a chord progression
    std::vector<Note> generateMelody(
        const std::vector<int>& chordRoots,      // Root notes of chords
        const std::vector<std::string>& chordQualities,  // "major", "minor", "7", etc.
        const MelodyParams& params,
        int numBars = 4
    );

    // Generate melody in a specific key/scale
    std::vector<Note> generateMelodyInScale(
        int rootNote,
        bool isMajor,
        const MelodyParams& params,
        int numBars = 4
    );

    // Humanize a melody (add slight timing/velocity variations)
    void humanize(std::vector<Note>& melody, float amount = 0.5f);

private:
    // Music theory helpers
    std::vector<int> getScaleNotes(int root, bool isMajor);
    std::vector<int> getChordTones(int root, const std::string& quality);
    bool isInScale(int note, const std::vector<int>& scale);
    int findNearestScaleNote(int note, const std::vector<int>& scale);

    // Melody generation strategies
    std::vector<Note> generatePopMelody(const std::vector<int>& scale, int numBars);
    std::vector<Note> generateJazzMelody(const std::vector<int>& scale, int numBars);
    std::vector<Note> generateEDMMelody(const std::vector<int>& scale, int numBars);
    std::vector<Note> generateBalladMelody(const std::vector<int>& scale, int numBars);

    // Rhythm generation
    std::vector<float> generateRhythm(int numBars, int complexity, bool syncopated);

    // Contour shaping
    void applyContour(std::vector<Note>& melody, const std::string& shape);
    void constrainToRange(std::vector<Note>& melody, int minPitch, int maxPitch);

    // Random number generation
    std::mt19937 rng_;
    int getRandomInt(int min, int max);
    float getRandomFloat(float min, float max);

    // Style templates
    static const std::map<std::string, std::vector<float> > RHYTHM_PATTERNS;
    static const std::vector<int> MAJOR_SCALE;
    static const std::vector<int> MINOR_SCALE;
    static const std::vector<int> BLUES_SCALE;
};
