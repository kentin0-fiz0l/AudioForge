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
            info.setBpm(bpm);
            info.setPpqPosition(ppq);
            info.setTimeInSamples(timeInSamples);
            return info;
        }

        bool playing = true;
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
        // and returns every output event with its absolute sample time.
        std::vector<OutEvent> run(const std::vector<OutEvent>& input, int numBlocks)
        {
            std::vector<OutEvent> out;
            for (int b = 0; b < numBlocks; ++b)
            {
                const int64_t blockStart = (int64_t) b * blockSize;
                juce::MidiBuffer midi;
                for (const auto& e : input)
                    if (e.time >= blockStart && e.time < blockStart + blockSize)
                        midi.addEvent(e.message, (int) (e.time - blockStart));

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
};

static MIDIGrooveQuantizerTests midiGrooveQuantizerTests;
