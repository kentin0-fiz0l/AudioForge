#pragma once

#include <vector>
#include <string>
#include <map>

namespace MusicTheory {

// Note representation (MIDI note number)
using Note = int;

// Musical scales (semitone intervals from root)
enum class Scale {
    Major,
    Minor,
    Dorian,
    Phrygian,
    Lydian,
    Mixolydian,
    HarmonicMinor,
    MelodicMinor,
    Pentatonic,
    Blues,
    Chromatic
};

// Chord types
enum class ChordType {
    Major,      // 1-3-5
    Minor,      // 1-b3-5
    Diminished, // 1-b3-b5
    Augmented,  // 1-3-#5
    Sus2,       // 1-2-5
    Sus4,       // 1-4-5
    Major7,     // 1-3-5-7
    Minor7,     // 1-b3-5-b7
    Dom7,       // 1-3-5-b7
    Dim7,       // 1-b3-b5-bb7
    MajAdd9,    // 1-3-5-9
    MinAdd9     // 1-b3-5-9
};

// Chord structure
struct Chord {
    Note root;
    ChordType type;
    std::vector<Note> notes;

    Chord(Note r, ChordType t) : root(r), type(t) {
        generateNotes();
    }

    void generateNotes();
};

// Get scale intervals
std::vector<int> getScaleIntervals(Scale scale);

// Get notes in a scale
std::vector<Note> getScaleNotes(Note root, Scale scale);

// Generate chord progression for a key
std::vector<Chord> generateProgression(Note key, Scale scale, const std::string& pattern);

// Common chord progressions
namespace Progressions {
    // Electronic/Dance
    const std::string HOUSE_CLASSIC = "I-V-vi-IV";
    const std::string TECHNO_DARK = "i-VI-III-VII";
    const std::string TRANCE_UPLIFTING = "I-V-vi-iii-IV";

    // Pop/Rock
    const std::string POP_CLASSIC = "I-V-vi-IV";
    const std::string POP_EMOTIONAL = "vi-IV-I-V";

    // Hip-Hop
    const std::string HIPHOP_SIMPLE = "i-i-i-i";
    const std::string HIPHOP_MELODIC = "i-VII-VI-V";
}

// Drum patterns (MIDI note numbers for GM drum map)
namespace DrumNotes {
    const Note KICK = 36;
    const Note SNARE = 38;
    const Note CLAP = 39;
    const Note CLOSED_HAT = 42;
    const Note OPEN_HAT = 46;
    const Note CRASH = 49;
    const Note RIDE = 51;
}

// Genre-specific drum patterns (16th note grid, 0-15 steps per bar)
struct DrumPattern {
    std::string name;
    std::vector<int> kickSteps;      // Which 16th notes have kicks
    std::vector<int> snareSteps;     // Which 16th notes have snares
    std::vector<int> hatSteps;       // Which 16th notes have hats
    std::vector<int> openHatSteps;   // Which 16th notes have open hats

    DrumPattern(const std::string& n) : name(n) {}
};

// Genre-specific patterns
namespace DrumPatterns {
    DrumPattern fourOnFloor();
    DrumPattern houseClassic();
    DrumPattern technoHypnotic();
    DrumPattern dnbBasic();
    DrumPattern trapPattern();
    DrumPattern hipHopBoom();
}

// BPM ranges for genres
namespace BPM {
    const int HOUSE_MIN = 120;
    const int HOUSE_MAX = 130;
    const int TECHNO_MIN = 125;
    const int TECHNO_MAX = 135;
    const int TRANCE_MIN = 130;
    const int TRANCE_MAX = 140;
    const int DNB_MIN = 170;
    const int DNB_MAX = 180;
    const int HIPHOP_MIN = 80;
    const int HIPHOP_MAX = 100;
    const int TRAP_MIN = 130;
    const int TRAP_MAX = 150;
}

} // namespace MusicTheory
