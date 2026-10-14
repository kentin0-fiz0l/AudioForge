#include "MIDIGenerator.h"
#include "MusicTheory.h"
#include "Styles.h"
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

bool MIDIGenerator::partPlays(Part part, const TrackStructure::Section& section) {
    // Mostly by energy, so a plan with section types we don't know still
    // builds sensibly. The exception is the breakdown: it drops the kick so the
    // next drop hits, and brings the lead in over the chords.
    const bool breakdown = section.type == "breakdown";
    switch (part) {
        case Part::Chords:   return true;
        case Part::Hats:     return section.energy >= 2;
        case Part::Kick:     return section.energy >= 3 && !breakdown;
        case Part::Backbeat: return section.energy >= 6;
        case Part::Bass:     return section.energy >= 6;
        case Part::Lead:     return section.energy >= 9 || breakdown;
    }
    return true;
}

namespace {

double sixteenth(int step) { return step * 0.25; }

// Writes the notes of one track, a bar at a time. Each add*() method is one
// part of the arrangement; the order they are called in is the order notes
// are added, which the sort in finish() keeps for notes that share a time.
class BarWriter {
public:
    explicit BarWriter(const TrackStructure& structure)
        : structure_(structure),
          style_(Styles::genre(structure.genre)),
          minor_(structure.minor),
          random_(structure.seed) {
        const int key = std::clamp(structure.keyRoot, 0, 11);

        // Bass from F1 to E2, chords from C3, lead from C4
        bassRoot_ = 36 + key > 40 ? 24 + key : 36 + key;
        chordRoot_ = 48 + key;
        leadRoot_ = 60 + key;

        chords_ = MusicTheory::parseProgression(structure.progression);
        if (chords_.empty()) chords_ = {0};

        // Two lead phrases, in scale steps above each bar's chord: the first
        // plays three bars in four, the second answers it in the fourth
        phrase_ = makePhrase();
        answer_ = makePhrase();
    }

    void addBar(const TrackStructure::Section& section, int bar) {
        const BarContext context{section, bar * 4.0, bar - section.startBar,
                                 chords_[static_cast<size_t>(bar) % chords_.size()]};

        if (structure_.hasDrums) addDrums(context);
        if (structure_.hasBass && MIDIGenerator::partPlays(MIDIGenerator::Part::Bass, section)) addBass(context);
        if (structure_.hasChords && MIDIGenerator::partPlays(MIDIGenerator::Part::Chords, section)) addChords(context);
        if (structure_.hasLead && MIDIGenerator::partPlays(MIDIGenerator::Part::Lead, section)) addLead(context);
    }

    // Note-offs ahead of note-ons that share a time, so a re-struck pitch is
    // released before it sounds again
    std::vector<MIDIGenerator::Event> finish() {
        std::stable_sort(events_.begin(), events_.end(), [](const MIDIGenerator::Event& a, const MIDIGenerator::Event& b) {
            if (a.time != b.time) return a.time < b.time;
            return !a.isNoteOn && b.isNoteOn;
        });
        return std::move(events_);
    }

private:
    struct BarContext {
        const TrackStructure::Section& section;
        double barStart;     // In quarter notes
        int barInSection;
        int chord;           // Scale degree of the bar's chord
    };

    std::vector<int> makePhrase() {
        std::vector<int> steps;
        int step = 2 * random_.nextInt(3); // Start on the root, third or fifth
        for (size_t i = 0; i < style_.lead.size(); ++i) {
            steps.push_back(step);
            step = std::clamp(step + random_.nextInt({-2, 3}), -2, 7);
        }
        return steps;
    }

    void addNote(int channel, int note, int velocity, double start, double length) {
        note = std::clamp(note, 0, 127);
        events_.push_back({start, channel, note, std::clamp(velocity, 1, 127), true});
        events_.push_back({start + length, channel, note, 0, false});
    }

    void addDrums(const BarContext& bar) {
        namespace Drums = MusicTheory::DrumNotes;
        using Part = MIDIGenerator::Part;
        const auto& section = bar.section;
        const bool fillBar = section.type == "build" && bar.barInSection == section.lengthBars - 1;

        if (section.type == "drop" && bar.barInSection == 0)
            addNote(kDrumChannel, Drums::CRASH, 100, bar.barStart, 1.0);

        if (MIDIGenerator::partPlays(Part::Kick, section))
            for (int step : style_.kick)
                addNote(kDrumChannel, Drums::KICK, step % 4 == 0 ? 110 : 95, bar.barStart + sixteenth(step), 0.1);

        if (fillBar) { // A snare roll that rises into the next section
            for (int step = 0; step < 16; ++step)
                addNote(kDrumChannel, Drums::SNARE, 50 + step * 5, bar.barStart + sixteenth(step), 0.1);
        } else if (MIDIGenerator::partPlays(Part::Backbeat, section)) {
            for (int step : style_.backbeat)
                addNote(kDrumChannel, style_.backbeatNote, 100, bar.barStart + sixteenth(step), 0.1);
        }

        if (MIDIGenerator::partPlays(Part::Hats, section)) {
            for (int step : style_.closedHat)
                addNote(kDrumChannel, Drums::CLOSED_HAT, step % 2 == 0 ? 72 : 55, bar.barStart + sixteenth(step), 0.1);
            for (int step : style_.openHat)
                addNote(kDrumChannel, Drums::OPEN_HAT, 75, bar.barStart + sixteenth(step), 0.2);
        }
    }

    void addBass(const BarContext& bar) {
        const int note = MusicTheory::degreeToPitch(bassRoot_, minor_, bar.chord);
        for (size_t i = 0; i < style_.bass.size(); ++i) {
            // Held for the genre's length, but released before the next bass note
            const int start = style_.bass[i];
            const int next = i + 1 < style_.bass.size() ? style_.bass[i + 1] : 16;
            addNote(kBassChannel, note, i == 0 ? 105 : 88, bar.barStart + sixteenth(start),
                    sixteenth(1) * std::min(style_.bassLength, static_cast<double>(next - start)) * 0.95);
        }
    }

    void addChords(const BarContext& bar) {
        for (int third = 0; third < 3; ++third) // Root, third and fifth from the scale
            addNote(kChordChannel, MusicTheory::degreeToPitch(chordRoot_, minor_, bar.chord + 2 * third), 70,
                    bar.barStart, 3.9);
    }

    void addLead(const BarContext& bar) {
        const auto& steps = bar.barInSection % 4 == 3 ? answer_ : phrase_;
        for (size_t i = 0; i < style_.lead.size(); ++i) {
            const int start = style_.lead[i];
            const int next = i + 1 < style_.lead.size() ? style_.lead[i + 1] : 16;
            addNote(kLeadChannel, MusicTheory::degreeToPitch(leadRoot_, minor_, bar.chord + steps[i]), i == 0 ? 100 : 90,
                    bar.barStart + sixteenth(start), sixteenth(std::min(next - start, 4)) * 0.9);
        }
    }

    const TrackStructure& structure_;
    const Styles::Genre& style_;
    bool minor_;
    juce::Random random_;
    int bassRoot_ = 0, chordRoot_ = 0, leadRoot_ = 0;
    std::vector<int> chords_, phrase_, answer_;
    std::vector<MIDIGenerator::Event> events_;
};

} // namespace

std::unique_ptr<const MIDIGenerator::Sequence> MIDIGenerator::generateFromStructure(const TrackStructure& structure) {
    auto sequence = std::make_unique<Sequence>();
    sequence->id = nextSequenceId();
    sequence->bpm = structure.bpm;

    BarWriter writer(structure);
    for (const auto& section : structure.sections) {
        for (int bar = section.startBar; bar < section.startBar + section.lengthBars; ++bar) {
            sequence->totalBars = std::max(sequence->totalBars, bar + 1);
            writer.addBar(section, bar);
        }
    }

    sequence->events = writer.finish();
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
