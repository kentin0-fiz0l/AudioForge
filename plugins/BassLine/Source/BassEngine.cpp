/*
  BassEngine.cpp - Bass Line Generation Implementation
*/

#include "BassEngine.h"
#include <algorithm>
#include <cmath>

const std::vector<int> BassEngine::MAJOR_SCALE{0, 2, 4, 5, 7, 9, 11};
const std::vector<int> BassEngine::MINOR_SCALE{0, 2, 3, 5, 7, 8, 10};

BassEngine::BassEngine()
{
    std::random_device rd;
    rng_.seed(rd());
}

int BassEngine::midiNoteFromRoot(int root, int octave)
{
    // C1 = 24, C2 = 36, C3 = 48
    int baseOctave = octave + 1;  // Octave 1 = MIDI octave 1
    return (baseOctave * 12) + root;
}

int BassEngine::getFifth(int root)
{
    return (root + 7) % 12;
}

int BassEngine::getChordTone(int root, bool isMajor, int degree)
{
    const auto& scale = isMajor ? MAJOR_SCALE : MINOR_SCALE;
    if (degree >= 0 && degree < static_cast<int>(scale.size()))
        return (root + scale[degree]) % 12;
    return root;
}

std::vector<int> BassEngine::getPassingTones(int fromPitch, int toPitch)
{
    std::vector<int> tones;
    if (std::abs(toPitch - fromPitch) > 2) {
        int direction = (toPitch > fromPitch) ? 1 : -1;
        int current = fromPitch + direction;
        while (current != toPitch) {
            tones.push_back(current);
            current += direction;
        }
    }
    return tones;
}

void BassEngine::addHumanization(std::vector<Note>& notes)
{
    std::uniform_real_distribution<float> timeDist(-0.01f, 0.01f);
    std::uniform_int_distribution<int> velDist(-5, 5);

    for (auto& note : notes) {
        note.startTime += timeDist(rng_);
        int newVel = note.velocity + velDist(rng_);
        note.velocity = std::max(40, std::min(127, newVel));
    }
}

std::vector<BassEngine::Note> BassEngine::generateBassLineInKey(
    int keyRoot,
    bool keyIsMajor,
    const BassParams& params,
    int numBars)
{
    // Create simple chord progression in key
    std::vector<ChordInfo> chords;

    // I - IV - V - I progression (simplified)
    ChordInfo chord1 = {keyRoot, keyIsMajor, 0.0f, 4.0f};
    ChordInfo chord2 = {(keyRoot + 5) % 12, keyIsMajor, 4.0f, 4.0f};
    ChordInfo chord3 = {(keyRoot + 7) % 12, keyIsMajor, 8.0f, 4.0f};
    ChordInfo chord4 = {keyRoot, keyIsMajor, 12.0f, 4.0f};

    chords.push_back(chord1);
    chords.push_back(chord2);
    chords.push_back(chord3);
    chords.push_back(chord4);

    return generateBassLine(chords, params);
}

std::vector<BassEngine::Note> BassEngine::generateBassLine(
    const std::vector<ChordInfo>& chords,
    const BassParams& params)
{
    // Dispatch to style-specific generator
    if (params.style == "Rock")
        return generateRockBass(chords, params);
    else if (params.style == "Funk")
        return generateFunkBass(chords, params);
    else if (params.style == "Jazz")
        return generateJazzBass(chords, params);
    else if (params.style == "EDM")
        return generateEDMBass(chords, params);
    else if (params.style == "Reggae")
        return generateReggaeBass(chords, params);
    else if (params.style == "Latin")
        return generateLatinBass(chords, params);
    else if (params.style == "Blues")
        return generateBluesBass(chords, params);

    return generateRockBass(chords, params);
}

std::vector<BassEngine::Note> BassEngine::generateRockBass(
    const std::vector<ChordInfo>& chords,
    const BassParams& params)
{
    std::vector<Note> notes;

    for (const auto& chord : chords) {
        int rootPitch = midiNoteFromRoot(chord.root, params.octave);
        int fifthPitch = midiNoteFromRoot(getFifth(chord.root), params.octave);

        // Root-fifth pattern (driving eighth notes)
        float beatPos = chord.startBeat;
        int beatsInChord = static_cast<int>(chord.duration);

        for (int beat = 0; beat < beatsInChord; ++beat) {
            // Root on beats 1 and 3
            if (beat % 2 == 0) {
                notes.push_back({rootPitch, beatPos, 0.45f, 100, true});
            }
            // Fifth on beats 2 and 4 (if enabled)
            else if (params.useFifths) {
                notes.push_back({fifthPitch, beatPos, 0.45f, 95, false});
            }
            beatPos += 1.0f;
        }
    }

    addHumanization(notes);
    return notes;
}

std::vector<BassEngine::Note> BassEngine::generateFunkBass(
    const std::vector<ChordInfo>& chords,
    const BassParams& params)
{
    std::vector<Note> notes;
    std::uniform_int_distribution<int> accentDist(0, 3);

    for (const auto& chord : chords) {
        int rootPitch = midiNoteFromRoot(chord.root, params.octave);
        int fifthPitch = midiNoteFromRoot(getFifth(chord.root), params.octave);

        // Syncopated, groove-heavy pattern (16th notes)
        float beatPos = chord.startBeat;
        int sixteenths = static_cast<int>(chord.duration * 4);

        // Funk pattern: emphasize and 16th, offbeat groove
        for (int i = 0; i < sixteenths; ++i) {
            bool isDownbeat = (i % 4 == 0);
            bool isOffbeat = (i % 4 == 2);

            if (isDownbeat) {
                // Strong root notes
                notes.push_back({rootPitch, beatPos, 0.2f, 110, true});
            }
            else if (isOffbeat && params.useFifths) {
                // Syncopated fifths
                notes.push_back({fifthPitch, beatPos, 0.15f, 95, false});
            }
            else if (params.complexity > 5 && accentDist(rng_) == 0) {
                // Ghost notes (lower velocity)
                notes.push_back({rootPitch, beatPos, 0.1f, 60, false});
            }

            beatPos += 0.25f;
        }
    }

    addHumanization(notes);
    return notes;
}

std::vector<BassEngine::Note> BassEngine::generateJazzBass(
    const std::vector<ChordInfo>& chords,
    const BassParams& params)
{
    std::vector<Note> notes;

    for (size_t i = 0; i < chords.size(); ++i) {
        const auto& chord = chords[i];
        int rootPitch = midiNoteFromRoot(chord.root, params.octave);

        // Walking bass (quarter notes)
        float beatPos = chord.startBeat;
        int numBeats = static_cast<int>(chord.duration);

        for (int beat = 0; beat < numBeats; ++beat) {
            if (beat == 0) {
                // Start on root
                notes.push_back({rootPitch, beatPos, 0.9f, 95, true});
            }
            else if (beat == numBeats - 1 && params.usePassingTones && i < chords.size() - 1) {
                // Chromatic approach to next chord
                int nextRoot = midiNoteFromRoot(chords[i + 1].root, params.octave);
                int approach = (nextRoot > rootPitch) ? nextRoot - 1 : nextRoot + 1;
                notes.push_back({approach, beatPos, 0.9f, 90, false});
            }
            else {
                // Chord tones (3rd, 5th, 7th)
                int degree = ((beat % 3) == 1) ? 2 : 4;  // 3rd or 5th
                int tone = getChordTone(chord.root, chord.isMajor, degree);
                int pitch = midiNoteFromRoot(tone, params.octave);
                notes.push_back({pitch, beatPos, 0.9f, 92, false});
            }

            beatPos += 1.0f;
        }
    }

    addHumanization(notes);
    return notes;
}

std::vector<BassEngine::Note> BassEngine::generateEDMBass(
    const std::vector<ChordInfo>& chords,
    const BassParams& params)
{
    std::vector<Note> notes;

    for (const auto& chord : chords) {
        int rootPitch = midiNoteFromRoot(chord.root, params.octave);

        // Sustained root notes (whole notes or half notes)
        float duration = (params.complexity > 5) ? 2.0f : chord.duration;
        float beatPos = chord.startBeat;

        while (beatPos < chord.startBeat + chord.duration) {
            notes.push_back({rootPitch, beatPos, duration * 0.95f, 120, true});
            beatPos += duration;
        }
    }

    // No humanization for EDM (quantized)
    return notes;
}

std::vector<BassEngine::Note> BassEngine::generateReggaeBass(
    const std::vector<ChordInfo>& chords,
    const BassParams& params)
{
    std::vector<Note> notes;

    for (const auto& chord : chords) {
        int rootPitch = midiNoteFromRoot(chord.root, params.octave);

        // Offbeat pattern (emphasis on beats 2 and 4)
        float beatPos = chord.startBeat;
        int numBeats = static_cast<int>(chord.duration);

        for (int beat = 0; beat < numBeats; ++beat) {
            bool isOffbeat = (beat % 2 == 1);

            if (isOffbeat) {
                // Strong offbeat accents
                notes.push_back({rootPitch, beatPos, 0.4f, 105, true});
            }
            else if (params.complexity > 3) {
                // Light downbeats
                notes.push_back({rootPitch, beatPos, 0.3f, 75, false});
            }

            beatPos += 1.0f;
        }
    }

    addHumanization(notes);
    return notes;
}

std::vector<BassEngine::Note> BassEngine::generateLatinBass(
    const std::vector<ChordInfo>& chords,
    const BassParams& params)
{
    std::vector<Note> notes;

    for (const auto& chord : chords) {
        int rootPitch = midiNoteFromRoot(chord.root, params.octave);
        int fifthPitch = midiNoteFromRoot(getFifth(chord.root), params.octave);

        // Tumbao pattern (3-2 or 2-3 clave)
        float beatPos = chord.startBeat;
        int numBeats = static_cast<int>(chord.duration);

        for (int beat = 0; beat < numBeats; ++beat) {
            // Tumbao: syncopated pattern
            if (beat == 0 || beat == 2) {
                notes.push_back({rootPitch, beatPos, 0.45f, 100, true});
            }
            else if (beat == 1 && params.useFifths) {
                notes.push_back({fifthPitch, beatPos + 0.5f, 0.4f, 95, false});
            }

            beatPos += 1.0f;
        }
    }

    addHumanization(notes);
    return notes;
}

std::vector<BassEngine::Note> BassEngine::generateBluesBass(
    const std::vector<ChordInfo>& chords,
    const BassParams& params)
{
    std::vector<Note> notes;

    for (const auto& chord : chords) {
        int rootPitch = midiNoteFromRoot(chord.root, params.octave);
        int fifthPitch = midiNoteFromRoot(getFifth(chord.root), params.octave);

        // Blues shuffle pattern (triplet feel)
        float beatPos = chord.startBeat;
        int numBeats = static_cast<int>(chord.duration);

        for (int beat = 0; beat < numBeats; ++beat) {
            // Root on downbeat
            notes.push_back({rootPitch, beatPos, 0.6f, 100, true});

            // Fifth on offbeat (shuffle)
            if (params.useFifths && beat % 2 == 0) {
                notes.push_back({fifthPitch, beatPos + 0.67f, 0.3f, 90, false});
            }

            beatPos += 1.0f;
        }
    }

    addHumanization(notes);
    return notes;
}
