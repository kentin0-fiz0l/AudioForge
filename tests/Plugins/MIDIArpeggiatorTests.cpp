/**
 * MIDI Arpeggiator Tests
 *
 * Behaviour tests for the MIDI Arpeggiator plugin, driven through the
 * processor's public API (MIDI in, MIDI out, a fake host transport):
 * - Free-running steps keep coming block after block
 * - Swing delays the off-beat steps
 * - A pattern change applies at the next step
 * - Latch: turning it off clears latched notes, a fresh chord replaces them
 * - Steps lock to the host beat position while the host is playing
 */

#include <juce_audio_processors/juce_audio_processors.h>

#include "../../plugins/MIDIArpeggiator/Source/PluginProcessor.h"

namespace
{
    constexpr double arpSampleRate = 48000.0;
    constexpr int arpBlockSize = 512;
    constexpr double arpBpm = 120.0;

    // 1/16 at 120 BPM and 48 kHz: a quarter of 24000 samples
    constexpr int sixteenthRateIndex = 4;
    constexpr int samplesPerSixteenth = 6000;

    class ArpPlayHead : public juce::AudioPlayHead
    {
    public:
        juce::Optional<PositionInfo> getPosition() const override
        {
            PositionInfo info;
            info.setIsPlaying(playing);
            info.setPpqPosition(ppq);
            info.setBpm(arpBpm);
            return info;
        }

        bool playing = false;
        double ppq = 0.0;
    };

    struct NoteEvent
    {
        int64_t sample;
        int note;
        bool isOn;
    };

    // A processor on a fake host transport that records every note it outputs
    // with its absolute sample position
    struct ArpHarness
    {
        ArpHarness()
        {
            processor.setPlayHead(&playHead);
            processor.prepareToPlay(arpSampleRate, arpBlockSize);
            setParameter("rate", (float)sixteenthRateIndex);
        }

        void setParameter(const juce::String& id, float value)
        {
            auto* param = processor.getAPVTS().getParameter(id);
            param->setValueNotifyingHost(param->convertTo0to1(value));
        }

        void processBlock(juce::MidiBuffer midiIn = {})
        {
            processor.processBlock(audio, midiIn);

            for (const auto metadata : midiIn)
            {
                const auto message = metadata.getMessage();
                if (message.isNoteOn() || message.isNoteOff())
                    events.push_back({ samplePosition + metadata.samplePosition,
                                       message.getNoteNumber(), message.isNoteOn() });
            }

            samplePosition += arpBlockSize;
            if (playHead.playing)
                playHead.ppq += arpBlockSize * arpBpm / (60.0 * arpSampleRate);
        }

        void processSamples(int64_t numSamples)
        {
            for (int64_t done = 0; done < numSamples; done += arpBlockSize)
                processBlock();
        }

        void sendNotes(std::initializer_list<int> notes, bool on)
        {
            juce::MidiBuffer midi;
            for (int note : notes)
                midi.addEvent(on ? juce::MidiMessage::noteOn(1, note, (juce::uint8)100)
                                 : juce::MidiMessage::noteOff(1, note), 0);
            processBlock(midi);
        }

        std::vector<NoteEvent> noteOnsFrom(int64_t fromSample) const
        {
            std::vector<NoteEvent> result;
            for (const auto& e : events)
                if (e.isOn && e.sample >= fromSample)
                    result.push_back(e);
            return result;
        }

        bool allNotesReleased() const
        {
            int sounding = 0;
            for (const auto& e : events)
                sounding += e.isOn ? 1 : -1;
            return sounding == 0;
        }

        MIDIArpeggiatorProcessor processor;
        ArpPlayHead playHead;
        juce::AudioBuffer<float> audio { 2, arpBlockSize };
        std::vector<NoteEvent> events;
        int64_t samplePosition = 0;
    };
}

//==============================================================================
class MIDIArpeggiatorTests : public juce::UnitTest
{
public:
    MIDIArpeggiatorTests()
        : juce::UnitTest("MIDI Arpeggiator", "MIDIArpeggiator")
    {
    }

    void runTest() override
    {
        testFreeRunningStepsContinue();
        testSwingDelaysOffBeats();
        testPatternChangeAppliesAtNextStep();
        testLatchOffClearsNotes();
        testLatchFreshChordReplacesNotes();
        testStepsLockToHostBeat();
    }

private:
    void testFreeRunningStepsContinue()
    {
        beginTest("Free-running steps continue across blocks");

        ArpHarness arp;
        arp.sendNotes({ 60, 64, 67 }, true);
        arp.processSamples(samplesPerSixteenth * 8);

        auto ons = arp.noteOnsFrom(0);
        expectGreaterOrEqual((int)ons.size(), 8, "One note per step for eight steps");

        for (size_t i = 0; i < ons.size(); ++i)
            expectEquals(ons[i].sample, (int64_t)(i * samplesPerSixteenth), "Steps fall on the 1/16 grid");

        static const int upOrder[] = { 60, 64, 67 };
        for (size_t i = 0; i < ons.size(); ++i)
            expectEquals(ons[i].note, upOrder[i % 3], "Up pattern cycles through the chord");

        arp.sendNotes({ 60, 64, 67 }, false);
        arp.processSamples(samplesPerSixteenth * 2);
        expect(arp.allNotesReleased(), "Releasing the chord leaves no note hanging");
    }

    void testSwingDelaysOffBeats()
    {
        beginTest("Swing delays off-beat steps");

        ArpHarness arp;
        arp.setParameter("swing", 0.5f);
        arp.sendNotes({ 60, 64, 67 }, true);
        arp.processSamples(samplesPerSixteenth * 6);

        auto ons = arp.noteOnsFrom(0);
        expectGreaterOrEqual((int)ons.size(), 6, "Six steps played");

        // Swing 0.5 delays each off-beat by a quarter of a step
        const int64_t swingDelay = samplesPerSixteenth / 4;
        for (size_t i = 0; i < 6 && i < ons.size(); ++i)
        {
            const int64_t grid = (int64_t)(i * samplesPerSixteenth);
            expectEquals(ons[i].sample, (i % 2 == 0) ? grid : grid + swingDelay,
                         "On-beats stay on the grid, off-beats are delayed");
        }

        // Every note still ends before the next one starts
        int sounding = 0;
        for (const auto& e : arp.events)
        {
            sounding += e.isOn ? 1 : -1;
            expect(sounding <= 1, "Swung notes do not overlap");
        }
    }

    void testPatternChangeAppliesAtNextStep()
    {
        beginTest("Pattern change applies at the next step");

        ArpHarness arp;
        arp.sendNotes({ 60, 64, 67 }, true);
        arp.processSamples(samplesPerSixteenth * 3);

        // Switch Up -> Down with the chord still held and no new MIDI
        arp.setParameter("pattern", 1.0f);
        const auto changedAt = arp.samplePosition;
        arp.processSamples(samplesPerSixteenth * 6);

        auto ons = arp.noteOnsFrom(changedAt);
        expectGreaterOrEqual((int)ons.size(), 6, "Arp keeps playing after the change");

        // Down cycles 67 -> 64 -> 60 -> 67; Up would go 60 -> 64 -> 67 -> 60
        auto nextInDown = [](int note) { return note == 67 ? 64 : note == 64 ? 60 : 67; };
        for (size_t i = 1; i < 6 && i < ons.size(); ++i)
            expectEquals(ons[i].note, nextInDown(ons[i - 1].note),
                         "Steps after the change follow the Down pattern");
    }

    void testLatchOffClearsNotes()
    {
        beginTest("Turning latch off clears latched notes");

        ArpHarness arp;
        arp.setParameter("latch", 1.0f);
        arp.sendNotes({ 60, 64, 67 }, true);
        arp.sendNotes({ 60, 64, 67 }, false);
        arp.processSamples(samplesPerSixteenth * 3);

        const auto releasedAt = arp.samplePosition;
        arp.processSamples(samplesPerSixteenth * 3);
        expect(!arp.noteOnsFrom(releasedAt).empty(), "Latch keeps playing after release");

        arp.setParameter("latch", 0.0f);
        arp.processBlock();
        const auto latchOffAt = arp.samplePosition;
        arp.processSamples(samplesPerSixteenth * 4);

        expect(arp.noteOnsFrom(latchOffAt).empty(), "No more steps once latch is off");
        expect(arp.allNotesReleased(), "No note left hanging when latch is turned off");
    }

    void testLatchFreshChordReplacesNotes()
    {
        beginTest("Latch: a fresh chord after release replaces the latched notes");

        ArpHarness arp;
        arp.setParameter("latch", 1.0f);
        arp.sendNotes({ 60, 64, 67 }, true);
        arp.sendNotes({ 60, 64, 67 }, false);
        arp.processSamples(samplesPerSixteenth * 3);

        arp.sendNotes({ 62 }, true);
        const auto freshAt = arp.samplePosition;
        arp.processSamples(samplesPerSixteenth * 4);

        auto ons = arp.noteOnsFrom(freshAt);
        expect(!ons.empty(), "Arp plays the new chord");
        for (const auto& e : ons)
            expectEquals(e.note, 62, "Only the new note plays");

        // Notes added while a key is still down join the chord
        arp.sendNotes({ 65 }, true);
        const auto addedAt = arp.samplePosition;
        arp.processSamples(samplesPerSixteenth * 4);

        bool heard62 = false, heard65 = false;
        for (const auto& e : arp.noteOnsFrom(addedAt))
        {
            heard62 |= e.note == 62;
            heard65 |= e.note == 65;
        }
        expect(heard62 && heard65, "A note added while keys are held joins the chord");
    }

    void testStepsLockToHostBeat()
    {
        beginTest("Steps lock to the host beat position while playing");

        ArpHarness arp;
        arp.playHead.playing = true;
        arp.playHead.ppq = 0.1; // start between two 1/16 steps

        arp.sendNotes({ 60, 64, 67 }, true);
        arp.processSamples(samplesPerSixteenth * 4);

        auto ons = arp.noteOnsFrom(0);
        expectGreaterOrEqual((int)ons.size(), 3, "Arp plays while the host is playing");

        // 1/16 steps fall on ppq 0.25, 0.5, 0.75...; ppq 0.25 is 0.15 beats after the start
        const int64_t firstStep = (int64_t)std::llround(0.15 * 60.0 / arpBpm * arpSampleRate);
        for (size_t i = 0; i < ons.size(); ++i)
            expect(std::abs(ons[i].sample - (firstStep + (int64_t)i * samplesPerSixteenth)) <= 1,
                   "Step " + juce::String((int)i) + " at sample " + juce::String(ons[i].sample)
                       + " should be on the host's 1/16 grid");

        // A host jump (loop back) re-locks to the new position
        arp.playHead.ppq = 4.2;
        const auto jumpAt = arp.samplePosition;
        arp.processSamples(samplesPerSixteenth * 2);

        auto afterJump = arp.noteOnsFrom(jumpAt);
        expect(!afterJump.empty(), "Arp keeps playing after a host jump");
        if (!afterJump.empty())
        {
            const int64_t expected = jumpAt + (int64_t)std::llround(0.05 * 60.0 / arpBpm * arpSampleRate);
            expect(std::abs(afterJump.front().sample - expected) <= 1,
                   "First step after the jump lands on ppq 4.25");
        }
    }
};

static MIDIArpeggiatorTests midiArpeggiatorTests;
