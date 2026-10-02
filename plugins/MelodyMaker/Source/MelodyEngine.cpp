/*
  MelodyEngine.cpp - Implementation
*/

#include "MelodyEngine.h"
#include <algorithm>
#include <cmath>

// Static member definitions
const std::vector<int> MelodyEngine::MAJOR_SCALE{0, 2, 4, 5, 7, 9, 11};
const std::vector<int> MelodyEngine::MINOR_SCALE{0, 2, 3, 5, 7, 8, 10};
const std::vector<int> MelodyEngine::BLUES_SCALE{0, 3, 5, 6, 7, 10};

const std::map<std::string, std::vector<float> > MelodyEngine::RHYTHM_PATTERNS{
    {"pop", {0.0f, 0.5f, 1.0f, 1.5f, 2.0f, 2.5f, 3.0f, 3.5f}},
    {"jazz", {0.0f, 0.33f, 0.67f, 1.0f, 1.33f, 1.67f, 2.0f, 2.33f}},
    {"edm", {0.0f, 0.25f, 0.5f, 0.75f, 1.0f, 1.25f, 1.5f, 1.75f}},
    {"ballad", {0.0f, 1.0f, 2.0f, 3.0f}}
};

MelodyEngine::MelodyEngine()
    : rng_(std::random_device{}())
{
}

std::vector<MelodyEngine::Note> MelodyEngine::generateMelody(
    const std::vector<int>& chordRoots,
    const std::vector<std::string>& chordQualities,
    const MelodyParams& params,
    int numBars)
{
    std::vector<Note> melody;

    // For now, use the first chord's root as the key
    if (chordRoots.empty()) return melody;

    int keyRoot = chordRoots[0];
    bool isMajor = (chordQualities[0] == "major" || chordQualities[0] == "maj");

    auto scale = getScaleNotes(keyRoot, isMajor);

    // Generate based on style
    std::string style = params.style;
    std::transform(style.begin(), style.end(), style.begin(), ::tolower);

    if (style == "pop" || style == "rock") {
        melody = generatePopMelody(scale, numBars);
    } else if (style == "jazz") {
        melody = generateJazzMelody(scale, numBars);
    } else if (style == "edm" || style == "funk") {
        melody = generateEDMMelody(scale, numBars);
    } else if (style == "ballad" || style == "classical") {
        melody = generateBalladMelody(scale, numBars);
    } else {
        melody = generatePopMelody(scale, numBars);
    }

    // Apply contour and constraints
    constrainToRange(melody, params.minPitch, params.maxPitch);

    if (params.rhythmComplexity > 5) {
        humanize(melody, 0.3f);
    }

    return melody;
}

std::vector<MelodyEngine::Note> MelodyEngine::generateMelodyInScale(
    int rootNote,
    bool isMajor,
    const MelodyParams& params,
    int numBars)
{
    auto scale = getScaleNotes(rootNote, isMajor);
    auto melody = generatePopMelody(scale, numBars);
    constrainToRange(melody, params.minPitch, params.maxPitch);
    return melody;
}

void MelodyEngine::humanize(std::vector<Note>& melody, float amount)
{
    for (auto& note : melody) {
        // Add slight timing variation
        float timingOffset = getRandomFloat(-0.05f, 0.05f) * amount;
        note.startTime += timingOffset;

        // Add velocity variation
        int velocityOffset = static_cast<int>(getRandomFloat(-10.0f, 10.0f) * amount);
        note.velocity = std::clamp(note.velocity + velocityOffset, 1, 127);
    }
}

std::vector<int> MelodyEngine::getScaleNotes(int root, bool isMajor)
{
    const auto& intervals = isMajor ? MAJOR_SCALE : MINOR_SCALE;
    std::vector<int> scale;

    for (int octave = -1; octave <= 2; ++octave) {
        for (int interval : intervals) {
            int note = root + interval + (octave * 12);
            if (note >= 0 && note < 128) {
                scale.push_back(note);
            }
        }
    }

    return scale;
}

std::vector<int> MelodyEngine::getChordTones(int root, const std::string& quality)
{
    std::vector<int> tones;
    tones.push_back(root);           // Root

    if (quality == "major" || quality == "maj") {
        tones.push_back(root + 4);   // Major 3rd
        tones.push_back(root + 7);   // Perfect 5th
    } else if (quality == "minor" || quality == "min") {
        tones.push_back(root + 3);   // Minor 3rd
        tones.push_back(root + 7);   // Perfect 5th
    } else if (quality == "7") {
        tones.push_back(root + 4);
        tones.push_back(root + 7);
        tones.push_back(root + 10);  // Minor 7th
    }

    return tones;
}

bool MelodyEngine::isInScale(int note, const std::vector<int>& scale)
{
    return std::find(scale.begin(), scale.end(), note) != scale.end();
}

int MelodyEngine::findNearestScaleNote(int note, const std::vector<int>& scale)
{
    if (scale.empty()) return note;

    int nearest = scale[0];
    int minDist = std::abs(note - nearest);

    for (int scaleNote : scale) {
        int dist = std::abs(note - scaleNote);
        if (dist < minDist) {
            minDist = dist;
            nearest = scaleNote;
        }
    }

    return nearest;
}

std::vector<MelodyEngine::Note> MelodyEngine::generatePopMelody(
    const std::vector<int>& scale,
    int numBars)
{
    std::vector<Note> melody;
    int numNotes = numBars * 4; // 4 notes per bar

    int currentPitch = scale[scale.size() / 2]; // Start in middle

    for (int i = 0; i < numNotes; ++i) {
        Note note;
        note.startTime = static_cast<float>(i);
        note.duration = getRandomInt(0, 2) == 0 ? 0.5f : 1.0f;
        note.velocity = getRandomInt(80, 100);

        // Melodic motion: mostly steps, occasional leaps
        int motion = getRandomInt(-3, 3);
        int targetPitch = currentPitch + motion;
        note.pitch = findNearestScaleNote(targetPitch, scale);

        currentPitch = note.pitch;
        note.isChordTone = true; // Simplified

        melody.push_back(note);
    }

    return melody;
}

std::vector<MelodyEngine::Note> MelodyEngine::generateJazzMelody(
    const std::vector<int>& scale,
    int numBars)
{
    std::vector<Note> melody;
    int numNotes = numBars * 6; // More notes for jazz

    int currentPitch = scale[scale.size() / 2];

    for (int i = 0; i < numNotes; ++i) {
        Note note;
        note.startTime = static_cast<float>(i) * 0.67f; // Triplet feel
        note.duration = 0.33f;
        note.velocity = getRandomInt(70, 95);

        // More chromatic movement in jazz
        int motion = getRandomInt(-4, 4);
        note.pitch = currentPitch + motion;

        // Snap to scale if too far
        if (!isInScale(note.pitch, scale)) {
            note.pitch = findNearestScaleNote(note.pitch, scale);
        }

        currentPitch = note.pitch;
        melody.push_back(note);
    }

    return melody;
}

std::vector<MelodyEngine::Note> MelodyEngine::generateEDMMelody(
    const std::vector<int>& scale,
    int numBars)
{
    std::vector<Note> melody;
    int numNotes = numBars * 8; // 16th note patterns

    int currentPitch = scale[scale.size() / 2 + 3]; // Higher for EDM

    for (int i = 0; i < numNotes; ++i) {
        Note note;
        note.startTime = static_cast<float>(i) * 0.5f;
        note.duration = 0.25f;
        note.velocity = 100; // Constant velocity for EDM

        // Arpeggiated patterns
        if (i % 4 == 0) {
            currentPitch = scale[scale.size() / 2 + getRandomInt(0, 6)];
        }

        note.pitch = currentPitch;
        melody.push_back(note);
    }

    return melody;
}

std::vector<MelodyEngine::Note> MelodyEngine::generateBalladMelody(
    const std::vector<int>& scale,
    int numBars)
{
    std::vector<Note> melody;
    int numNotes = numBars * 2; // Slower, longer notes

    int currentPitch = scale[scale.size() / 2];

    for (int i = 0; i < numNotes; ++i) {
        Note note;
        note.startTime = static_cast<float>(i) * 2.0f;
        note.duration = 2.0f;
        note.velocity = getRandomInt(60, 85);

        // Smooth, stepwise motion
        int motion = getRandomInt(-2, 2);
        int targetPitch = currentPitch + motion;
        note.pitch = findNearestScaleNote(targetPitch, scale);

        currentPitch = note.pitch;
        melody.push_back(note);
    }

    return melody;
}

std::vector<float> MelodyEngine::generateRhythm(int numBars, int complexity, bool syncopated)
{
    std::vector<float> rhythm;
    float totalBeats = static_cast<float>(numBars * 4);

    float subdivision = (complexity > 5) ? 0.25f : 0.5f;

    for (float beat = 0.0f; beat < totalBeats; beat += subdivision) {
        if (syncopated && getRandomInt(0, 3) == 0) {
            beat += subdivision * 0.5f; // Offset for syncopation
        }
        rhythm.push_back(beat);
    }

    return rhythm;
}

void MelodyEngine::applyContour(std::vector<Note>& melody, const std::string& shape)
{
    // Simple arc: start low, peak in middle, end low
    if (shape == "arc") {
        for (size_t i = 0; i < melody.size(); ++i) {
            float progress = static_cast<float>(i) / melody.size();
            float height = std::sin(progress * 3.14159f) * 12.0f;
            melody[i].pitch += static_cast<int>(height);
        }
    }
}

void MelodyEngine::constrainToRange(std::vector<Note>& melody, int minPitch, int maxPitch)
{
    for (auto& note : melody) {
        note.pitch = std::clamp(note.pitch, minPitch, maxPitch);
    }
}

int MelodyEngine::getRandomInt(int min, int max)
{
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng_);
}

float MelodyEngine::getRandomFloat(float min, float max)
{
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng_);
}
