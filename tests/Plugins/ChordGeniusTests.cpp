/**
 * ChordGenius Plugin Tests
 *
 * ChordGenius is a MIDI effect that plays the selected chord progression,
 * one chord per bar, in time with the host transport:
 * - Chords land on the bar grid and follow the progression
 * - Roman numerals map to the right scale degrees (IV is not V)
 * - Key and mode transpose the progression
 * - Stopping the transport releases what is sounding
 * - Key, mode and genre survive a save and reload
 */

#include <juce_audio_processors/juce_audio_processors.h>

#include "../../plugins/ChordGenius/Source/PluginProcessor.h"

namespace
{
    constexpr double testSampleRate = 44100.0;
    constexpr int testBlockSize = 512;

    // Reports a transport state to the processor, as a host would
    class FakePlayHead : public juce::AudioPlayHead
    {
    public:
        juce::Optional<PositionInfo> getPosition() const override
        {
            PositionInfo info;
            info.setIsPlaying(playing);
            info.setPpqPosition(ppq);
            info.setBpm(bpm);
            info.setTimeSignature(TimeSignature { 4, 4 });
            return info;
        }

        bool playing = false;
        double ppq = 0.0;
        double bpm = 120.0;
    };

    struct NoteEvent
    {
        int note;
        bool isOn;
        juce::int64 sample; // Since the transport started
    };

    // A processor wired to a fake host transport that advances as blocks are processed
    struct PlaybackHarness
    {
        PlaybackHarness()
        {
            processor.setPlayHead(&playHead);
            processor.prepareToPlay(testSampleRate, testBlockSize);
        }

        // Processes enough blocks to cover the given number of quarter notes,
        // returning every note event with its time
        std::vector<NoteEvent> play(double quarterNotes)
        {
            playHead.playing = true;
            const double ppqPerBlock = testBlockSize * playHead.bpm / (60.0 * testSampleRate);
            const int numBlocks = (int) std::ceil(quarterNotes / ppqPerBlock);

            std::vector<NoteEvent> events;
            for (int i = 0; i < numBlocks; ++i)
            {
                collect(processBlock(), events);
                playHead.ppq += ppqPerBlock;
                samplesPlayed += testBlockSize;
            }
            return events;
        }

        std::vector<NoteEvent> stop()
        {
            playHead.playing = false;
            std::vector<NoteEvent> events;
            collect(processBlock(), events);
            return events;
        }

        juce::MidiBuffer processBlock()
        {
            juce::MidiBuffer midi;
            audio.clear();
            processor.processBlock(audio, midi);
            return midi;
        }

        void collect(const juce::MidiBuffer& midi, std::vector<NoteEvent>& events) const
        {
            for (const auto metadata : midi)
            {
                const auto message = metadata.getMessage();
                if (message.isNoteOnOrOff())
                    events.push_back({ message.getNoteNumber(), message.isNoteOn(),
                                       samplesPlayed + metadata.samplePosition });
            }
        }

        ChordGeniusProcessor processor;
        FakePlayHead playHead;
        juce::AudioBuffer<float> audio { 2, testBlockSize };
        juce::int64 samplesPlayed = 0;
    };

    juce::AudioParameterChoice* findChoice(juce::AudioProcessor& processor, const juce::String& id)
    {
        for (auto* param : processor.getParameters())
            if (auto* choice = dynamic_cast<juce::AudioParameterChoice*>(param))
                if (choice->paramID == id)
                    return choice;
        return nullptr;
    }

    // Selects a choice by its displayed name, as a user would; false if there is no such choice
    bool select(juce::AudioProcessor& processor, const juce::String& id, const juce::String& name)
    {
        auto* choice = findChoice(processor, id);
        if (choice == nullptr || !choice->choices.contains(name))
            return false;

        choice->setValueNotifyingHost(choice->convertTo0to1((float) choice->choices.indexOf(name)));
        return true;
    }

    // The notes switched on at a given time, sorted
    std::vector<int> notesStartingAt(const std::vector<NoteEvent>& events, juce::int64 sample)
    {
        std::vector<int> notes;
        for (const auto& e : events)
            if (e.isOn && std::abs(e.sample - sample) <= 1)
                notes.push_back(e.note);
        std::sort(notes.begin(), notes.end());
        return notes;
    }

    juce::int64 barStartSample(int bar, double bpm = 120.0)
    {
        return juce::roundToInt(bar * 4 * 60.0 / bpm * testSampleRate);
    }
}

class ChordGeniusTests : public juce::UnitTest
{
public:
    ChordGeniusTests()
        : juce::UnitTest("ChordGenius", "Plugins")
    {
    }

    void runTest() override
    {
        testPlaysProgressionOnBarGrid();
        testSilentWhenTransportStopped();
        testStopReleasesNotes();
        testFourIsNotFive();
        testKeyAndMode();
        testStateRoundTrip();
        testIgnoresForeignState();
    }

private:
    void expectNotes(const std::vector<int>& actual, std::vector<int> expected, const juce::String& what)
    {
        std::sort(expected.begin(), expected.end());
        juce::String a, e;
        for (int n : actual) a << n << " ";
        for (int n : expected) e << n << " ";
        expect(actual == expected, what + ": expected " + e + "got " + a);
    }

    void testPlaysProgressionOnBarGrid()
    {
        beginTest("Plays the pop progression, one chord per bar");

        PlaybackHarness harness;
        const auto events = harness.play(16.0);

        // Default: pop (I-V-vi-IV) in C major
        expectNotes(notesStartingAt(events, barStartSample(0)), { 60, 64, 67 }, "Bar 1 is C");
        expectNotes(notesStartingAt(events, barStartSample(1)), { 67, 71, 74 }, "Bar 2 is G");
        expectNotes(notesStartingAt(events, barStartSample(2)), { 69, 72, 76 }, "Bar 3 is Am");
        expectNotes(notesStartingAt(events, barStartSample(3)), { 65, 69, 72 }, "Bar 4 is F");

        int noteOns = 0;
        for (const auto& e : events)
            noteOns += (e.isOn && e.sample < barStartSample(4) - 1) ? 1 : 0;
        expectEquals(noteOns, 12, "Only chord notes, only on bar lines");

        // Each chord is released where the next one starts
        int offsAtBar2 = 0;
        for (const auto& e : events)
            if (!e.isOn && std::abs(e.sample - barStartSample(1)) <= 1)
                ++offsAtBar2;
        expectEquals(offsAtBar2, 3, "The C chord ends at bar 2");
    }

    void testSilentWhenTransportStopped()
    {
        beginTest("Silent while the transport is stopped");

        PlaybackHarness harness;
        int events = 0;
        for (int i = 0; i < 20; ++i)
            events += harness.processBlock().getNumEvents();
        expectEquals(events, 0);
    }

    void testStopReleasesNotes()
    {
        beginTest("Stopping the transport releases the sounding chord");

        PlaybackHarness harness;
        harness.play(6.0); // Into bar 2: G is sounding
        const auto events = harness.stop();

        std::vector<int> offs;
        for (const auto& e : events)
            if (!e.isOn)
                offs.push_back(e.note);
        std::sort(offs.begin(), offs.end());
        expectNotes(offs, { 67, 71, 74 }, "G chord released");
    }

    void testFourIsNotFive()
    {
        beginTest("IV is the fourth degree, not the fifth");

        PlaybackHarness harness;
        expect(select(harness.processor, "genre", "Rock"), "Has a genre choice named Rock");
        const auto events = harness.play(16.0);

        // Rock: I-IV-V-I
        expectNotes(notesStartingAt(events, barStartSample(1)), { 65, 69, 72 }, "IV in C is F");
        expectNotes(notesStartingAt(events, barStartSample(2)), { 67, 71, 74 }, "V in C is G");
    }

    void testKeyAndMode()
    {
        beginTest("Key and mode transpose the progression");

        PlaybackHarness harness;
        expect(select(harness.processor, "key", "A"), "Has a key choice named A");
        expect(select(harness.processor, "mode", "Minor"), "Has a mode choice named Minor");
        expect(select(harness.processor, "genre", "EDM"), "Has a genre choice named EDM");
        const auto events = harness.play(16.0);

        // EDM: i-VI-III-VII in A minor is Am-F-C-G
        expectNotes(notesStartingAt(events, barStartSample(0)), { 69, 72, 76 }, "i is Am");
        expectNotes(notesStartingAt(events, barStartSample(1)), { 77, 81, 84 }, "VI is F");
        expectNotes(notesStartingAt(events, barStartSample(2)), { 72, 76, 79 }, "III is C");
        expectNotes(notesStartingAt(events, barStartSample(3)), { 79, 83, 86 }, "VII is G");
    }

    void testStateRoundTrip()
    {
        beginTest("Key, mode and genre are saved and restored");

        juce::MemoryBlock state;
        {
            ChordGeniusProcessor processor;
            expect(select(processor, "key", "D"));
            expect(select(processor, "mode", "Minor"));
            expect(select(processor, "genre", "Jazz"));
            processor.getStateInformation(state);
        }

        expect(state.getSize() > 0, "Something was saved");

        PlaybackHarness harness;
        harness.processor.setStateInformation(state.getData(), (int) state.getSize());

        auto* key = findChoice(harness.processor, "key");
        auto* mode = findChoice(harness.processor, "mode");
        auto* genre = findChoice(harness.processor, "genre");
        expect(key != nullptr && key->getCurrentChoiceName() == "D", "Key restored");
        expect(mode != nullptr && mode->getCurrentChoiceName() == "Minor", "Mode restored");
        expect(genre != nullptr && genre->getCurrentChoiceName() == "Jazz", "Genre restored");

        // Jazz: ii-V-I-vi; in D minor ii is Em
        const auto events = harness.play(4.0);
        expectNotes(notesStartingAt(events, barStartSample(0)), { 64, 67, 71 }, "Restored progression plays");
    }

    void testIgnoresForeignState()
    {
        beginTest("Ignores a state it cannot read");

        ChordGeniusProcessor processor;
        expect(select(processor, "key", "E"));

        const char junk[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
        processor.setStateInformation(junk, sizeof(junk));
        processor.setStateInformation(nullptr, 0);

        juce::XmlElement other("SomeOtherPlugin");
        juce::MemoryBlock otherState;
        juce::AudioProcessor::copyXmlToBinary(other, otherState);
        processor.setStateInformation(otherState.getData(), (int) otherState.getSize());

        auto* key = findChoice(processor, "key");
        expect(key != nullptr && key->getCurrentChoiceName() == "E", "Key unchanged");
    }
};

static ChordGeniusTests chordGeniusTests;
