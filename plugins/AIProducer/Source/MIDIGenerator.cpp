#include "MIDIGenerator.h"

MIDIGenerator::MIDIGenerator() {}

void MIDIGenerator::generateFromStructure(const TrackStructure& structure) {
    generatedSequence_ = std::make_unique<GeneratedSequence>();
    generatedSequence_->bpm = structure.bpm;
    generatedSequence_->notes.clear();

    // Calculate total bars
    totalBars_ = 0;
    for (const auto& section : structure.sections) {
        int endBar = section.startBar + section.lengthBars;
        if (endBar > totalBars_) {
            totalBars_ = endBar;
        }
    }
    generatedSequence_->totalBars = totalBars_;

    // Generate MIDI for each instrument
    if (structure.hasDrums) {
        generateDrums(structure, generatedSequence_->notes);
    }
    if (structure.hasBass) {
        generateBass(structure, generatedSequence_->notes);
    }
    if (structure.hasChords) {
        generateChords(structure, generatedSequence_->notes);
    }
    if (structure.hasLead) {
        generateLead(structure, generatedSequence_->notes);
    }
}

void MIDIGenerator::fillMIDIBuffer(juce::MidiBuffer& buffer,
                                   int startSample,
                                   int numSamples,
                                   double sampleRate,
                                   double bpm) {
    if (!generatedSequence_) return;

    // Convert sample position to quarter notes
    double samplesPerQuarterNote = (sampleRate * 60.0) / bpm;
    double startQuarterNote = startSample / samplesPerQuarterNote;
    double endQuarterNote = (startSample + numSamples) / samplesPerQuarterNote;

    // Add notes that fall within this time range
    for (const auto& note : generatedSequence_->notes) {
        if (note.startTime >= startQuarterNote && note.startTime < endQuarterNote) {
            int sampleOffset = static_cast<int>((note.startTime - startQuarterNote) * samplesPerQuarterNote);
            sampleOffset = juce::jlimit(0, numSamples - 1, sampleOffset);

            // Note on
            buffer.addEvent(juce::MidiMessage::noteOn(note.channel + 1, note.noteNumber, (juce::uint8)note.velocity),
                           sampleOffset);

            // Note off (schedule it for later)
            int noteOffSample = static_cast<int>((note.startTime + note.duration - startQuarterNote) * samplesPerQuarterNote);
            if (noteOffSample < numSamples) {
                buffer.addEvent(juce::MidiMessage::noteOff(note.channel + 1, note.noteNumber),
                               juce::jlimit(0, numSamples - 1, noteOffSample));
            }
        }
    }
}

void MIDIGenerator::reset() {
    currentPlaybackPosition_ = 0.0;
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
                note.channel = 9; // MIDI drum channel (10 in 1-indexed)
                note.noteNumber = MusicTheory::DrumNotes::KICK;
                note.velocity = 100;
                note.startTime = barStart + (step * 0.25); // 16th notes
                note.duration = 0.1;
                notes.push_back(note);
            }

            // Snare
            for (int step : pattern.snareSteps) {
                MIDINote note;
                note.channel = 9;
                note.noteNumber = MusicTheory::DrumNotes::SNARE;
                note.velocity = 90;
                note.startTime = barStart + (step * 0.25);
                note.duration = 0.1;
                notes.push_back(note);
            }

            // Hi-hats
            for (int step : pattern.hatSteps) {
                MIDINote note;
                note.channel = 9;
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
                note.channel = 1; // Bass channel
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
            root.channel = 2;
            root.noteNumber = rootNote + 12; // C5
            root.velocity = 60;
            root.startTime = bar * 4.0;
            root.duration = 4.0;

            fifth.channel = 2;
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
                    note.channel = 3;
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
