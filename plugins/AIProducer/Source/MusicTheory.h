#pragma once

#include <string>
#include <vector>

namespace MusicTheory {

// General MIDI drum notes
namespace DrumNotes {
    constexpr int KICK = 36;
    constexpr int SNARE = 38;
    constexpr int CLAP = 39;
    constexpr int CLOSED_HAT = 42;
    constexpr int OPEN_HAT = 46;
    constexpr int CRASH = 49;
}

// Semitones from the root for each degree of the natural minor or major scale
const std::vector<int>& scaleIntervals(bool minor);

// The pitch of a scale degree. Degree 0 is `root`; degrees past 6 or below 0
// continue into the octaves above and below.
int degreeToPitch(int root, bool minor, int degree);

// Scale degrees (0-6) of a progression of Roman numerals, e.g. "i-VI-III-VII"
// gives {0, 5, 2, 6}. Case is ignored: each chord takes the quality the scale
// gives it. An unrecognised numeral counts as the tonic.
std::vector<int> parseProgression(const std::string& progression);

} // namespace MusicTheory
