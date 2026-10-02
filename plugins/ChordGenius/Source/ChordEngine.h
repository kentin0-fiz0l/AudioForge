/*
  ChordEngine.h - Music Theory Engine for ChordGenius
  Generates chord progressions, suggests chords, analyzes keys
*/

#pragma once
#include <vector>
#include <string>
#include <random>
#include <map>

class ChordEngine
{
public:
    // Chord representation
    struct Chord
    {
        std::string name;           // e.g. "Cmaj", "Dm7"
        std::string romanNumeral;   // e.g. "I", "ii", "V7"
        std::vector<int> notes;     // MIDI notes (e.g. [60, 64, 67] for C major)
        int root;                   // Root MIDI note
        std::string quality;        // "major", "minor", "dom7", etc.
    };

    // Key/Scale information
    struct Key
    {
        int root;                   // Root MIDI note
        std::string name;           // e.g. "C Major"
        std::vector<int> scale;     // Scale degrees
        bool isMajor;
    };

    ChordEngine();

    // Main functions
    std::vector<Chord> generateProgression(const std::string& genre, int numChords = 4);
    std::vector<Chord> suggestNextChords(const std::vector<Chord>& currentProgression);
    void setKey(int root, bool major = true);
    Key getCurrentKey() const { return currentKey_; }

    // Utility functions
    static std::string midiNoteToName(int note);
    static std::vector<int> getScale(int root, bool major);

private:
    Key currentKey_;
    std::mt19937 rng_;

    // Music theory data (defined in cpp)
    static const std::map<std::string, std::vector<std::string> > PROGRESSIONS;
    static const std::vector<int> MAJOR_SCALE;
    static const std::vector<int> MINOR_SCALE;

    // Helper functions
    Chord createChord(int degree, bool major, const std::string& extension = "");
    std::vector<std::string> getRomanNumerals(bool major);
    int romanToScaleDegree(const std::string& roman);
};
