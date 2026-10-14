/**
 * MIDI Groove Quantizer Tests
 *
 * Behaviour tests through the processor's public API:
 * - Notes move to the grid even when the grid line is in a later block
 * - The grid follows the host's beat (PPQ) position, not the sample counter
 * - Note-offs keep each note's length
 * - The lookahead is reported as latency
 * - A flood of notes never loses a note-off (bounded queue)
 * - With the transport stopped, MIDI passes straight through
 * - A repeated note never has its new note-on cut by the previous note-off
 * - Same-pitch notes always come out as on, off, on, off, even when quantizing
 *   moves a later note-on before an earlier one
 * - A host tempo of 0 or below 1 BPM is treated as no tempo
 * - Stopping never releases a zero-length note
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include "../../plugins/MIDIGrooveQuantizer/Source/PluginProcessor.h"

namespace
{
    constexpr double sampleRate = 48000.0;
    constexpr int blockSize = 512;
    constexpr double bpm = 120.0;                        // quarter note = 24000 samples
    constexpr int64_t sixteenth = 6000;                  // samples per 1/16 at 120 BPM, 48 kHz
    constexpr double samplesPerBeat = sampleRate * 60.0 / bpm;

    class FakePlayHead : public juce::AudioPlayHead
    {
    public:
        juce::Optional<PositionInfo> getPosition() const override
        {
            PositionInfo info;
            info.setIsPlaying(playing);
            info.setBpm(tempo);
            info.setPpqPosition(ppq);
            info.setTimeInSamples(timeInSamples);
            return info;
        }

        bool playing = true;
        double tempo = bpm;
        double ppq = 0.0;
        int64_t timeInSamples = 0;
    };

    struct OutEvent
    {
        int64_t time;
        juce::MidiMessage message;
    };

    struct Harness
    {
        explicit Harness(double startPpq = 0.0)
        {
            playHead.ppq = startPpq;
            processor.setPlayHead(&playHead);
            processor.prepareToPlay(sampleRate, blockSize);
        }

        void setParam(const juce::String& id, float value)
        {
            auto* p = processor.getAPVTS().getParameter(id);
            p->setValueNotifyingHost(p->convertTo0to1(value));
        }

        // Feeds each input event in the block that contains it, runs numBlocks blocks,
        // and returns every output event with its absolute sample time. Successive
        // calls continue the same timeline.
        std::vector<OutEvent> run(const std::vector<OutEvent>& input, int numBlocks)
        {
            std::vector<OutEvent> out;
            for (int b = 0; b < numBlocks; ++b, ++blocksDone)
            {
                const int64_t blockStart = blocksDone * blockSize;
                juce::MidiBuffer midi;
                for (const auto& e : input)
                    if (e.time >= blockStart && e.time < blockStart + blockSize)
                        midi.addEvent(e.message, (int) (e.time - blockStart));

                if (bypassed)
                    processor.processBlockBypassed(audio, midi);
                else
                    processor.processBlock(audio, midi);

                for (const auto m : midi)
                    out.push_back({ blockStart + m.samplePosition, m.getMessage() });

                playHead.timeInSamples += blockSize;
                if (playHead.playing)
                    playHead.ppq += blockSize / samplesPerBeat;
            }
            return out;
        }

        MIDIGrooveQuantizerProcessor processor;
        FakePlayHead playHead;
        juce::AudioBuffer<float> audio { 2, blockSize };
        int64_t blocksDone = 0;
        bool bypassed = false;
    };

    juce::MidiMessage on(int note) { return juce::MidiMessage::noteOn(1, note, (juce::uint8) 100); }
    juce::MidiMessage off(int note) { return juce::MidiMessage::noteOff(1, note); }

    int64_t timeOf(const std::vector<OutEvent>& out, int note, bool noteOn)
    {
        for (const auto& e : out)
            if (e.message.getNoteNumber() == note
                && (noteOn ? e.message.isNoteOn() : e.message.isNoteOff()))
                return e.time;
        return -1;
    }
}

class MIDIGrooveQuantizerTests : public juce::UnitTest
{
public:
    MIDIGrooveQuantizerTests()
        : juce::UnitTest("MIDI Groove Quantizer", "MIDIGrooveQuantizer")
    {
    }

    void runTest() override
    {
        testNotesReachGridInLaterBlock();
        testGridFollowsHostPpq();
        testNoteOffsKeepLength();
        testZeroStrengthOnlyDelays();
        testLatencyReported();
        testFloodKeepsNoteOffs();
        testStoppedTransportPassesThrough();
        testRepeatedNoteNotCutByPreviousNoteOff();
        testSwappedRepeatedNotesStayValid();
        testZeroTempoPassesThrough();
        testPassThroughNoteOnClearsShift();
        testStopDropsNotesNotYetStarted();
        testBypassReleasesQueuedNotes();
    }

private:
    void testNotesReachGridInLaterBlock()
    {
        beginTest("Early and late notes land on the grid line, across block boundaries");

        Harness h;
        // Grid line at 6000 (block 11). One note 1000 samples early (block 9), one 1000 late.
        auto out = h.run({ { 5000, on(60) }, { 7000, on(62) } }, 40);
        const int64_t latency = h.processor.getLatencySamples();

        expectEquals(timeOf(out, 60, true), sixteenth + latency, "early note lands on the grid");
        expectEquals(timeOf(out, 62, true), sixteenth + latency, "late note lands on the grid");
    }

    void testGridFollowsHostPpq()
    {
        beginTest("Grid is counted from the host beat position");

        // The host starts at 1/8 of a beat (3000 samples past a grid line) while
        // timeInSamples is 0, so the sample counter and the beat grid disagree.
        Harness h(0.125);
        // At sample 2000 the beat position is 0.2083 -> nearest 1/16 is beat 0.25,
        // which is 3000 samples after the start.
        auto out = h.run({ { 2000, on(60) } }, 40);
        const int64_t latency = h.processor.getLatencySamples();

        expectEquals(timeOf(out, 60, true), (int64_t) 3000 + latency);
    }

    void testNoteOffsKeepLength()
    {
        beginTest("Note-offs move with their note-on, keeping the note length");

        Harness h;
        auto out = h.run({ { 5000, on(60) }, { 8000, off(60) } }, 40);
        const int64_t latency = h.processor.getLatencySamples();

        expectEquals(timeOf(out, 60, true), sixteenth + latency);
        expectEquals(timeOf(out, 60, false), (int64_t) 9000 + latency, "length stays 3000 samples");
    }

    void testZeroStrengthOnlyDelays()
    {
        beginTest("Strength 0 keeps the original timing (plus the reported latency)");

        Harness h;
        h.setParam("quantizeStrength", 0.0f);
        auto out = h.run({ { 5000, on(60) },
                           { 5100, juce::MidiMessage::controllerEvent(1, 64, 127) },
                           { 8000, off(60) } }, 40);
        const int64_t latency = h.processor.getLatencySamples();

        expectEquals((int) out.size(), 3);
        expectEquals(timeOf(out, 60, true), (int64_t) 5000 + latency);
        expectEquals(timeOf(out, 60, false), (int64_t) 8000 + latency);
        for (const auto& e : out)
            if (e.message.isController())
                expectEquals(e.time, (int64_t) 5100 + latency, "CCs stay in step with the notes");
    }

    void testLatencyReported()
    {
        beginTest("Lookahead is reported as latency and covers half a grid step");

        Harness h;
        h.run({}, 2);
        expect(h.processor.getLatencySamples() >= sixteenth / 2,
               "latency " + juce::String(h.processor.getLatencySamples()));

        Harness quarter;
        quarter.setParam("gridResolution", 0.0f);
        quarter.run({}, 2);
        expect(quarter.processor.getLatencySamples() >= 4 * sixteenth / 2,
               "1/4 grid latency " + juce::String(quarter.processor.getLatencySamples()));
    }

    void testFloodKeepsNoteOffs()
    {
        beginTest("A flood of notes never loses a note-off");

        Harness h;
        std::vector<OutEvent> input;
        constexpr int numNotes = 3000;
        for (int i = 0; i < numNotes; ++i)
        {
            const int64_t t = (int64_t) i * 4;
            input.push_back({ t, on(i % 128) });
            input.push_back({ t + 2, off(i % 128) });
        }

        auto out = h.run(input, 100);
        int ons = 0, offs = 0;
        for (const auto& e : out)
        {
            ons += e.message.isNoteOn() ? 1 : 0;
            offs += e.message.isNoteOff() ? 1 : 0;
        }
        expectEquals(ons, numNotes);
        expectEquals(offs, numNotes);
    }

    void testStoppedTransportPassesThrough()
    {
        beginTest("Stopped transport passes MIDI through unchanged");

        Harness h;
        h.playHead.playing = false;
        auto out = h.run({ { 5000, on(60) }, { 5300, off(60) } }, 20);

        expectEquals(timeOf(out, 60, true), (int64_t) 5000);
        expectEquals(timeOf(out, 60, false), (int64_t) 5300);
    }

    void testRepeatedNoteNotCutByPreviousNoteOff()
    {
        beginTest("A repeated note is not cut short by the previous note's note-off");

        // First A is late (4000 -> 6000), so its note-off moves 11800 -> 13800.
        // Second A is early (14000 -> 12000), before that note-off.
        Harness h;
        auto out = h.run({ { 4000, on(60) }, { 11800, off(60) },
                           { 14000, on(60) }, { 17000, off(60) } }, 60);
        const int64_t latency = h.processor.getLatencySamples();

        std::vector<OutEvent> notes;
        for (const auto& e : out)
            if (e.message.isNoteOnOrOff() && e.message.getNoteNumber() == 60)
                notes.push_back(e);

        expectEquals((int) notes.size(), 4);
        if (notes.size() != 4)
            return;
        expect(notes[0].message.isNoteOn() && notes[1].message.isNoteOff()
               && notes[2].message.isNoteOn() && notes[3].message.isNoteOff(),
               "order is on, off, on, off");
        expectEquals(notes[2].time, 2 * sixteenth + latency, "second note still on the grid");
        expect(notes[1].time <= notes[2].time, "first note-off comes before the second note-on");
        expectEquals(notes[3].time, (int64_t) 15000 + latency, "second note keeps its length");
    }

    void testSwappedRepeatedNotesStayValid()
    {
        beginTest("Same-pitch notes stay on, off, on, off when a later note-on is moved before an earlier one");

        // 1/4 grid. The host loops from beat 3.55 back to beat 0 after the first block.
        // The first A (beat 3.554) moves ~10700 samples later, to beat 4. The second A,
        // played after it at beat 0.004, moves 88 samples earlier, to beat 0: ahead of the first.
        Harness h(3.55);
        h.setParam("gridResolution", 0.0f);
        auto out = h.run({ { 100, on(60) }, { 300, off(60) } }, 1);
        h.playHead.ppq = 0.0;
        auto rest = h.run({ { 600, on(60) }, { 800, off(60) } }, 60);
        out.insert(out.end(), rest.begin(), rest.end());

        std::vector<OutEvent> notes;
        for (const auto& e : out)
            if (e.message.isNoteOnOrOff() && e.message.getNoteNumber() == 60)
                notes.push_back(e);

        expectEquals((int) notes.size(), 4);
        for (size_t i = 0; i < notes.size(); ++i)
        {
            expect(notes[i].message.isNoteOn() == (i % 2 == 0), "event " + juce::String((int) i) + " alternates on/off");
            if (i % 2 == 1)
                expect(notes[i].time > notes[i - 1].time, "note " + juce::String((int) i / 2) + " has a positive length");
        }
    }

    void testZeroTempoPassesThrough()
    {
        for (double tempo : { 0.0, 0.001 })
        {
            beginTest("A host tempo of " + juce::String(tempo) + " BPM passes MIDI through");

            Harness h;
            h.setParam("gridResolution", 0.0f);   // 1/4: the largest lookahead
            h.playHead.tempo = tempo;
            auto out = h.run({ { 5000, on(60) }, { 5300, off(60) } }, 20);

            expectEquals(timeOf(out, 60, true), (int64_t) 5000);
            expectEquals(timeOf(out, 60, false), (int64_t) 5300);
            const int latency = h.processor.getLatencySamples();
            expect(latency > 0 && latency <= 65536, "latency " + juce::String(latency));
        }
    }

    void testPassThroughNoteOnClearsShift()
    {
        beginTest("A note played while stopped keeps its length when play starts before its note-off");

        Harness h;
        // While playing, a late note on 60 is moved 1000 samples earlier
        h.run({ { 7000, on(60) }, { 8000, off(60) } }, 40);

        h.playHead.playing = false;
        h.run({ { 21000, on(60) } }, 4);   // passes through unmoved

        h.playHead.playing = true;
        auto out = h.run({ { 23000, off(60) } }, 40);
        const int64_t latency = h.processor.getLatencySamples();

        expectEquals(timeOf(out, 60, false), (int64_t) 23000 + latency,
                     "note-off is not moved by the earlier note's shift");
    }

    void testStopDropsNotesNotYetStarted()
    {
        beginTest("Stopping never releases a zero-length note");

        Harness h;
        // Note 60 is fully queued when the transport stops; note 62 is held (no note-off yet)
        auto out = h.run({ { 100, on(60) }, { 300, off(60) }, { 400, on(62) } }, 1);
        h.playHead.playing = false;
        auto stopped = h.run({ { 1000, off(62) } }, 4);
        out.insert(out.end(), stopped.begin(), stopped.end());

        int ons60 = 0, offs60 = 0;
        for (const auto& e : out)
            if (e.message.getNoteNumber() == 60)
                (e.message.isNoteOn() ? ons60 : offs60) += 1;
        expectEquals(ons60, offs60, "no stuck note");
        if (ons60 > 0)
            expect(timeOf(out, 60, false) > timeOf(out, 60, true), "note 60 has a length");

        expect(timeOf(out, 62, true) >= 0, "held note is still played");
        expect(timeOf(out, 62, false) > timeOf(out, 62, true), "held note is released after it starts");
    }

    void testBypassReleasesQueuedNotes()
    {
        beginTest("Bypassing mid-note releases the note and passes MIDI through");

        Harness h;
        // Both events enter the lookahead queue (the note-off at block 3); play
        // until the note-on is out
        std::vector<OutEvent> out = h.run({ { 100, on(60) }, { 2000, off(60) } }, 4);
        while (timeOf(out, 60, true) < 0 && h.blocksDone < 100)
        {
            const auto more = h.run({}, 1);
            out.insert(out.end(), more.begin(), more.end());
        }
        expect(timeOf(out, 60, true) >= 0, "the note-on is played before bypassing");
        expect(timeOf(out, 60, false) < 0, "its note-off is still queued");

        h.bypassed = true;
        const int64_t bypassStart = h.blocksDone * blockSize;
        const auto bypassed = h.run({ { bypassStart + 10, on(64) } }, 2);
        expect(timeOf(bypassed, 60, false) >= 0, "the queued note-off is released on bypass");
        expectEquals(timeOf(bypassed, 64, true), bypassStart + 10,
                     "MIDI passes straight through while bypassed");

        h.bypassed = false;
        const auto resumed = h.run({}, 40);
        expectEquals(timeOf(resumed, 60, false), (int64_t) -1, "the note-off is not sent again after bypass");
    }
};

static MIDIGrooveQuantizerTests midiGrooveQuantizerTests;
