/**
 * AI Producer Tests
 *
 * Tests for the Track Sketch MIDI generator (plugins/AIProducer):
 * - Writing: every genre and length gives a well-formed track in key
 * - Sequencing: the generated track is time-ordered, and block-by-block
 *   playback emits every event exactly once
 * - Playback: the processor follows the host transport (start, stop, seek)
 * - Controls: changing them rebuilds the track, and they are saved and loaded
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <map>
#include <set>

// Include AI Producer plugin headers
#include "../../plugins/AIProducer/Source/MIDIGenerator.h"
#include "../../plugins/AIProducer/Source/MusicTheory.h"
#include "../../plugins/AIProducer/Source/Styles.h"
#include "../../plugins/AIProducer/Source/PluginProcessor.h"

namespace
{
    constexpr double testSampleRate = 48000.0;
    constexpr int testBlockSize = 512;

    constexpr int drumChannel = 10;
    constexpr int kickNote = 36;

    // Four bars of drop, drums only, so the counts below follow from the
    // house drum pattern: per bar 4 kicks, 2 claps, 12 closed and 4 open hats
    TrackStructure makeDrumStructure()
    {
        TrackStructure structure;
        structure.genre = "House";
        structure.bpm = 120;
        structure.sections.push_back({"drop", 0, 4, 10});
        structure.hasBass = false;
        structure.hasChords = false;
        structure.hasLead = false;
        return structure;
    }

    constexpr int drumNotesPerBar = 4 + 2 + 12 + 4;

    // Returns an empty string, or what is wrong with the sequence
    juce::String checkSequence(const MIDIGenerator::Sequence& sequence)
    {
        std::map<std::pair<int, int>, bool> held;
        double previous = 0.0;

        for (const auto& event : sequence.events)
        {
            if (event.time < previous) return "events out of order";
            previous = event.time;

            if (event.noteNumber < 0 || event.noteNumber > 127) return "note out of range";
            if (event.channel < 1 || event.channel > 16) return "channel out of range";

            auto& isHeld = held[{event.channel, event.noteNumber}];
            if (event.isNoteOn)
            {
                if (isHeld) return "note " + juce::String(event.noteNumber) + " on channel "
                                   + juce::String(event.channel) + " starts again before it ends";
                if (event.velocity < 1 || event.velocity > 127) return "velocity out of range";
                isHeld = true;
            }
            else
            {
                if (! isHeld) return "note-off without a note-on";
                isHeld = false;
            }
        }

        for (const auto& note : held)
            if (note.second) return "a note never ends";

        return {};
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

    void setParameter(AIProducerProcessor& processor, const char* id, float value)
    {
        auto* param = processor.getValueTreeState().getParameter(id);
        param->setValueNotifyingHost(param->convertTo0to1(value));
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
        // An 8-bar loop: one drop section, so every part plays from bar 1
        PlaybackHarness()
        {
            setParameter(processor, "length", 0.0f);
            processor.regenerateIfChanged();
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
class AIProducerWritingTests : public juce::UnitTest
{
public:
    AIProducerWritingTests()
        : juce::UnitTest("Track Sketch Writing", "Plugins")
    {
    }

    void runTest() override
    {
        testProgressionsAreRead();
        testScaleDegrees();
        testEveryGenreAndLengthIsWellFormed();
        testEverythingIsInKey();
        testSameSettingsGiveTheSameTrack();
        testVariationChangesTheTrack();
        testTheArrangementBuilds();
    }

private:
    void testProgressionsAreRead()
    {
        beginTest("Roman numeral progressions are read as scale degrees");

        expect(MusicTheory::parseProgression("i-VI-III-VII") == std::vector<int>{0, 5, 2, 6}, "i-VI-III-VII");
        expect(MusicTheory::parseProgression("I-V-vi-IV") == std::vector<int>{0, 4, 5, 3}, "I-V-vi-IV");
        expect(MusicTheory::parseProgression("ii-x-") == std::vector<int>{1, 0}, "Unknown numerals count as the tonic");
    }

    void testScaleDegrees()
    {
        beginTest("Scale degrees continue into the octaves above and below");

        expectEquals(MusicTheory::degreeToPitch(57, true, 0), 57, "A");
        expectEquals(MusicTheory::degreeToPitch(57, true, 2), 60, "C, the minor third");
        expectEquals(MusicTheory::degreeToPitch(57, true, 7), 69, "A an octave up");
        expectEquals(MusicTheory::degreeToPitch(57, true, -1), 55, "G below");
        expectEquals(MusicTheory::degreeToPitch(60, false, -7), 48, "C an octave down");
    }

    void testEveryGenreAndLengthIsWellFormed()
    {
        beginTest("Every genre and length gives a well-formed track");

        const int expectedBars[] = {8, 32, 72};

        for (int genre = 0; genre < static_cast<int>(Styles::genres().size()); ++genre)
        {
            for (int length = 0; length < 3; ++length)
            {
                Styles::Settings settings;
                settings.genre = genre;
                settings.length = length;
                const auto structure = Styles::build(settings);
                const auto sequence = MIDIGenerator::generateFromStructure(structure);
                const auto name = structure.genre + ", length " + std::to_string(length);

                expectEquals(sequence->totalBars, expectedBars[length], name);
                expectEquals(sequence->bpm, static_cast<double>(Styles::genres()[static_cast<size_t>(genre)].bpm), name);
                expect(! sequence->events.empty(), name + " has notes");
                expectEquals(checkSequence(*sequence), juce::String(), name);
            }
        }
    }

    void testEverythingIsInKey()
    {
        beginTest("Bass, chords and lead stay in the key and scale chosen");

        for (int key = 0; key < 12; ++key)
        {
            for (bool minor : {true, false})
            {
                Styles::Settings settings;
                settings.keyRoot = key;
                settings.minor = minor;
                settings.drums = false;
                const auto sequence = MIDIGenerator::generateFromStructure(Styles::build(settings));

                const auto& intervals = MusicTheory::scaleIntervals(minor);
                int outOfKey = 0;
                for (const auto& event : sequence->events)
                    if (event.isNoteOn
                        && std::find(intervals.begin(), intervals.end(), ((event.noteNumber - key) % 12 + 12) % 12) == intervals.end())
                        ++outOfKey;

                expectEquals(outOfKey, 0, Styles::keyNames()[static_cast<size_t>(key)] + (minor ? " minor" : " major"));
            }
        }
    }

    void testSameSettingsGiveTheSameTrack()
    {
        beginTest("The same settings always give the same track");

        Styles::Settings settings;
        settings.genre = 2;
        settings.variation = 7;
        const auto first = MIDIGenerator::generateFromStructure(Styles::build(settings));
        const auto second = MIDIGenerator::generateFromStructure(Styles::build(settings));

        bool same = first->events.size() == second->events.size();
        for (size_t i = 0; same && i < first->events.size(); ++i)
            same = first->events[i].time == second->events[i].time
                   && first->events[i].noteNumber == second->events[i].noteNumber;

        expect(same, "Reloading a session must play what was saved");
    }

    void testVariationChangesTheTrack()
    {
        beginTest("A new variation changes the chords and the lead");

        auto notesOn = [](int variation, int channel) {
            Styles::Settings settings;
            settings.variation = variation;
            const auto sequence = MIDIGenerator::generateFromStructure(Styles::build(settings));
            std::vector<int> notes;
            for (const auto& event : sequence->events)
                if (event.isNoteOn && event.channel == channel)
                    notes.push_back(event.noteNumber);
            return notes;
        };

        expect(notesOn(1, 2) != notesOn(2, 2), "Chords");
        expect(notesOn(1, 3) != notesOn(2, 3), "Lead");
    }

    void testTheArrangementBuilds()
    {
        beginTest("The full song builds: parts come in and drop out by section");

        const auto sequence = MIDIGenerator::generateFromStructure(Styles::build(Styles::Settings()));

        // Note-ons per channel (and kicks) in a range of bars
        auto count = [&](int fromBar, int toBar, int channel, int note = -1) {
            int n = 0;
            for (const auto& event : sequence->events)
                if (event.isNoteOn && event.channel == channel && (note < 0 || event.noteNumber == note)
                    && event.time >= fromBar * 4.0 && event.time < toBar * 4.0)
                    ++n;
            return n;
        };
        constexpr int bass = 1, chords = 2, lead = 3;

        expect(count(0, 8, drumChannel, kickNote) > 0, "Intro: kick, for DJs to mix in");
        expectEquals(count(0, 8, bass) + count(0, 8, lead), 0, "Intro: no bass or lead yet");
        expect(count(8, 16, bass) > 0, "Build: the bass comes in");
        for (int channel : {bass, chords, lead})
            expect(count(16, 32, channel) > 0, "Drop: every part plays");
        expect(count(16, 32, drumChannel, kickNote) > 0, "Drop: kick");
        expectEquals(count(32, 40, drumChannel, kickNote), 0, "Breakdown: no kick, so the next drop hits");
        expect(count(32, 40, lead) > 0, "Breakdown: the lead over the chords");
        expectEquals(count(64, 72, drumChannel, kickNote) + count(64, 72, bass), 0, "Outro: kick and bass gone");
    }
};

static AIProducerWritingTests aiProducerWritingTests;

//==============================================================================
class AIProducerSequencingTests : public juce::UnitTest
{
public:
    AIProducerSequencingTests()
        : juce::UnitTest("Track Sketch Sequencing", "Plugins")
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

        auto sequence = MIDIGenerator::generateFromStructure(makeDrumStructure());

        // Plus a crash at the top of the drop
        const int expectedNotes = 4 * drumNotesPerBar + 1;
        expectEquals(static_cast<int>(sequence->events.size()), expectedNotes * 2,
                     "Each note should produce one note-on and one note-off");
        expectEquals(sequence->totalBars, 4, "Sections cover four bars");
        expectEquals(checkSequence(*sequence), juce::String());
    }

    void testBlockPlaybackEmitsEverythingOnce()
    {
        beginTest("Block-by-block playback emits every event exactly once");

        auto sequence = MIDIGenerator::generateFromStructure(makeDrumStructure());
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

        auto sequence = MIDIGenerator::generateFromStructure(makeDrumStructure());
        MIDIGenerator::HeldNotes held;
        const double blockPpq = testBlockSize * ppqPerSample;

        // First block: kick, closed hat and crash start on beat 1.
        // None of them is shorter than a block, so nothing ends yet.
        juce::MidiBuffer firstBlock;
        MIDIGenerator::renderRange(*sequence, firstBlock, held, 0.0, blockPpq,
                                   0.0, ppqPerSample, testBlockSize);
        const auto first = countMidi(firstBlock);

        expectEquals(first.noteOns, 3, "Three notes start on the first beat");
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

        auto sequence = MIDIGenerator::generateFromStructure(makeDrumStructure());
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
        : juce::UnitTest("Track Sketch Playback", "Plugins")
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

        MidiCounts total;
        for (int i = 0; i < 10; ++i)
            total += countMidi(harness.processBlock());

        expectEquals(total.noteOns + total.noteOffs, 0, "No MIDI should be sent until the host plays");
    }

    void testPlaysInTimeWithTheHost()
    {
        beginTest("Plays the track in time with the host");

        PlaybackHarness harness;
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
        harness.playHead.playing = true;

        expectEquals(countMidi(harness.processBlock()).kicks, 1, "Kick on the first beat");

        // Ten blocks end at sample 5120; the next 16th note at 128 BPM is at 5625
        MidiCounts following;
        for (int i = 0; i < 9; ++i)
            following += countMidi(harness.processBlock());

        expectEquals(following.noteOns, 0, "Nothing else starts before the next 16th note");
    }

    void testSeekingReleasesHeldNotes()
    {
        beginTest("Seeking releases held notes and resumes from the new position");

        PlaybackHarness harness;
        harness.playHead.playing = true;

        MidiCounts beforeSeek;
        for (int i = 0; i < 5; ++i)
            beforeSeek += countMidi(harness.processBlock());

        // Jump to the downbeat of bar 5
        harness.playHead.ppq = 16.0;
        const auto afterSeek = countMidi(harness.processBlock());

        expectEquals(afterSeek.noteOffs, beforeSeek.noteOns - beforeSeek.noteOffs,
                     "Notes left sounding before the seek should be released");
        expectEquals(afterSeek.kicks, 1, "Playback should resume on the new downbeat");
    }

    void testReplacingTheTrackLeavesNoStuckNotes()
    {
        beginTest("Changing a control during playback leaves no stuck notes");

        PlaybackHarness harness;
        harness.playHead.playing = true;

        MidiCounts total;
        for (int i = 0; i < 5; ++i)
            total += countMidi(harness.processBlock());

        // A new variation while notes are sounding
        harness.processor.nextVariation();
        for (int i = 0; i < 5; ++i)
            total += countMidi(harness.processBlock());

        harness.playHead.playing = false;
        total += countMidi(harness.processBlock());

        expectEquals(total.noteOffs, total.noteOns, "Every note should be released");
    }
};

static AIProducerPlaybackTests aiProducerPlaybackTests;

//==============================================================================
class AIProducerControlTests : public juce::UnitTest
{
public:
    AIProducerControlTests()
        : juce::UnitTest("Track Sketch Controls", "Plugins")
    {
    }

    void runTest() override
    {
        testATrackIsReadyOnLoad();
        testControlsRebuildTheTrack();
        testNextVariationWraps();
        testStateRoundTrip();
        testDamagedStateIsIgnored();
    }

private:
    void testATrackIsReadyOnLoad()
    {
        beginTest("A track is ready as soon as the plugin loads");

        AIProducerProcessor processor;
        expectEquals(processor.getDescription(), juce::String("House in A minor, 72 bars at 124 BPM"));
    }

    void testControlsRebuildTheTrack()
    {
        beginTest("Changing a control rebuilds the track");

        AIProducerProcessor processor;
        setParameter(processor, "genre", 5.0f);
        setParameter(processor, "key", 2.0f);
        setParameter(processor, "scale", 1.0f);
        setParameter(processor, "length", 1.0f);
        processor.regenerateIfChanged();

        expectEquals(processor.getDescription(), juce::String("Drum & Bass in D major, 32 bars at 174 BPM"));
    }

    void testNextVariationWraps()
    {
        beginTest("New Variation counts up and wraps back to 1");

        AIProducerProcessor processor;
        auto* variation = processor.getValueTreeState().getRawParameterValue("variation");

        processor.nextVariation();
        expectEquals(juce::roundToInt(variation->load()), 2);

        setParameter(processor, "variation", 100.0f);
        processor.nextVariation();
        expectEquals(juce::roundToInt(variation->load()), 1);
    }

    void testStateRoundTrip()
    {
        beginTest("Controls are saved and loaded with the session");

        AIProducerProcessor original;
        setParameter(original, "genre", 3.0f);
        setParameter(original, "variation", 42.0f);
        setParameter(original, "lead", 0.0f);
        original.regenerateIfChanged();

        juce::MemoryBlock state;
        original.getStateInformation(state);

        AIProducerProcessor restored;
        restored.setStateInformation(state.getData(), static_cast<int>(state.getSize()));

        auto& params = restored.getValueTreeState();
        expectEquals(juce::roundToInt(params.getRawParameterValue("genre")->load()), 3);
        expectEquals(juce::roundToInt(params.getRawParameterValue("variation")->load()), 42);
        expectEquals(juce::roundToInt(params.getRawParameterValue("lead")->load()), 0);
        expectEquals(restored.getDescription(), original.getDescription(), "The loaded track is rebuilt at once");
    }

    void testDamagedStateIsIgnored()
    {
        beginTest("A damaged or foreign state is ignored");

        AIProducerProcessor processor;

        const char garbage[] = "not a saved state";
        processor.setStateInformation(garbage, sizeof(garbage));
        processor.setStateInformation(nullptr, 0);

        juce::MemoryBlock foreign;
        juce::XmlElement other("SOMETHING_ELSE");
        juce::AudioProcessor::copyXmlToBinary(other, foreign);
        processor.setStateInformation(foreign.getData(), static_cast<int>(foreign.getSize()));

        // Out-of-range values in our own format are clamped, not trusted
        juce::XmlElement wild("PARAMETERS");
        auto* genre = wild.createNewChildElement("PARAM");
        genre->setAttribute("id", "genre");
        genre->setAttribute("value", 999.0);
        auto* variation = wild.createNewChildElement("PARAM");
        variation->setAttribute("id", "variation");
        variation->setAttribute("value", -5.0);
        juce::MemoryBlock wildState;
        juce::AudioProcessor::copyXmlToBinary(wild, wildState);
        processor.setStateInformation(wildState.getData(), static_cast<int>(wildState.getSize()));

        auto& params = processor.getValueTreeState();
        const int genreIndex = juce::roundToInt(params.getRawParameterValue("genre")->load());
        const int variationValue = juce::roundToInt(params.getRawParameterValue("variation")->load());
        expect(genreIndex >= 0 && genreIndex < static_cast<int>(Styles::genres().size()), "Genre stays in range");
        expect(variationValue >= 1 && variationValue <= 100, "Variation stays in range");
        expect(processor.getDescription().isNotEmpty(), "A track is still ready");
    }
};

static AIProducerControlTests aiProducerControlTests;
