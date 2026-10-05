/**
 * DrumSynth Tests
 *
 * The note map follows General MIDI, and the hi-hat can be played closed or
 * open: open rings longer, and a closed hit cuts an open one short.
 */

#include <juce_audio_basics/juce_audio_basics.h>
#include <cmath>
#include <vector>

#include "../../plugins/DrumSynth/Source/DrumNotes.h"
#include "../../plugins/DrumSynth/Source/HiHatModule.h"

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
};

static DrumSynthTests drumSynthTests;
