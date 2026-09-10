#include "HarmonizerEngine.h"

HarmonizerEngine::HarmonizerEngine() {}

void HarmonizerEngine::processNoteOn(int noteNumber, int velocity, int channel,
                                      juce::MidiBuffer& output, int samplePosition) {
    // Always output the original note
    float normalizedVel = velocity / 127.0f;
    output.addEvent(juce::MidiMessage::noteOn(channel, noteNumber, normalizedVel),
                   samplePosition);

    // Generate harmony notes
    auto harmonyNotes = generateHarmonyNotes(noteNumber);

    // Store active note
    ActiveNote active;
    active.originalNote = noteNumber;
    active.harmonyNotes = harmonyNotes;
    active.channel = channel;
    active.velocity = velocity;
    activeNotes_.push_back(active);

    // Output harmony notes based on mix parameter
    if (mix_ > 0.0f) {
        int vel = getVelocityForNote(velocity);
        float harmonyVel = (vel / 127.0f) * mix_;

        for (int harmonyNote : harmonyNotes) {
            if (harmonyNote >= 0 && harmonyNote <= 127) {
                output.addEvent(juce::MidiMessage::noteOn(channel, harmonyNote, harmonyVel),
                              samplePosition);
            }
        }
    }
}

void HarmonizerEngine::processNoteOff(int noteNumber, int channel,
                                       juce::MidiBuffer& output, int samplePosition) {
    // Output note off for original note
    output.addEvent(juce::MidiMessage::noteOff(channel, noteNumber),
                   samplePosition);

    // Find and remove active note, sending note offs for harmony
    for (auto it = activeNotes_.begin(); it != activeNotes_.end(); ++it) {
        if (it->originalNote == noteNumber && it->channel == channel) {
            // Send note offs for all harmony notes
            for (int harmonyNote : it->harmonyNotes) {
                if (harmonyNote >= 0 && harmonyNote <= 127) {
                    output.addEvent(juce::MidiMessage::noteOff(channel, harmonyNote),
                                  samplePosition);
                }
            }
            activeNotes_.erase(it);
            break;
        }
    }
}

std::vector<int> HarmonizerEngine::generateHarmonyNotes(int rootNote) {
    std::vector<int> notes;

    // Third above
    if (thirdAbove_) {
        int harmonyNote = transposeInScale(rootNote, 2);  // 2 scale degrees up
        if (harmonyNote != rootNote)
            notes.push_back(harmonyNote);
    }

    // Fifth above
    if (fifthAbove_) {
        int harmonyNote = transposeInScale(rootNote, 4);  // 4 scale degrees up
        if (harmonyNote != rootNote)
            notes.push_back(harmonyNote);
    }

    // Octave above
    if (octaveAbove_) {
        int harmonyNote = rootNote + 12;
        if (harmonyNote <= 127)
            notes.push_back(harmonyNote);
    }

    // Third below
    if (thirdBelow_) {
        int harmonyNote = transposeInScale(rootNote, -2);  // 2 scale degrees down
        if (harmonyNote != rootNote)
            notes.push_back(harmonyNote);
    }

    // Fifth below
    if (fifthBelow_) {
        int harmonyNote = transposeInScale(rootNote, -4);  // 4 scale degrees down
        if (harmonyNote != rootNote)
            notes.push_back(harmonyNote);
    }

    // Octave below
    if (octaveBelow_) {
        int harmonyNote = rootNote - 12;
        if (harmonyNote >= 0)
            notes.push_back(harmonyNote);
    }

    return notes;
}

int HarmonizerEngine::transposeInScale(int noteNumber, int degrees) {
    if (scale_ == Scale::Chromatic) {
        // In chromatic mode, use fixed semitone intervals
        if (degrees == 2) return noteNumber + 4;      // Major 3rd
        if (degrees == -2) return noteNumber - 4;     // Major 3rd down
        if (degrees == 4) return noteNumber + 7;      // Perfect 5th
        if (degrees == -4) return noteNumber - 7;     // Perfect 5th down
        return noteNumber;
    }

    // Get scale pattern
    auto pattern = getScalePattern(scale_);

    // Find current position in scale
    int keyOffset = static_cast<int>(key_);
    int relativeNote = (noteNumber - keyOffset + 120) % 12;  // +120 to ensure positive

    // Find which scale degree we're on
    int currentDegree = -1;
    for (int i = 0; i < 7; ++i) {
        if (pattern[i] == relativeNote) {
            currentDegree = i;
            break;
        }
    }

    // If note not in scale, find closest
    if (currentDegree == -1) {
        for (int i = 0; i < 7; ++i) {
            if (pattern[i] >= relativeNote) {
                currentDegree = i;
                break;
            }
        }
        if (currentDegree == -1) currentDegree = 0;
    }

    // Calculate target degree
    int targetDegree = (currentDegree + degrees + 70) % 7;  // +70 to ensure positive
    int octaveShift = (currentDegree + degrees) / 7;
    if (currentDegree + degrees < 0) octaveShift = -1;

    // Get target note in scale
    int targetRelative = pattern[targetDegree];
    int targetNote = keyOffset + targetRelative + (octaveShift * 12);

    // Adjust to match original octave range
    while (targetNote < noteNumber - 6 && degrees > 0) targetNote += 12;
    while (targetNote > noteNumber + 6 && degrees < 0) targetNote -= 12;

    return juce::jlimit(0, 127, targetNote);
}

std::array<int, 12> HarmonizerEngine::getScalePattern(Scale scale) {
    // Returns semitone positions for 7 notes (rest are -1)
    std::array<int, 12> pattern;
    pattern.fill(-1);

    switch (scale) {
        case Scale::Chromatic:
            for (int i = 0; i < 12; ++i) pattern[i] = i;
            break;
        case Scale::Major:
            pattern = {0, -1, 2, -1, 4, 5, -1, 7, -1, 9, -1, 11};
            break;
        case Scale::Minor:
        case Scale::Aeolian:
            pattern = {0, -1, 2, 3, -1, 5, -1, 7, 8, -1, 10, -1};
            break;
        case Scale::Dorian:
            pattern = {0, -1, 2, 3, -1, 5, -1, 7, -1, 9, 10, -1};
            break;
        case Scale::Phrygian:
            pattern = {0, 1, -1, 3, -1, 5, -1, 7, 8, -1, 10, -1};
            break;
        case Scale::Lydian:
            pattern = {0, -1, 2, -1, 4, -1, 6, 7, -1, 9, -1, 11};
            break;
        case Scale::Mixolydian:
            pattern = {0, -1, 2, -1, 4, 5, -1, 7, -1, 9, 10, -1};
            break;
        case Scale::Locrian:
            pattern = {0, 1, -1, 3, -1, 5, 6, -1, 8, -1, 10, -1};
            break;
        case Scale::HarmonicMinor:
            pattern = {0, -1, 2, 3, -1, 5, -1, 7, 8, -1, -1, 11};
            break;
        case Scale::MelodicMinor:
            pattern = {0, -1, 2, 3, -1, 5, -1, 7, -1, 9, -1, 11};
            break;
        case Scale::Pentatonic:
            pattern = {0, -1, 2, -1, 4, -1, -1, 7, -1, 9, -1, -1};
            break;
        case Scale::Blues:
            pattern = {0, -1, -1, 3, -1, 5, 6, 7, -1, -1, 10, -1};
            break;
        default:
            pattern = {0, -1, 2, -1, 4, 5, -1, 7, -1, 9, -1, 11};  // Default to Major
    }

    return pattern;
}

int HarmonizerEngine::getVelocityForNote(int inputVelocity) const {
    return useInputVelocity_ ? inputVelocity : fixedVelocity_;
}
