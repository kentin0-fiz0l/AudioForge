#include "Styles.h"
#include "MusicTheory.h"
#include <algorithm>

namespace Styles {

namespace {

const std::vector<int> kFourOnTheFloor = {0, 4, 8, 12};
const std::vector<int> kOffbeats = {2, 6, 10, 14};
const std::vector<int> kEighths = {0, 2, 4, 6, 8, 10, 12, 14};
const std::vector<int> kSixteenthsBetweenOffbeats = {0, 1, 3, 4, 5, 7, 8, 9, 11, 12, 13, 15};
const std::vector<int> kAllSixteenths = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

constexpr int kSnare = MusicTheory::DrumNotes::SNARE;
constexpr int kClap = MusicTheory::DrumNotes::CLAP;

// Four chords each, one per bar; the variation picks one
const std::vector<std::string> kMinorProgressions = {"i-VI-III-VII", "i-VII-VI-VII", "i-iv-VI-VII", "i-VI-iv-VII"};
const std::vector<std::string> kMajorProgressions = {"I-V-vi-IV", "vi-IV-I-V", "I-IV-vi-V", "IV-I-V-vi"};

} // namespace

const std::vector<Genre>& genres() {
    static const std::vector<Genre> all = {
        {"House", 124, kFourOnTheFloor, {4, 12}, kSixteenthsBetweenOffbeats, kOffbeats, kClap,
         kOffbeats, 1.5, {0, 3, 6, 8, 10, 12}},
        {"Techno", 130, kFourOnTheFloor, {4, 12}, kAllSixteenths, kOffbeats, kClap,
         {2, 3, 6, 7, 10, 11, 14, 15}, 0.8, {0, 3, 6, 10}},
        {"Trance", 138, kFourOnTheFloor, {4, 12}, kSixteenthsBetweenOffbeats, kOffbeats, kClap,
         {1, 2, 3, 5, 6, 7, 9, 10, 11, 13, 14, 15}, 0.8, {0, 2, 3, 6, 8, 10, 11, 14}},
        {"Hip-Hop", 90, {0, 7, 10}, {4, 12}, kEighths, {}, kSnare,
         {0, 7, 10}, 3.0, {0, 4, 6, 10, 12}},
        {"Trap", 140, {0, 7, 10}, {8}, kAllSixteenths, {}, kClap,
         {0, 7, 10}, 5.0, {0, 3, 6, 8, 12}},
        {"Drum & Bass", 174, {0, 10}, {4, 12}, kEighths, {}, kSnare,
         {0, 10}, 4.0, {0, 3, 6, 8, 12, 14}},
    };
    return all;
}

const Genre& genre(const std::string& name) {
    for (const auto& g : genres())
        if (g.name == name) return g;
    return genres().front();
}

std::vector<std::string> genreNames() {
    std::vector<std::string> names;
    for (const auto& g : genres()) names.push_back(g.name);
    return names;
}

std::vector<std::string> lengthNames() {
    return {"8-bar loop", "32 bars", "Full song (72 bars)"};
}

std::vector<std::string> partNames() {
    return {"All", "Drums", "Bass", "Chords", "Lead"};
}

std::vector<std::string> keyNames() {
    return {"C", "C#", "D", "Eb", "E", "F", "F#", "G", "Ab", "A", "Bb", "B"};
}

bool Settings::operator==(const Settings& o) const {
    return genre == o.genre && keyRoot == o.keyRoot && minor == o.minor && length == o.length
           && drums == o.drums && bass == o.bass && chords == o.chords && lead == o.lead
           && variation == o.variation && part == o.part;
}

TrackStructure build(const Settings& settings) {
    const auto& all = genres();
    const auto& style = all[static_cast<size_t>(std::clamp(settings.genre, 0, static_cast<int>(all.size()) - 1))];

    TrackStructure structure;
    structure.genre = style.name;
    structure.bpm = style.bpm;
    structure.keyRoot = std::clamp(settings.keyRoot, 0, 11);
    structure.minor = settings.minor;
    structure.seed = std::max(1, settings.variation);

    const auto& progressions = settings.minor ? kMinorProgressions : kMajorProgressions;
    structure.progression = progressions[static_cast<size_t>(structure.seed - 1) % progressions.size()];

    switch (settings.length) {
        case 0:
            structure.sections = {{"drop", 0, 8, 10}};
            break;
        case 1:
            structure.sections = {{"intro", 0, 8, 3}, {"build", 8, 8, 6}, {"drop", 16, 16, 10}};
            break;
        default:
            structure.sections = {{"intro", 0, 8, 3},      {"build", 8, 8, 6}, {"drop", 16, 16, 10},
                                  {"breakdown", 32, 8, 4}, {"build", 40, 8, 7}, {"drop", 48, 16, 10},
                                  {"outro", 64, 8, 2}};
            break;
    }

    auto plays = [&](bool on, int part) { return on && (settings.part == 0 || settings.part == part); };
    structure.hasDrums = plays(settings.drums, 1);
    structure.hasBass = plays(settings.bass, 2);
    structure.hasChords = plays(settings.chords, 3);
    structure.hasLead = plays(settings.lead, 4);
    return structure;
}

std::string describe(const TrackStructure& structure, int totalBars) {
    const auto keys = keyNames();
    return structure.genre + " in " + keys[static_cast<size_t>(std::clamp(structure.keyRoot, 0, 11))]
           + (structure.minor ? " minor, " : " major, ") + std::to_string(totalBars) + " bars at "
           + std::to_string(structure.bpm) + " BPM";
}

} // namespace Styles
