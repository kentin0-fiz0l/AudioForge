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
        int harmonyNote = transposeInScale(rootNote, 2, 4);
        if (harmonyNote != rootNote)
            notes.push_back(harmonyNote);
    }

    // Fifth above
    if (fifthAbove_) {
        int harmonyNote = transposeInScale(rootNote, 4, 7);
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
        int harmonyNote = transposeInScale(rootNote, -2, -4);
        if (harmonyNote != rootNote)
            notes.push_back(harmonyNote);
    }

    // Fifth below
    if (fifthBelow_) {
        int harmonyNote = transposeInScale(rootNote, -4, -7);
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

// Returns the harmony note `degrees` scale steps from noteNumber (2 = 3rd, 4 = 5th,
// negative = below). `semitones` is the interval used where scale steps don't apply.
int HarmonizerEngine::transposeInScale(int noteNumber, int degrees, int semitones) const {
    if (scale_ == Scale::Chromatic)
        return noteNumber + semitones;

    const auto scale = getScaleTones(scale_);
    const int keyOffset = static_cast<int>(key_);
    auto inScale = [&](int note) {
        int pitchClass = ((note - keyOffset) % 12 + 12) % 12;
        for (int i = 0; i < scale.size; ++i)
            if (scale.tones[i] == pitchClass) return true;
        return false;
    };

    if (scale.size < 7) {
        // Pentatonic/Blues have no diatonic 3rd or 5th: use the scale note nearest
        // to the chromatic interval, preferring the one closer to the input note.
        const int target = noteNumber + semitones;
        const int towardInput = semitones > 0 ? -1 : 1;
        for (int distance = 0; distance < 12; ++distance) {
            if (inScale(target + towardInput * distance)) return target + towardInput * distance;
            if (inScale(target - towardInput * distance)) return target - towardInput * distance;
        }
        return target;
    }

    // Seven-note scale: find the input's degree (the scale note at or below it)
    const int relative = noteNumber - keyOffset;
    const int octave = (relative >= 0 ? relative : relative - 11) / 12;
    const int pitchClass = relative - octave * 12;
    int degree = 0;
    while (degree < 6 && scale.tones[degree + 1] <= pitchClass) ++degree;

    // An out-of-key note harmonizes from its scale neighbour on the harmony's side
    if (scale.tones[degree] != pitchClass && degrees > 0)
        ++degree;

    const int target = octave * 7 + degree + degrees;
    const int targetOctave = (target >= 0 ? target : target - 6) / 7;
    return keyOffset + targetOctave * 12 + scale.tones[target - targetOctave * 7];
}

HarmonizerEngine::ScaleTones HarmonizerEngine::getScaleTones(Scale scale) {
    switch (scale) {
        case Scale::Minor:
        case Scale::Aeolian:       return {{0, 2, 3, 5, 7, 8, 10}, 7};
        case Scale::Dorian:        return {{0, 2, 3, 5, 7, 9, 10}, 7};
        case Scale::Phrygian:      return {{0, 1, 3, 5, 7, 8, 10}, 7};
        case Scale::Lydian:        return {{0, 2, 4, 6, 7, 9, 11}, 7};
        case Scale::Mixolydian:    return {{0, 2, 4, 5, 7, 9, 10}, 7};
        case Scale::Locrian:       return {{0, 1, 3, 5, 6, 8, 10}, 7};
        case Scale::HarmonicMinor: return {{0, 2, 3, 5, 7, 8, 11}, 7};
        case Scale::MelodicMinor:  return {{0, 2, 3, 5, 7, 9, 11}, 7};
        case Scale::Pentatonic:    return {{0, 2, 4, 7, 9}, 5};
        case Scale::Blues:         return {{0, 3, 5, 6, 7, 10}, 6};
        case Scale::Major:
        default:                   return {{0, 2, 4, 5, 7, 9, 11}, 7};
    }
}

int HarmonizerEngine::getVelocityForNote(int inputVelocity) const {
    return useInputVelocity_ ? inputVelocity : fixedVelocity_;
}
