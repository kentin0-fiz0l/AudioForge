#include "MusicTheory.h"
#include <map>
#include <sstream>

namespace MusicTheory {

std::vector<int> getScaleIntervals(Scale scale) {
    switch (scale) {
        case Scale::Major:
            return {0, 2, 4, 5, 7, 9, 11};
        case Scale::Minor:
            return {0, 2, 3, 5, 7, 8, 10};
        case Scale::Dorian:
            return {0, 2, 3, 5, 7, 9, 10};
        case Scale::Phrygian:
            return {0, 1, 3, 5, 7, 8, 10};
        case Scale::Lydian:
            return {0, 2, 4, 6, 7, 9, 11};
        case Scale::Mixolydian:
            return {0, 2, 4, 5, 7, 9, 10};
        case Scale::HarmonicMinor:
            return {0, 2, 3, 5, 7, 8, 11};
        case Scale::MelodicMinor:
            return {0, 2, 3, 5, 7, 9, 11};
        case Scale::Pentatonic:
            return {0, 2, 4, 7, 9};
        case Scale::Blues:
            return {0, 3, 5, 6, 7, 10};
        case Scale::Chromatic:
        default:
            return {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    }
}

std::vector<Note> getScaleNotes(Note root, Scale scale) {
    std::vector<int> intervals = getScaleIntervals(scale);
    std::vector<Note> notes;

    for (int interval : intervals) {
        notes.push_back(root + interval);
    }

    return notes;
}

void Chord::generateNotes() {
    notes.clear();
    notes.push_back(root);

    switch (type) {
        case ChordType::Major:
            notes.push_back(root + 4);  // Major 3rd
            notes.push_back(root + 7);  // Perfect 5th
            break;
        case ChordType::Minor:
            notes.push_back(root + 3);  // Minor 3rd
            notes.push_back(root + 7);  // Perfect 5th
            break;
        case ChordType::Diminished:
            notes.push_back(root + 3);  // Minor 3rd
            notes.push_back(root + 6);  // Diminished 5th
            break;
        case ChordType::Augmented:
            notes.push_back(root + 4);  // Major 3rd
            notes.push_back(root + 8);  // Augmented 5th
            break;
        case ChordType::Sus2:
            notes.push_back(root + 2);  // Major 2nd
            notes.push_back(root + 7);  // Perfect 5th
            break;
        case ChordType::Sus4:
            notes.push_back(root + 5);  // Perfect 4th
            notes.push_back(root + 7);  // Perfect 5th
            break;
        case ChordType::Major7:
            notes.push_back(root + 4);  // Major 3rd
            notes.push_back(root + 7);  // Perfect 5th
            notes.push_back(root + 11); // Major 7th
            break;
        case ChordType::Minor7:
            notes.push_back(root + 3);  // Minor 3rd
            notes.push_back(root + 7);  // Perfect 5th
            notes.push_back(root + 10); // Minor 7th
            break;
        case ChordType::Dom7:
            notes.push_back(root + 4);  // Major 3rd
            notes.push_back(root + 7);  // Perfect 5th
            notes.push_back(root + 10); // Minor 7th
            break;
        case ChordType::Dim7:
            notes.push_back(root + 3);  // Minor 3rd
            notes.push_back(root + 6);  // Diminished 5th
            notes.push_back(root + 9);  // Diminished 7th
            break;
        case ChordType::MajAdd9:
            notes.push_back(root + 4);  // Major 3rd
            notes.push_back(root + 7);  // Perfect 5th
            notes.push_back(root + 14); // Major 9th
            break;
        case ChordType::MinAdd9:
            notes.push_back(root + 3);  // Minor 3rd
            notes.push_back(root + 7);  // Perfect 5th
            notes.push_back(root + 14); // Major 9th
            break;
    }
}

std::vector<Chord> generateProgression(Note key, Scale scale, const std::string& pattern) {
    std::vector<Chord> progression;
    std::vector<Note> scaleNotes = getScaleNotes(key, scale);

    // Parse progression pattern (e.g., "I-V-vi-IV")
    std::istringstream iss(pattern);
    std::string token;

    while (std::getline(iss, token, '-')) {
        // Map Roman numerals to scale degrees
        bool isMinor = (token[0] >= 'a' && token[0] <= 'z');
        char romanNumeral = isMinor ? (token[0] - 32) : token[0];

        int degree = 0;
        if (romanNumeral == 'I') degree = 0;
        else if (romanNumeral == 'V' && token[1] == 'I') degree = 5; // VI
        else if (romanNumeral == 'V' && token[1] == 'I' && token[2] == 'I') degree = 6; // VII
        else if (romanNumeral == 'V') degree = 4; // V
        else if (romanNumeral == 'I' && token[1] == 'V') degree = 3; // IV
        else if (romanNumeral == 'I' && token[1] == 'I' && token[2] == 'I') degree = 2; // III
        else if (romanNumeral == 'I' && token[1] == 'I') degree = 1; // II

        Note root = scaleNotes[degree % scaleNotes.size()];
        ChordType type = isMinor ? ChordType::Minor : ChordType::Major;

        progression.emplace_back(root, type);
    }

    return progression;
}

// Drum Patterns Implementation
namespace DrumPatterns {

DrumPattern fourOnFloor() {
    DrumPattern pattern("Four on the Floor");
    pattern.kickSteps = {0, 4, 8, 12};           // Every beat
    pattern.snareSteps = {4, 12};                // 2 and 4
    pattern.hatSteps = {0, 2, 4, 6, 8, 10, 12, 14}; // 8ths
    pattern.openHatSteps = {6, 14};              // Occasional open hats
    return pattern;
}

DrumPattern houseClassic() {
    DrumPattern pattern("House Classic");
    pattern.kickSteps = {0, 4, 8, 12};           // Four on floor
    pattern.snareSteps = {4, 12};                // Claps on 2 and 4
    pattern.hatSteps = {0, 2, 4, 6, 8, 10, 12, 14}; // Closed hats on 8ths
    pattern.openHatSteps = {3, 7, 11, 15};       // Open hats on off-beats
    return pattern;
}

DrumPattern technoHypnotic() {
    DrumPattern pattern("Techno Hypnotic");
    pattern.kickSteps = {0, 6, 8, 14};           // Syncopated kick
    pattern.snareSteps = {4, 12};                // Snare on 2 and 4
    pattern.hatSteps = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}; // 16ths
    pattern.openHatSteps = {};                    // No open hats
    return pattern;
}

DrumPattern dnbBasic() {
    DrumPattern pattern("DnB Basic");
    pattern.kickSteps = {0, 6};                  // Kick on 1 and syncopated
    pattern.snareSteps = {4, 10, 12};            // Amen break style
    pattern.hatSteps = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}; // 16ths
    pattern.openHatSteps = {};
    return pattern;
}

DrumPattern trapPattern() {
    DrumPattern pattern("Trap");
    pattern.kickSteps = {0, 6, 12};              // Syncopated kick
    pattern.snareSteps = {4, 12};                // Snare on 2 and 4
    pattern.hatSteps = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}; // Rapid 16ths
    pattern.openHatSteps = {2, 6, 10, 14};       // Open hat accents
    return pattern;
}

DrumPattern hipHopBoom() {
    DrumPattern pattern("Hip-Hop Boom Bap");
    pattern.kickSteps = {0, 6, 10};              // Classic boom bap
    pattern.snareSteps = {4, 12};                // Snare on 2 and 4
    pattern.hatSteps = {0, 2, 4, 6, 8, 10, 12, 14}; // 8th note hats
    pattern.openHatSteps = {};
    return pattern;
}

} // namespace DrumPatterns

} // namespace MusicTheory
