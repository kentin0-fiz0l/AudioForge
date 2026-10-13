#pragma once

#include "TrackStructure.h"
#include <string>
#include <vector>

// What each genre sounds like, and how the plugin's controls become a plan.
namespace Styles {

// Steps on a 16th-note grid, 0-15 per bar
struct Genre {
    std::string name;
    int bpm;
    std::vector<int> kick, backbeat, closedHat, openHat;
    int backbeatNote;              // GM drum note: 38 snare or 39 clap
    std::vector<int> bass;         // When the bass plays
    double bassLength;             // In 16ths
    std::vector<int> lead;         // When the lead can play
};

const std::vector<Genre>& genres();
const Genre& genre(const std::string& name);   // Falls back to the first genre
std::vector<std::string> genreNames();

// The settings the plugin's controls describe
struct Settings {
    int genre = 0;                 // Index into genres()
    int keyRoot = 9;               // 0 = C ... 11 = B
    bool minor = true;
    int length = 2;                // Index into lengthNames()
    bool drums = true, bass = true, chords = true, lead = true;
    int part = 0;                  // Index into partNames(): 0 plays every part
    int variation = 1;

    bool operator==(const Settings& other) const;
    bool operator!=(const Settings& other) const { return !(*this == other); }
};

std::vector<std::string> lengthNames();

// "All", then one name per part. A host that cannot split MIDI by channel
// (Ableton Live) runs one copy per part with the same settings: the same
// settings always write the same notes, so the copies play together.
std::vector<std::string> partNames();
std::vector<std::string> keyNames();

// Builds the plan for a set of controls: the genre's tempo, a chord
// progression chosen by the variation, and the arrangement for the length
TrackStructure build(const Settings& settings);

// "House in A minor, 72 bars at 124 BPM"
std::string describe(const TrackStructure& structure, int totalBars);

} // namespace Styles
