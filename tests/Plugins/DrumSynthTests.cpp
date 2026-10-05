/**
 * DrumSynth Tests
 *
 * The note map follows General MIDI, and the hi-hat can be played closed or
 * open: open rings longer, and a closed hit cuts an open one short.
 *
 * It is a kit for house and techno: kick, snare, clap and both hats. Hits
 * land where their notes are, and play as hard as they are hit.
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <cmath>
#include <vector>

#include "../../plugins/DrumSynth/Source/DrumNotes.h"
#include "../../plugins/DrumSynth/Source/HiHatModule.h"
#include "../../plugins/DrumSynth/Source/PluginProcessor.h"

class DrumSynthTests : public juce::UnitTest
{
public:
    DrumSynthTests()
        : juce::UnitTest("DrumSynth", "Plugins")
    {
    }

    void runTest() override
    {
        testNoteMap();
        testOpenHatRingsLonger();
        testClosedHatCutsOffOpenHat();
        testClapPlays();
        testClapIsSeveralHandsThenATail();
        testClapSitsWithTheSnare();
        testHitsFollowVelocity();
        testHitsLandWhereTheirNotesAre();
        testNewParametersComeLast();
    }

private:
    static constexpr double sampleRate = 48000.0;

    // The plugin's default hi-hat settings
    static float next(HiHatModule& hat)
    {
        return hat.processSample(sampleRate, 0.5f, 0.1f, 0.7f, 0.5f);
    }

    struct Hit
    {
        double seconds = 0.0;
        float peak = 0.0f;
        bool finite = true;
    };

    // Lets the hat ring until it stops by itself
    static Hit ringOut(HiHatModule& hat)
    {
        Hit hit;
        int samples = 0;

        while (hat.isActive() && samples < static_cast<int>(sampleRate * 10.0))
        {
            const float sample = next(hat);
            hit.finite = hit.finite && std::isfinite(sample);
            hit.peak = juce::jmax(hit.peak, std::abs(sample));
            ++samples;
        }

        hit.seconds = samples / sampleRate;
        return hit;
    }

    void testNoteMap()
    {
        beginTest("Notes follow the General MIDI drum map");

        expect(drumForNote(36) == Drum::kick);
        expect(drumForNote(38) == Drum::snare);
        expect(drumForNote(42) == Drum::closedHat);
        expect(drumForNote(44) == Drum::closedHat, "The pedal hat should play the closed hat");
        expect(drumForNote(46) == Drum::openHat, "A#1 should play the open hat");
        expect(drumForNote(39) == Drum::clap, "D#1 should play the clap");

        for (int note : { 0, 35, 37, 40, 45, 47, 60, 127 })
            expect(drumForNote(note) == Drum::none, "Note " + juce::String(note) + " should play nothing");
    }

    void testOpenHatRingsLonger()
    {
        beginTest("An open hat rings longer than a closed one, at the same level");

        HiHatModule hat;
        hat.trigger(false);
        const auto closed = ringOut(hat);

        hat.trigger(true);
        const auto open = ringOut(hat);

        expect(closed.finite && open.finite, "Non-finite output");
        expect(closed.seconds > 0.01, "Closed hat lasts only " + juce::String(closed.seconds, 4) + " s");
        expect(open.seconds > 5.0 * closed.seconds,
               "Open hat lasts " + juce::String(open.seconds, 3) + " s against "
                   + juce::String(closed.seconds, 3) + " s closed");
        expect(open.seconds < 2.0, "Open hat lasts " + juce::String(open.seconds, 3) + " s");

        // The two are the same hit with a different tail, so equally loud
        expect(closed.peak > 0.05f, "Closed hat peaks at only " + juce::String(closed.peak, 4));
        expectWithinAbsoluteError(juce::Decibels::gainToDecibels(open.peak),
                                  juce::Decibels::gainToDecibels(closed.peak), 1.5f);
    }

    void testClosedHatCutsOffOpenHat()
    {
        beginTest("A closed hit cuts off an open hat that is still ringing");

        HiHatModule reference;
        reference.trigger(false);
        const double closedSeconds = ringOut(reference).seconds;

        HiHatModule hat;
        hat.trigger(true);
        for (int i = 0; i < static_cast<int>(sampleRate * 0.02); ++i)
            next(hat);

        expect(hat.isActive(), "The open hat should still be ringing after 20 ms");

        hat.trigger(false);
        const double afterChoke = ringOut(hat).seconds;

        expectWithinAbsoluteError(afterChoke, closedSeconds, closedSeconds * 0.05);
    }

    //==========================================================================
    // Through the plugin itself

    static constexpr int blockSize = 512;

    // One note into a new instance, and what comes out of the left channel
    // over the next half second
    static std::vector<float> play(int note, int velocity = 127, int atSample = 0)
    {
        PluginProcessor processor;
        processor.setRateAndBufferSizeDetails(sampleRate, blockSize);
        processor.prepareToPlay(sampleRate, blockSize);

        std::vector<float> output;
        juce::AudioBuffer<float> buffer(2, blockSize);

        for (int block = 0; block < static_cast<int>(0.5 * sampleRate / blockSize); ++block)
        {
            juce::MidiBuffer midi;
            if (block == 0)
                midi.addEvent(juce::MidiMessage::noteOn(1, note, static_cast<juce::uint8>(velocity)), atSample);

            buffer.clear();
            processor.processBlock(buffer, midi);
            output.insert(output.end(), buffer.getReadPointer(0), buffer.getReadPointer(0) + blockSize);
        }

        return output;
    }

    static float peakOf(const std::vector<float>& samples)
    {
        float peak = 0.0f;
        for (float sample : samples)
            peak = juce::jmax(peak, std::abs(sample));
        return peak;
    }

    static float rmsBetween(const std::vector<float>& samples, double fromMs, double toMs)
    {
        const auto from = static_cast<size_t>(fromMs * 0.001 * sampleRate);
        const auto to = static_cast<size_t>(toMs * 0.001 * sampleRate);

        double sum = 0.0;
        for (size_t i = from; i < to; ++i)
            sum += samples[i] * samples[i];

        return static_cast<float>(std::sqrt(sum / static_cast<double>(to - from)));
    }

    void testClapPlays()
    {
        beginTest("D#1 plays a clap");

        const auto clap = play(39);

        expect(peakOf(clap) > 0.05f, "The clap peaks at " + juce::String(peakOf(clap), 4));
        expect(rmsBetween(clap, 400.0, 490.0) < 0.001f, "The clap should have died away within half a second");
    }

    void testClapIsSeveralHandsThenATail()
    {
        beginTest("A clap is three quick bursts and then a tail");

        // Hands do not land together. The bursts are 11 ms apart, each gone
        // before the next, which is what tells a clap from a snare.
        const auto clap = play(39);

        for (double burst : { 0.0, 11.0, 22.0 })
        {
            const float during = rmsBetween(clap, burst + 0.5, burst + 3.0);
            const float between = rmsBetween(clap, burst + 7.0, burst + 10.5);

            expect(during > 3.0f * between,
                   "The burst at " + juce::String(burst, 0) + " ms is at " + juce::String(during, 4)
                       + " and the gap after it at " + juce::String(between, 4));
        }

        // After the last burst the tail carries on instead of cutting off
        expect(rmsBetween(clap, 45.0, 60.0) > 0.3f * rmsBetween(clap, 33.5, 36.0), "The tail dies too fast");
    }

    void testClapSitsWithTheSnare()
    {
        beginTest("The clap is within 4 dB of the snare");

        const float difference = juce::Decibels::gainToDecibels(peakOf(play(39)) / peakOf(play(38)));

        // The snare's noise makes its peak move by a dB or so from hit to hit
        expect(std::abs(difference) < 4.0f, "The clap is " + juce::String(difference, 1) + " dB from the snare");
    }

    void testHitsFollowVelocity()
    {
        // The noise in a snare or a hat makes its peak a matter of luck,
        // so hits are compared by their level over the first 30 ms
        for (int note : { 36, 38, 39, 42, 46 })
        {
            beginTest("Note " + juce::String(note) + " plays half as loud at half velocity");

            const float ratio = rmsBetween(play(note, 64), 0.0, 30.0) / rmsBetween(play(note, 127), 0.0, 30.0);

            expect(ratio > 0.4f && ratio < 0.6f,
                   "Half velocity came out at " + juce::String(ratio, 2) + " of full");
        }
    }

    void testHitsLandWhereTheirNotesAre()
    {
        beginTest("A hit starts at its note's position in the block, not at the start of the block");

        const int position = 300;
        const auto kick = play(36, 127, position);

        float before = 0.0f, after = 0.0f;
        for (int i = 0; i < position; ++i)
            before = juce::jmax(before, std::abs(kick[static_cast<size_t>(i)]));
        for (int i = position; i < blockSize; ++i)
            after = juce::jmax(after, std::abs(kick[static_cast<size_t>(i)]));

        expectEquals(before, 0.0f, "Nothing should sound before the note");
        expect(after > 0.01f, "The kick should sound from the note on");
    }

    void testNewParametersComeLast()
    {
        beginTest("The clap's parameters come after the ones hosts already know");

        PluginProcessor processor;
        const auto& params = processor.getParameters();

        expectEquals(params.size(), 16);
        expectEquals(params[0]->getName(64), juce::String("Kick Pitch"));
        expectEquals(params[13]->getName(64), juce::String("HiHat Click"));

        if (params.size() == 16)
        {
            expectEquals(params[14]->getName(64), juce::String("Clap Tone"));
            expectEquals(params[15]->getName(64), juce::String("Clap Decay"));
        }
    }
};

static DrumSynthTests drumSynthTests;
