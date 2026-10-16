/**
 * ChordGenius Plugin Tests
 *
 * ChordGenius is a MIDI effect that plays the selected chord progression,
 * one chord per bar, in time with the host transport:
 * - Chords land on the bar grid and follow the progression
 * - Roman numerals map to the right scale degrees (IV is not V)
 * - Key and mode transpose the progression
 * - Stopping the transport releases what is sounding
 * - Seeks, loops (with blocks that cross the loop end), count-ins, meter
 *   changes, other sample rates and block sizes, and zero-length blocks
 * - No position, or bypass, releases what is sounding
 * - Key, mode and genre survive a save and reload
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <map>

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
            if (hasPpq)
                info.setPpqPosition(ppq);
            info.setBpm(bpm);
            info.setTimeSignature(TimeSignature { beatsPerBar, 4 });
            info.setIsLooping(looping);
            info.setLoopPoints(LoopPoints { loopStart, loopEnd });
            return info;
        }

        bool playing = false;
        bool hasPpq = true;
        double ppq = 0.0;
        double bpm = 120.0;
        int beatsPerBar = 4;
        bool looping = false;
        double loopStart = 0.0;
        double loopEnd = 0.0;
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
        explicit PlaybackHarness(double rate = testSampleRate, int size = testBlockSize)
            : sampleRate(rate), blockSize(size), audio(2, size)
        {
            processor.setPlayHead(&playHead);
            processor.prepareToPlay(sampleRate, blockSize);
        }

        // Processes enough blocks to cover the given number of quarter notes,
        // returning every note event with its time. Like Live, a looping
        // transport does not split a block at the loop end; the next block
        // starts back inside the loop.
        std::vector<NoteEvent> play(double quarterNotes)
        {
            playHead.playing = true;
            const double ppqPerBlock = blockSize * playHead.bpm / (60.0 * sampleRate);
            const int numBlocks = (int) std::ceil(quarterNotes / ppqPerBlock);

            std::vector<NoteEvent> events;
            for (int i = 0; i < numBlocks; ++i)
            {
                collect(processBlock(), events);
                const bool wasInLoop = playHead.ppq < playHead.loopEnd;
                playHead.ppq += ppqPerBlock;
                if (playHead.looping && wasInLoop && playHead.ppq >= playHead.loopEnd)
                    playHead.ppq += playHead.loopStart - playHead.loopEnd;
                samplesPlayed += blockSize;
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

        double sampleRate;
        int blockSize;
        ChordGeniusProcessor processor;
        FakePlayHead playHead;
        juce::AudioBuffer<float> audio;
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

    juce::int64 barStartSample(int bar, double bpm = 120.0, double sampleRate = testSampleRate)
    {
        return juce::roundToInt(bar * 4 * 60.0 / bpm * sampleRate);
    }

    int countNoteOns(const std::vector<NoteEvent>& events)
    {
        return (int) std::count_if(events.begin(), events.end(), [](const NoteEvent& e) { return e.isOn; });
    }

    // The notes switched on at each time, in order of time
    std::map<juce::int64, std::vector<int>> chordsByStart(const std::vector<NoteEvent>& events)
    {
        std::map<juce::int64, std::vector<int>> chords;
        for (const auto& e : events)
            if (e.isOn)
                chords[e.sample].push_back(e.note);
        for (auto& [sample, notes] : chords)
            std::sort(notes.begin(), notes.end());
        return chords;
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
        testSampleRatesAndBlockSizes();
        testCountIn();
        testSeek();
        testLoopWithoutSplitBlocks();
        testMissingPpq();
        testBypassReleasesNotes();
        testZeroLengthBlock();
        testMeterChange();
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

    void testSampleRatesAndBlockSizes()
    {
        beginTest("Chords land on the bar lines at any sample rate and block size");

        const std::pair<double, int> configs[] = { { 48000.0, 1 }, { 96000.0, 4096 }, { 44100.0, 4096 }, { 48000.0, 64 } };
        for (const auto& [rate, size] : configs)
        {
            const juce::String where = " at " + juce::String(rate) + " Hz, " + juce::String(size) + "-sample blocks";

            PlaybackHarness harness(rate, size);
            const auto events = harness.play(16.0);

            expectNotes(notesStartingAt(events, barStartSample(0, 120.0, rate)), { 60, 64, 67 }, "Bar 1 is C" + where);
            expectNotes(notesStartingAt(events, barStartSample(1, 120.0, rate)), { 67, 71, 74 }, "Bar 2 is G" + where);
            expectNotes(notesStartingAt(events, barStartSample(2, 120.0, rate)), { 69, 72, 76 }, "Bar 3 is Am" + where);
            expectNotes(notesStartingAt(events, barStartSample(3, 120.0, rate)), { 65, 69, 72 }, "Bar 4 is F" + where);

            int noteOns = 0;
            for (const auto& e : events)
                noteOns += (e.isOn && e.sample < barStartSample(4, 120.0, rate) - 1) ? 1 : 0;
            expectEquals(noteOns, 12, "Only chord notes, only on bar lines" + where);
        }
    }

    void testCountIn()
    {
        beginTest("Waits through a count-in for bar 1");

        PlaybackHarness harness;
        harness.playHead.ppq = -4.0; // One bar of count-in
        const auto events = harness.play(12.0);

        const auto bar1 = barStartSample(1);
        for (const auto& e : events)
            expect(e.sample >= bar1 - 1, "Nothing during the count-in");

        expectNotes(notesStartingAt(events, bar1), { 60, 64, 67 }, "Bar 1 is C, after the count-in");
        expectNotes(notesStartingAt(events, barStartSample(2)), { 67, 71, 74 }, "Bar 2 is G");
    }

    void testSeek()
    {
        beginTest("A seek releases the chord and plays the new bar's chord");

        PlaybackHarness harness;
        harness.play(6.0);              // Into bar 2: G is sounding
        harness.playHead.ppq = 13.0;    // Seek into bar 4
        const auto seekSample = harness.samplesPlayed;
        const auto events = harness.play(2.0);

        std::vector<int> offs;
        for (const auto& e : events)
            if (!e.isOn && e.sample == seekSample)
                offs.push_back(e.note);
        std::sort(offs.begin(), offs.end());
        expectNotes(offs, { 67, 71, 74 }, "G released at the seek");
        expectNotes(notesStartingAt(events, seekSample), { 65, 69, 72 }, "F plays at the seek");
        expectEquals(countNoteOns(events), 3, "Only the F chord");
    }

    void testLoopWithoutSplitBlocks()
    {
        beginTest("A loop plays each chord once per pass, even when a block crosses the loop end");

        PlaybackHarness harness;
        harness.playHead.looping = true;
        harness.playHead.loopStart = 0.0;
        harness.playHead.loopEnd = 8.0; // Two bars: C, G
        const auto events = harness.play(23.0); // Three passes

        juce::String sequence;
        bool allTriads = true;
        for (const auto& [sample, notes] : chordsByStart(events))
        {
            allTriads = allTriads && notes.size() == 3;
            sequence << (notes.front() == 60 ? "C" : notes.front() == 67 ? "G" : "?");
        }
        expect(allTriads, "Every chord starts on its own");
        expectEquals(sequence, juce::String("CGCGCG"), "Chords per pass");
    }

    void testMissingPpq()
    {
        beginTest("A playing transport with no musical position is treated as stopped");

        PlaybackHarness harness;
        harness.playHead.hasPpq = false;
        expectEquals((int) harness.play(8.0).size(), 0, "Nothing plays without a bar grid");

        PlaybackHarness sounding;
        sounding.play(6.0); // G is sounding
        sounding.playHead.hasPpq = false;
        std::vector<NoteEvent> events;
        sounding.collect(sounding.processBlock(), events);

        std::vector<int> offs;
        for (const auto& e : events)
            if (!e.isOn)
                offs.push_back(e.note);
        std::sort(offs.begin(), offs.end());
        expectNotes(offs, { 67, 71, 74 }, "G released when the position goes away");
        expectEquals(countNoteOns(events), 0, "Nothing new starts");
    }

    void testBypassReleasesNotes()
    {
        beginTest("Bypassing releases the sounding chord and passes MIDI through");

        PlaybackHarness harness;
        harness.play(6.0); // G is sounding

        juce::MidiBuffer midi;
        midi.addEvent(juce::MidiMessage::noteOn(2, 50, (juce::uint8) 90), 10);
        harness.processor.processBlockBypassed(harness.audio, midi);

        std::vector<int> offs;
        bool passedThrough = false;
        for (const auto metadata : midi)
        {
            const auto message = metadata.getMessage();
            if (message.isNoteOff())
                offs.push_back(message.getNoteNumber());
            passedThrough = passedThrough || (message.isNoteOn() && message.getNoteNumber() == 50);
        }
        std::sort(offs.begin(), offs.end());
        expectNotes(offs, { 67, 71, 74 }, "G released on bypass");
        expect(passedThrough, "Input MIDI passes through while bypassed");

        // Un-bypassed, the current bar's chord comes back in
        harness.playHead.ppq += harness.blockSize * harness.playHead.bpm / (60.0 * harness.sampleRate);
        harness.samplesPlayed += harness.blockSize;
        const auto resumeSample = harness.samplesPlayed;
        const auto events = harness.play(1.0);
        expectNotes(notesStartingAt(events, resumeSample), { 67, 71, 74 }, "G plays again after bypass");
    }

    void testZeroLengthBlock()
    {
        beginTest("Zero-length blocks emit nothing");

        PlaybackHarness harness;
        harness.playHead.playing = true;
        juce::AudioBuffer<float> empty(2, 0);

        // The position creeps across a bar line by less than the jump tolerance
        int eventsInEmptyBlocks = 0;
        for (double ppq : { 3.9996, 4.0004 })
        {
            harness.playHead.ppq = ppq;
            juce::MidiBuffer midi;
            harness.processor.processBlock(empty, midi);
            eventsInEmptyBlocks += midi.getNumEvents();
        }
        expectEquals(eventsInEmptyBlocks, 0);

        // The next real block comes in with bar 2's chord
        std::vector<NoteEvent> events;
        harness.collect(harness.processBlock(), events);
        expectNotes(notesStartingAt(events, 0), { 67, 71, 74 }, "G plays in the next block");
    }

    void testMeterChange()
    {
        beginTest("A shorter meter skips to its next bar line without a burst of chords");

        PlaybackHarness harness;
        auto events = harness.play(32.0); // Eight bars of 4/4
        harness.playHead.beatsPerBar = 3;
        const auto later = harness.play(6.0);
        events.insert(events.end(), later.begin(), later.end());

        bool allTriads = true;
        for (const auto& [sample, notes] : chordsByStart(events))
            allTriads = allTriads && notes.size() == 3;
        expect(allTriads, "One chord at a time");

        // In 3/4 counted from the start, the next bar line after PPQ 32 is bar 12 at PPQ 33
        const auto sampleAtPpq = [](double ppq) { return (juce::int64) juce::roundToInt(ppq * 0.5 * testSampleRate); };
        expectNotes(notesStartingAt(events, sampleAtPpq(33.0)), { 65, 69, 72 }, "Bar 12 is F");
        expectNotes(notesStartingAt(events, sampleAtPpq(36.0)), { 60, 64, 67 }, "Bar 13 is C");
    }
};

static ChordGeniusTests chordGeniusTests;
