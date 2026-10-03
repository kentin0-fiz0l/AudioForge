#include "MIDIGenerator.h"
#include <algorithm>
#include <atomic>

namespace {

// MIDI channels (1-16), matching the routing described in AI_PRODUCER_README.md
constexpr int kDrumChannel = 10;
constexpr int kBassChannel = 1;
constexpr int kChordChannel = 2;
constexpr int kLeadChannel = 3;

std::uint64_t nextSequenceId() {
    static std::atomic<std::uint64_t> counter{0};
    return ++counter;
}

} // namespace

void MIDIGenerator::HeldNotes::noteOn(int channel, int noteNumber) {
    held_[static_cast<size_t>(channel - 1)].set(static_cast<size_t>(noteNumber));
}

bool MIDIGenerator::HeldNotes::noteOff(int channel, int noteNumber) {
    auto& notes = held_[static_cast<size_t>(channel - 1)];
    const auto note = static_cast<size_t>(noteNumber);

    const bool wasHeld = notes.test(note);
    notes.reset(note);
    return wasHeld;
}

void MIDIGenerator::HeldNotes::releaseAll(juce::MidiBuffer& buffer, int sampleOffset) {
    for (size_t channel = 0; channel < held_.size(); ++channel) {
        auto& notes = held_[channel];
        if (notes.none()) continue;

        for (size_t note = 0; note < notes.size(); ++note) {
            if (notes.test(note)) {
                buffer.addEvent(juce::MidiMessage::noteOff(static_cast<int>(channel) + 1,
                                                           static_cast<int>(note)),
                                sampleOffset);
            }
        }
        notes.reset();
    }
}

bool MIDIGenerator::HeldNotes::isEmpty() const {
    return std::all_of(held_.begin(), held_.end(),
                       [](const std::bitset<128>& notes) { return notes.none(); });
}

std::unique_ptr<const MIDIGenerator::Sequence> MIDIGenerator::generateFromStructure(const TrackStructure& structure) {
    auto sequence = std::make_unique<Sequence>();
    sequence->id = nextSequenceId();
    sequence->bpm = structure.bpm;

    // Calculate total bars
    for (const auto& section : structure.sections) {
        sequence->totalBars = std::max(sequence->totalBars, section.startBar + section.lengthBars);
    }

    // Generate notes for each instrument
    std::vector<MIDINote> notes;
    if (structure.hasDrums) {
        generateDrums(structure, notes);
    }
    if (structure.hasBass) {
        generateBass(structure, notes);
    }
    if (structure.hasChords) {
        generateChords(structure, notes);
    }
    if (structure.hasLead) {
        generateLead(structure, notes);
    }

    // Split each note into a note-on and a note-off event. Keeping them as
    // separate events means a note-off is found when playback reaches it,
    // however many blocks after the note-on that is.
    sequence->events.reserve(notes.size() * 2);
    for (const auto& note : notes) {
        sequence->events.push_back({note.startTime, note.channel, note.noteNumber, note.velocity, true});
        sequence->events.push_back({note.startTime + note.duration, note.channel, note.noteNumber, 0, false});
    }

    std::stable_sort(sequence->events.begin(), sequence->events.end(),
                     [](const Event& a, const Event& b) {
                         if (a.time != b.time) return a.time < b.time;
                         return !a.isNoteOn && b.isNoteOn; // Note-offs first
                     });

    return sequence;
}

void MIDIGenerator::renderRange(const Sequence& sequence,
                                juce::MidiBuffer& buffer,
                                HeldNotes& heldNotes,
                                double fromPpq,
                                double toPpq,
                                double blockStartPpq,
                                double ppqPerSample,
                                int numSamples) {
    if (numSamples <= 0 || ppqPerSample <= 0.0 || toPpq <= fromPpq) return;

    const auto& events = sequence.events;
    auto event = std::lower_bound(events.begin(), events.end(), fromPpq,
                                  [](const Event& e, double time) { return e.time < time; });

    for (; event != events.end() && event->time < toPpq; ++event) {
        const int sampleOffset = juce::jlimit(0, numSamples - 1,
                                              juce::roundToInt((event->time - blockStartPpq) / ppqPerSample));

        if (event->isNoteOn) {
            buffer.addEvent(juce::MidiMessage::noteOn(event->channel, event->noteNumber,
                                                      static_cast<juce::uint8>(event->velocity)),
                            sampleOffset);
            heldNotes.noteOn(event->channel, event->noteNumber);
        } else if (heldNotes.noteOff(event->channel, event->noteNumber)) {
            // Only release notes that are sounding: after a seek, the range can
            // hold note-offs for notes whose note-on was never played
            buffer.addEvent(juce::MidiMessage::noteOff(event->channel, event->noteNumber), sampleOffset);
        }
    }
}

// Generate drums (4/4 house beat)
void MIDIGenerator::generateDrums(const TrackStructure& structure, std::vector<MIDINote>& notes) {
    MusicTheory::DrumPattern pattern = MusicTheory::DrumPatterns::houseClassic();

    for (const auto& section : structure.sections) {
        int startBar = section.startBar;
        int endBar = section.startBar + section.lengthBars;

        for (int bar = startBar; bar < endBar; ++bar) {
            double barStart = bar * 4.0; // 4 quarter notes per bar

            // Kick drum (every 16th note grid)
            for (int step : pattern.kickSteps) {
                MIDINote note;
                note.channel = kDrumChannel;
                note.noteNumber = MusicTheory::DrumNotes::KICK;
                note.velocity = 100;
                note.startTime = barStart + (step * 0.25); // 16th notes
                note.duration = 0.1;
                notes.push_back(note);
            }

            // Snare
            for (int step : pattern.snareSteps) {
                MIDINote note;
                note.channel = kDrumChannel;
                note.noteNumber = MusicTheory::DrumNotes::SNARE;
                note.velocity = 90;
                note.startTime = barStart + (step * 0.25);
                note.duration = 0.1;
                notes.push_back(note);
            }

            // Hi-hats
            for (int step : pattern.hatSteps) {
                MIDINote note;
                note.channel = kDrumChannel;
                note.noteNumber = MusicTheory::DrumNotes::CLOSED_HAT;
                note.velocity = 70;
                note.startTime = barStart + (step * 0.25);
                note.duration = 0.1;
                notes.push_back(note);
            }
        }
    }
}

// Generate bass (simple root note pattern)
void MIDIGenerator::generateBass(const TrackStructure& structure, std::vector<MIDINote>& notes) {
    // Parse key (e.g., "Am" -> A = 57, minor)
    int rootNote = 57; // A3 (MIDI note 57)
    if (structure.key.find("C") != std::string::npos) rootNote = 48;
    else if (structure.key.find("D") != std::string::npos) rootNote = 50;
    else if (structure.key.find("E") != std::string::npos) rootNote = 52;
    else if (structure.key.find("F") != std::string::npos) rootNote = 53;
    else if (structure.key.find("G") != std::string::npos) rootNote = 55;
    else if (structure.key.find("A") != std::string::npos) rootNote = 57;
    else if (structure.key.find("B") != std::string::npos) rootNote = 59;

    for (const auto& section : structure.sections) {
        for (int bar = section.startBar; bar < section.startBar + section.lengthBars; ++bar) {
            // Bass on every beat
            for (int beat = 0; beat < 4; ++beat) {
                MIDINote note;
                note.channel = kBassChannel;
                note.noteNumber = rootNote - 12; // One octave lower
                note.velocity = 80;
                note.startTime = bar * 4.0 + beat;
                note.duration = 0.9; // Slightly staccato
                notes.push_back(note);
            }
        }
    }
}

// Generate chords (4-bar progression)
void MIDIGenerator::generateChords(const TrackStructure& structure, std::vector<MIDINote>& notes) {
    int rootNote = 60; // C4
    if (structure.key.find("A") != std::string::npos) rootNote = 57;

    // Simple chord progression (just root + fifth for now)
    for (const auto& section : structure.sections) {
        for (int bar = section.startBar; bar < section.startBar + section.lengthBars; ++bar) {
            // Whole note chords
            MIDINote root, fifth;
            root.channel = kChordChannel;
            root.noteNumber = rootNote + 12; // C5
            root.velocity = 60;
            root.startTime = bar * 4.0;
            root.duration = 4.0;

            fifth.channel = kChordChannel;
            fifth.noteNumber = rootNote + 12 + 7; // Fifth
            fifth.velocity = 60;
            fifth.startTime = bar * 4.0;
            fifth.duration = 4.0;

            notes.push_back(root);
            notes.push_back(fifth);
        }
    }
}

// Generate lead melody (simple pentatonic)
void MIDIGenerator::generateLead(const TrackStructure& structure, std::vector<MIDINote>& notes) {
    int rootNote = 72; // C5
    if (structure.key.find("A") != std::string::npos) rootNote = 69;

    // Pentatonic scale
    std::vector<int> scale = {0, 2, 4, 7, 9}; // Pentatonic intervals

    // Generate melody only in drop sections
    for (const auto& section : structure.sections) {
        if (section.type == "drop") {
            for (int bar = section.startBar; bar < section.startBar + section.lengthBars; bar += 2) {
                // Simple 8th note melody
                for (int i = 0; i < 8; ++i) {
                    MIDINote note;
                    note.channel = kLeadChannel;
                    note.noteNumber = rootNote + scale[i % scale.size()];
                    note.velocity = 90;
                    note.startTime = bar * 4.0 + i * 0.5;
                    note.duration = 0.4;
                    notes.push_back(note);
                }
            }
        }
    }
}
