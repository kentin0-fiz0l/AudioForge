/**
 * AI Producer Tests
 *
 * Tests for the AI Producer MIDI generator plugin:
 * - Sequencing: the generated track is time-ordered, and block-by-block
 *   playback emits every event exactly once
 * - Playback: the processor follows the host transport (start, stop, seek)
 */

#include <juce_audio_processors/juce_audio_processors.h>

// Include AI Producer plugin headers
#include "../../plugins/AIProducer/Source/MIDIGenerator.h"
#include "../../plugins/AIProducer/Source/PluginProcessor.h"

namespace
{
    constexpr double testSampleRate = 48000.0;
    constexpr int testBlockSize = 512;

    constexpr int drumChannel = 10;
    constexpr int kickNote = 36;

    // Two bars of intro, then two bars of drop (the only section with a lead)
    TrackStructure makeTestStructure()
    {
        TrackStructure structure;
        structure.genre = "House";
        structure.bpm = 120;
        structure.key = "Am";
        structure.scale = "Minor";
        structure.progression = "i-VI-III-VII";
        structure.sections.push_back({"intro", 0, 2, 3});
        structure.sections.push_back({"drop", 2, 2, 10});
        structure.hasDrums = true;
        structure.hasBass = true;
        structure.hasChords = true;
        structure.hasLead = true;
        structure.hasPads = false;
        return structure;
    }

    struct MidiCounts
    {
        int noteOns = 0;
        int noteOffs = 0;
        int kicks = 0;

        MidiCounts& operator+=(const MidiCounts& other)
        {
            noteOns += other.noteOns;
            noteOffs += other.noteOffs;
            kicks += other.kicks;
            return *this;
        }
    };

    MidiCounts countMidi(const juce::MidiBuffer& buffer)
    {
        MidiCounts counts;

        for (const auto metadata : buffer)
        {
            const auto message = metadata.getMessage();

            if (message.isNoteOn())
            {
                ++counts.noteOns;
                if (message.getChannel() == drumChannel && message.getNoteNumber() == kickNote)
                    ++counts.kicks;
            }
            else if (message.isNoteOff())
            {
                ++counts.noteOffs;
            }
        }

        return counts;
    }

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
            return info;
        }

        bool playing = false;
        double ppq = 0.0;
        double bpm = 128.0;
    };

    // A processor wired to a fake host transport that advances as blocks are processed
    struct PlaybackHarness
    {
        PlaybackHarness()
        {
            processor.setPlayHead(&playHead);
            processor.prepareToPlay(testSampleRate, testBlockSize);
        }

        double blockLengthInQuarterNotes() const
        {
            return testBlockSize * playHead.bpm / (60.0 * testSampleRate);
        }

        juce::MidiBuffer processBlock()
        {
            juce::MidiBuffer midi;
            audio.clear();
            processor.processBlock(audio, midi);

            if (playHead.playing)
                playHead.ppq += blockLengthInQuarterNotes();

            return midi;
        }

        AIProducerProcessor processor;
        FakePlayHead playHead;
        juce::AudioBuffer<float> audio { 2, testBlockSize };
    };
}

//==============================================================================
class AIProducerSequencingTests : public juce::UnitTest
{
public:
    AIProducerSequencingTests()
        : juce::UnitTest("AI Producer Sequencing", "Plugins")
    {
    }

    void runTest() override
    {
        testSequenceIsOrderedAndBalanced();
        testBlockPlaybackEmitsEverythingOnce();
        testNoteOffsArriveInLaterBlocks();
        testEventsLandOnTheRightSample();
        testHeldNotesAreReleased();
    }

private:
    // 120 BPM at 48 kHz: one quarter note is exactly 24000 samples
    static constexpr double ppqPerSample = 1.0 / 24000.0;

    void testSequenceIsOrderedAndBalanced()
    {
        beginTest("Generated sequence is time-ordered and balanced");

        auto sequence = MIDIGenerator::generateFromStructure(makeTestStructure());

        // Per bar: 4 kicks + 2 snares + 8 hats, 4 bass notes, 2 chord notes.
        // Plus one 8-note lead phrase in the two-bar drop.
        const int expectedNotes = 4 * (14 + 4 + 2) + 8;
        expectEquals(static_cast<int>(sequence->events.size()), expectedNotes * 2,
                     "Each note should produce one note-on and one note-off");
        expectEquals(sequence->totalBars, 4, "Sections cover four bars");

        int noteOns = 0;
        bool ordered = true;

        for (size_t i = 0; i < sequence->events.size(); ++i)
        {
            if (sequence->events[i].isNoteOn)
                ++noteOns;
            if (i > 0 && sequence->events[i].time < sequence->events[i - 1].time)
                ordered = false;
        }

        expect(ordered, "Events should be sorted by time");
        expectEquals(noteOns, expectedNotes, "Half of the events should be note-ons");
    }

    void testBlockPlaybackEmitsEverythingOnce()
    {
        beginTest("Block-by-block playback emits every event exactly once");

        auto sequence = MIDIGenerator::generateFromStructure(makeTestStructure());
        MIDIGenerator::HeldNotes held;
        MidiCounts total;

        // Play past the end of the four-bar track
        double fromPpq = 0.0;
        while (fromPpq < 17.0)
        {
            const double toPpq = fromPpq + testBlockSize * ppqPerSample;

            juce::MidiBuffer midi;
            MIDIGenerator::renderRange(*sequence, midi, held, fromPpq, toPpq,
                                       fromPpq, ppqPerSample, testBlockSize);
            total += countMidi(midi);
            fromPpq = toPpq;
        }

        const int expectedNotes = static_cast<int>(sequence->events.size()) / 2;
        expectEquals(total.noteOns, expectedNotes, "Every note-on should be played once");
        expectEquals(total.noteOffs, expectedNotes, "Every note-off should be played once");
        expectEquals(total.kicks, 16, "Four bars of four-on-the-floor");
        expect(held.isEmpty(), "Nothing should still be sounding after the track ends");
    }

    void testNoteOffsArriveInLaterBlocks()
    {
        beginTest("Note-offs arrive in a later block than their note-on");

        auto sequence = MIDIGenerator::generateFromStructure(makeTestStructure());
        MIDIGenerator::HeldNotes held;
        const double blockPpq = testBlockSize * ppqPerSample;

        // First block: kick, hat, bass and two chord notes start on beat 1.
        // None of them is shorter than a block, so nothing ends yet.
        juce::MidiBuffer firstBlock;
        MIDIGenerator::renderRange(*sequence, firstBlock, held, 0.0, blockPpq,
                                   0.0, ppqPerSample, testBlockSize);
        const auto first = countMidi(firstBlock);

        expectEquals(first.noteOns, 5, "Five notes start on the first beat");
        expectEquals(first.noteOffs, 0, "No note is shorter than one block");
        expect(! held.isEmpty(), "Notes should be sounding after the first block");

        // The kick and hat last 0.1 quarter notes, so their note-offs fall a
        // few blocks later; keep playing until just past that point.
        MidiCounts later;
        double fromPpq = blockPpq;
        while (fromPpq < 0.2)
        {
            const double toPpq = fromPpq + blockPpq;

            juce::MidiBuffer midi;
            MIDIGenerator::renderRange(*sequence, midi, held, fromPpq, toPpq,
                                       fromPpq, ppqPerSample, testBlockSize);
            later += countMidi(midi);
            fromPpq = toPpq;
        }

        expectEquals(later.noteOffs, 2, "Kick and hat note-offs should arrive in a later block");
        expectEquals(later.noteOns, 0, "The first beat should not be played again");
    }

    void testEventsLandOnTheRightSample()
    {
        beginTest("Events land on the right sample within a block");

        auto sequence = MIDIGenerator::generateFromStructure(makeTestStructure());
        MIDIGenerator::HeldNotes held;

        // Beat 2 is sample 24000, which is offset 448 into block 46
        const int block = 46;
        const double blockStartPpq = block * testBlockSize * ppqPerSample;
        const double blockEndPpq = (block + 1) * testBlockSize * ppqPerSample;

        juce::MidiBuffer midi;
        MIDIGenerator::renderRange(*sequence, midi, held, blockStartPpq, blockEndPpq,
                                   blockStartPpq, ppqPerSample, testBlockSize);

        int kickPosition = -1;
        for (const auto metadata : midi)
        {
            const auto message = metadata.getMessage();
            if (message.isNoteOn() && message.getChannel() == drumChannel && message.getNoteNumber() == kickNote)
                kickPosition = metadata.samplePosition;
        }

        expectEquals(kickPosition, 24000 - block * testBlockSize, "Kick on beat 2 should be sample-accurate");
    }

    void testHeldNotesAreReleased()
    {
        beginTest("Held notes are released on request");

        MIDIGenerator::HeldNotes held;
        expect(held.isEmpty(), "Nothing is held initially");

        held.noteOn(1, 60);
        held.noteOn(10, 36);
        held.noteOn(16, 127);
        held.noteOff(16, 127);

        juce::MidiBuffer midi;
        held.releaseAll(midi, 0);

        expectEquals(countMidi(midi).noteOffs, 2, "One note-off per sounding note");
        expect(held.isEmpty(), "Nothing is held after releasing");
    }
};

static AIProducerSequencingTests aiProducerSequencingTests;

//==============================================================================
class AIProducerPlaybackTests : public juce::UnitTest
{
public:
    AIProducerPlaybackTests()
        : juce::UnitTest("AI Producer Playback", "Plugins")
    {
    }

    void runTest() override
    {
        testSilentWhileStopped();
        testPlaysInTimeWithTheHost();
        testFirstBeatIsNotRepeated();
        testSeekingReleasesHeldNotes();
        testReplacingTheTrackLeavesNoStuckNotes();
    }

private:
    void testSilentWhileStopped()
    {
        beginTest("Silent while the transport is stopped");

        PlaybackHarness harness;
        harness.processor.generateTrack("house");

        MidiCounts total;
        for (int i = 0; i < 10; ++i)
            total += countMidi(harness.processBlock());

        expectEquals(total.noteOns + total.noteOffs, 0, "No MIDI should be sent until the host plays");
    }

    void testPlaysInTimeWithTheHost()
    {
        beginTest("Plays the generated track in time with the host");

        PlaybackHarness harness;
        harness.processor.generateTrack("house");
        harness.playHead.playing = true;

        // Play the first two bars, stopping short of bar 3's downbeat
        MidiCounts total;
        while (harness.playHead.ppq + harness.blockLengthInQuarterNotes() < 7.9)
            total += countMidi(harness.processBlock());

        expectEquals(total.kicks, 8, "Two bars of four-on-the-floor");

        // Stopping must release whatever is still sounding
        harness.playHead.playing = false;
        total += countMidi(harness.processBlock());

        expect(total.noteOns > 0, "Notes should have been played");
        expectEquals(total.noteOffs, total.noteOns, "Every note should be released once the transport stops");
    }

    void testFirstBeatIsNotRepeated()
    {
        beginTest("First beat is played once, not every block");

        PlaybackHarness harness;
        harness.processor.generateTrack("house");
        harness.playHead.playing = true;

        expectEquals(countMidi(harness.processBlock()).kicks, 1, "Kick on the first beat");

        MidiCounts following;
        for (int i = 0; i < 10; ++i)
            following += countMidi(harness.processBlock());

        expectEquals(following.noteOns, 0, "Nothing else starts before the next 8th note");
    }

    void testSeekingReleasesHeldNotes()
    {
        beginTest("Seeking releases held notes and resumes from the new position");

        PlaybackHarness harness;
        harness.processor.generateTrack("house");
        harness.playHead.playing = true;

        MidiCounts beforeSeek;
        for (int i = 0; i < 5; ++i)
            beforeSeek += countMidi(harness.processBlock());

        // Jump to the downbeat of bar 17 (the first drop)
        harness.playHead.ppq = 64.0;
        const auto afterSeek = countMidi(harness.processBlock());

        expectEquals(afterSeek.noteOffs, beforeSeek.noteOns - beforeSeek.noteOffs,
                     "Notes left sounding before the seek should be released");
        expectEquals(afterSeek.kicks, 1, "Playback should resume on the new downbeat");
    }

    void testReplacingTheTrackLeavesNoStuckNotes()
    {
        beginTest("Replacing the track during playback leaves no stuck notes");

        PlaybackHarness harness;
        harness.processor.generateTrack("house");
        harness.playHead.playing = true;

        MidiCounts total;
        for (int i = 0; i < 5; ++i)
            total += countMidi(harness.processBlock());

        // Generate again while notes are sounding
        harness.processor.generateTrack("house");
        for (int i = 0; i < 5; ++i)
            total += countMidi(harness.processBlock());

        harness.playHead.playing = false;
        total += countMidi(harness.processBlock());

        expectEquals(total.noteOffs, total.noteOns, "Every note should be released");
    }
};

static AIProducerPlaybackTests aiProducerPlaybackTests;
