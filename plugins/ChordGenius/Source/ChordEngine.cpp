/*
  ChordEngine.cpp - Implementation
*/

#include "ChordEngine.h"
#include <algorithm>

// Static member definitions
const std::vector<int> ChordEngine::MAJOR_SCALE = {0, 2, 4, 5, 7, 9, 11};
const std::vector<int> ChordEngine::MINOR_SCALE = {0, 2, 3, 5, 7, 8, 10};

const std::map<std::string, std::vector<std::string> > ChordEngine::PROGRESSIONS = {
    {"pop", {"I", "V", "vi", "IV"}},
    {"rock", {"I", "IV", "V", "I"}},
    {"jazz", {"ii", "V", "I", "vi"}},
    {"blues", {"I", "IV", "I", "V"}},
    {"edm", {"i", "VI", "III", "VII"}},
    {"emotional", {"vi", "IV", "I", "V"}},
    {"happy", {"I", "IV", "I", "V"}},
    {"sad", {"i", "VII", "VI", "VII"}}
};

ChordEngine::ChordEngine()
{
    // Initialize with C Major
    setKey(60, true);  // 60 = Middle C

    // Seed random number generator
    std::random_device rd;
    rng_ = std::mt19937(rd());
}

void ChordEngine::setKey(int root, bool major)
{
    currentKey_.root = root;
    currentKey_.isMajor = major;
    currentKey_.name = midiNoteToName(root) + (major ? " Major" : " Minor");
    currentKey_.scale = getScale(root, major);
}

std::vector<ChordEngine::Chord> ChordEngine::generateProgression(
    const std::string& genre, int numChords)
{
    std::vector<Chord> progression;

    // Get genre template (or default to pop)
    auto it = PROGRESSIONS.find(genre);
    std::vector<std::string> template_prog;

    if (it != PROGRESSIONS.end())
        template_prog = it->second;
    else
        template_prog = PROGRESSIONS.at("pop");

    // Generate chords based on template
    for (int i = 0; i < numChords && i < template_prog.size(); ++i)
    {
        std::string roman = template_prog[i];
        int degree = romanToScaleDegree(roman);

        // Determine if chord is major or minor based on roman numeral
        bool isUpperCase = !roman.empty() && std::isupper(roman[0]);
        bool chordIsMajor = isUpperCase;

        Chord chord = createChord(degree, chordIsMajor);
        chord.romanNumeral = roman;

        progression.push_back(chord);
    }

    return progression;
}

std::vector<ChordEngine::Chord> ChordEngine::suggestNextChords(
    const std::vector<Chord>& currentProgression)
{
    std::vector<Chord> suggestions;

    if (currentProgression.empty())
    {
        // Suggest starting chords (I, IV, vi)
        suggestions.push_back(createChord(0, true));   // I
        suggestions.push_back(createChord(3, true));   // IV
        suggestions.push_back(createChord(5, false));  // vi
    }
    else
    {
        // Get last chord
        // Suggest common progressions from that chord

        // Common next chords: I, IV, V, vi
        suggestions.push_back(createChord(0, currentKey_.isMajor));  // I/i
        suggestions.push_back(createChord(3, true));                 // IV
        suggestions.push_back(createChord(4, true));                 // V
        suggestions.push_back(createChord(5, !currentKey_.isMajor)); // vi/VI
    }

    return suggestions;
}

ChordEngine::Chord ChordEngine::createChord(int degree, bool major,
                                            const std::string& extension)
{
    Chord chord;

    // Get root note from scale degree
    int scaleNote = currentKey_.scale[degree % 7];
    chord.root = currentKey_.root + scaleNote;

    // Build triad
    chord.notes.push_back(chord.root);                      // Root
    chord.notes.push_back(chord.root + (major ? 4 : 3));   // Third
    chord.notes.push_back(chord.root + 7);                  // Fifth

    // Set quality and name
    chord.quality = major ? "major" : "minor";
    chord.name = midiNoteToName(chord.root) + (major ? "maj" : "m");

    // Roman numeral (will be set by caller)
    chord.romanNumeral = major ?
        std::to_string(degree + 1) :
        std::string(1, 'i' + degree);

    return chord;
}

std::string ChordEngine::midiNoteToName(int note)
{
    static const std::vector<std::string> noteNames = {
        "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
    };

    return noteNames[note % 12];
}

std::vector<int> ChordEngine::getScale(int root, bool major)
{
    const auto& intervals = major ? MAJOR_SCALE : MINOR_SCALE;
    std::vector<int> scale;

    for (int interval : intervals)
        scale.push_back(root + interval);

    return scale;
}

int ChordEngine::romanToScaleDegree(const std::string& roman)
{
    // Convert roman numeral to scale degree (0-6)
    std::string upper = roman;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

    if (upper.find("VII") != std::string::npos) return 6;
    if (upper.find("VI") != std::string::npos) return 5;
    if (upper.find("V") != std::string::npos) return 4;
    if (upper.find("IV") != std::string::npos) return 3;
    if (upper.find("III") != std::string::npos) return 2;
    if (upper.find("II") != std::string::npos) return 1;

    return 0;  // I
}
