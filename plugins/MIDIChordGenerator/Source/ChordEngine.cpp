#include "ChordEngine.h"

ChordEngine::ChordEngine() {}

void ChordEngine::processNoteOn(int noteNumber, int velocity, int channel,
                                 juce::MidiBuffer& output, int samplePosition) {
    // Generate chord from root note
    auto chordNotes = generateChord(noteNumber);

    // Store active chord
    ActiveNote active;
    active.rootNote = noteNumber;
    active.chordNotes = chordNotes;
    active.channel = channel;
    active.velocity = velocity;
    activeNotes_.push_back(active);

    // Output chord notes
    int vel = getVelocityForNote(velocity);
    float normalizedVel = vel / 127.0f;

    for (size_t i = 0; i < chordNotes.size(); ++i) {
        int note = chordNotes[i];
        if (note >= 0 && note <= 127) {
            int strumOffset = (int)(strumDelay_ * i); // Strum delay in samples (simplified)
            output.addEvent(juce::MidiMessage::noteOn(channel, note, normalizedVel),
                          samplePosition + strumOffset);
        }
    }
}

void ChordEngine::processNoteOff(int noteNumber, int channel,
                                  juce::MidiBuffer& output, int samplePosition) {
    // Find and remove active chord for this root note
    for (auto it = activeNotes_.begin(); it != activeNotes_.end(); ++it) {
        if (it->rootNote == noteNumber && it->channel == channel) {
            // Send note offs for all chord notes
            for (int note : it->chordNotes) {
                if (note >= 0 && note <= 127) {
                    output.addEvent(juce::MidiMessage::noteOff(channel, note),
                                  samplePosition);
                }
            }
            activeNotes_.erase(it);
            break;
        }
    }
}

std::vector<int> ChordEngine::generateChord(int rootNote) {
    std::vector<int> intervals = getChordIntervals(chordType_);
    std::vector<int> notes;

    // Apply octave shift to root
    int shiftedRoot = rootNote + (octaveShift_ * 12);

    // Generate chord notes from intervals
    for (int interval : intervals) {
        notes.push_back(shiftedRoot + interval);
    }

    // Apply voicing
    applyVoicing(notes, voicing_);

    return notes;
}

std::vector<int> ChordEngine::getChordIntervals(ChordType type) {
    // Returns semitone intervals from root
    switch (type) {
        case ChordType::Major:          return {0, 4, 7};
        case ChordType::Minor:          return {0, 3, 7};
        case ChordType::Diminished:     return {0, 3, 6};
        case ChordType::Augmented:      return {0, 4, 8};
        case ChordType::Sus2:           return {0, 2, 7};
        case ChordType::Sus4:           return {0, 5, 7};
        case ChordType::Major7:         return {0, 4, 7, 11};
        case ChordType::Minor7:         return {0, 3, 7, 10};
        case ChordType::Dominant7:      return {0, 4, 7, 10};
        case ChordType::MinorMaj7:      return {0, 3, 7, 11};
        case ChordType::Diminished7:    return {0, 3, 6, 9};
        case ChordType::HalfDiminished7: return {0, 3, 6, 10};
        case ChordType::Major6:         return {0, 4, 7, 9};
        case ChordType::Minor6:         return {0, 3, 7, 9};
        case ChordType::Add9:           return {0, 4, 7, 14};
        case ChordType::Major9:         return {0, 4, 7, 11, 14};
        case ChordType::Minor9:         return {0, 3, 7, 10, 14};
        case ChordType::Dominant9:      return {0, 4, 7, 10, 14};
        default:                        return {0, 4, 7};
    }
}

void ChordEngine::applyVoicing(std::vector<int>& notes, Voicing voicing) {
    if (notes.empty()) return;

    switch (voicing) {
        case Voicing::Root:
            // Already in root position
            break;

        case Voicing::First:
            // 1st inversion: move root up an octave
            if (notes.size() >= 2) {
                notes[0] += 12;
                std::sort(notes.begin(), notes.end());
            }
            break;

        case Voicing::Second:
            // 2nd inversion: move root and 3rd up an octave
            if (notes.size() >= 3) {
                notes[0] += 12;
                notes[1] += 12;
                std::sort(notes.begin(), notes.end());
            }
            break;

        case Voicing::Third:
            // 3rd inversion: move root, 3rd, and 5th up an octave
            if (notes.size() >= 4) {
                notes[0] += 12;
                notes[1] += 12;
                notes[2] += 12;
                std::sort(notes.begin(), notes.end());
            }
            break;

        case Voicing::Spread:
            // Wide voicing: spread notes across 2 octaves
            if (notes.size() >= 3) {
                for (size_t i = 1; i < notes.size(); ++i) {
                    notes[i] += 12; // Move upper notes up an octave
                }
            }
            break;

        case Voicing::Drop2:
            // Drop-2 voicing: drop second-highest note down an octave
            if (notes.size() >= 3) {
                notes[notes.size() - 2] -= 12;
                std::sort(notes.begin(), notes.end());
            }
            break;

        case Voicing::Drop3:
            // Drop-3 voicing: drop third-highest note down an octave
            if (notes.size() >= 4) {
                notes[notes.size() - 3] -= 12;
                std::sort(notes.begin(), notes.end());
            }
            break;
    }
}

int ChordEngine::getVelocityForNote(int inputVelocity) const {
    return useInputVelocity_ ? inputVelocity : fixedVelocity_;
}
