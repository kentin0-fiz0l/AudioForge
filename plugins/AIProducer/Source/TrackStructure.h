#pragma once

#include <string>
#include <vector>

// The plan for a track: style, key, harmony and arrangement. MIDIGenerator
// turns a plan into notes. Today the plan comes from the plugin's own controls
// (Arranger::build); the same shape can later be sent in from outside, for
// example by the AI Producer Remote Script.
struct TrackStructure {
    std::string genre = "House";   // A name from Styles::genreNames()
    int bpm = 124;                 // The tempo the track is written for
    int keyRoot = 9;               // Pitch class of the key: 0 = C ... 11 = B
    bool minor = true;
    std::string progression = "i-VI-III-VII"; // Roman numerals, one chord per bar
    int seed = 1;                  // Picks the variation (melody, fills)

    struct Section {
        std::string type;          // "intro", "build", "drop", "breakdown" or "outro"
        int startBar;              // From 0
        int lengthBars;
        int energy;                // 1-10
    };
    std::vector<Section> sections;

    bool hasDrums = true;
    bool hasBass = true;
    bool hasChords = true;
    bool hasLead = true;
};
