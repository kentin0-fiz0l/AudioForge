/**
 * MIDIHarmonizer Plugin Tests
 *
 * Drives the processor through processBlock with MIDI note-ons and checks the
 * harmony notes it emits:
 * - Chromatic mode uses fixed intervals (major 3rd, perfect 5th)
 * - Seven-note scales give diatonic 3rds and 5ths, above and below, in every key
 * - Out-of-key input notes still get in-key harmony
 * - Pentatonic and Blues harmony stays in the scale
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <set>

#include "../../plugins/MIDIHarmonizer/Source/PluginProcessor.h"

class MIDIHarmonizerPluginTests : public juce::UnitTest
{
public:
    MIDIHarmonizerPluginTests()
        : juce::UnitTest("MIDIHarmonizer Plugin", "Plugins")
    {
    }

    void runTest() override
    {
        testChromaticIntervals();
        testCMajorThirdsAndFifths();
        testDiatonicIntervalsInEveryKeyAndMode();
        testOutOfKeyInputGetsInKeyHarmony();
        testFiveAndSixNoteScalesStayInScale();
    }

private:
    // Scale choice indices as listed in the "scale" parameter
    enum ScaleIndex { Chromatic = 0, Major, Minor, Dorian, Phrygian, Lydian,
                      Mixolydian, Aeolian, Locrian, HarmonicMinor, MelodicMinor,
                      Pentatonic, Blues };

    struct ScaleDef { int index; const char* name; std::vector<int> tones; };

    static const std::vector<ScaleDef>& heptatonicScales()
    {
        static const std::vector<ScaleDef> scales {
            { Major,         "Major",          { 0, 2, 4, 5, 7, 9, 11 } },
            { Minor,         "Minor",          { 0, 2, 3, 5, 7, 8, 10 } },
            { Dorian,        "Dorian",         { 0, 2, 3, 5, 7, 9, 10 } },
            { Phrygian,      "Phrygian",       { 0, 1, 3, 5, 7, 8, 10 } },
            { Lydian,        "Lydian",         { 0, 2, 4, 6, 7, 9, 11 } },
            { Mixolydian,    "Mixolydian",     { 0, 2, 4, 5, 7, 9, 10 } },
            { Aeolian,       "Aeolian",        { 0, 2, 3, 5, 7, 8, 10 } },
            { Locrian,       "Locrian",        { 0, 1, 3, 5, 6, 8, 10 } },
            { HarmonicMinor, "Harmonic Minor", { 0, 2, 3, 5, 7, 8, 11 } },
            { MelodicMinor,  "Melodic Minor",  { 0, 2, 3, 5, 7, 9, 11 } },
        };
        return scales;
    }

    static void setParam(MIDIHarmonizerProcessor& p, const juce::String& id, float plainValue)
    {
        auto* param = p.getAPVTS().getParameter(id);
        param->setValueNotifyingHost(param->convertTo0to1(plainValue));
    }

    // Enables exactly one harmony voice ("thirdAbove", "fifthBelow", ...)
    static void soloHarmony(MIDIHarmonizerProcessor& p, const juce::String& id)
    {
        for (auto* h : { "thirdAbove", "fifthAbove", "octaveAbove",
                         "thirdBelow", "fifthBelow", "octaveBelow" })
            setParam(p, h, id == h ? 1.0f : 0.0f);
    }

    // Plays one note and returns the note numbers of every note-on except the input
    static std::vector<int> harmonyFor(MIDIHarmonizerProcessor& p, int note)
    {
        juce::AudioBuffer<float> buffer(2, 64);
        juce::MidiBuffer midi;
        midi.addEvent(juce::MidiMessage::noteOn(1, note, (juce::uint8) 100), 0);
        p.processBlock(buffer, midi);

        std::vector<int> result;
        for (const auto m : midi)
            if (m.getMessage().isNoteOn() && m.getMessage().getNoteNumber() != note)
                result.push_back(m.getMessage().getNoteNumber());

        // Release so the next call starts clean
        juce::MidiBuffer off;
        off.addEvent(juce::MidiMessage::noteOff(1, note), 0);
        p.processBlock(buffer, off);
        return result;
    }

    static int single(const std::vector<int>& notes) { return notes.size() == 1 ? notes[0] : -1; }

    static bool inScale(int note, int key, const std::vector<int>& tones)
    {
        int pc = ((note - key) % 12 + 12) % 12;
        return std::find(tones.begin(), tones.end(), pc) != tones.end();
    }

    void testChromaticIntervals()
    {
        beginTest("Chromatic mode uses major 3rd and perfect 5th");

        MIDIHarmonizerProcessor p;
        p.prepareToPlay(44100.0, 64);
        setParam(p, "scale", Chromatic);

        soloHarmony(p, "thirdAbove");
        expectEquals(single(harmonyFor(p, 61)), 65);
        soloHarmony(p, "fifthAbove");
        expectEquals(single(harmonyFor(p, 61)), 68);
        soloHarmony(p, "thirdBelow");
        expectEquals(single(harmonyFor(p, 61)), 57);
        soloHarmony(p, "fifthBelow");
        expectEquals(single(harmonyFor(p, 61)), 54);
    }

    void testCMajorThirdsAndFifths()
    {
        beginTest("C Major: 3rd above C is E, 5th above C is G");

        MIDIHarmonizerProcessor p;
        p.prepareToPlay(44100.0, 64);
        setParam(p, "key", 0);
        setParam(p, "scale", Major);

        soloHarmony(p, "thirdAbove");
        expectEquals(single(harmonyFor(p, 60)), 64, "C4 + 3rd should be E4");
        expectEquals(single(harmonyFor(p, 62)), 65, "D4 + 3rd should be F4");
        expectEquals(single(harmonyFor(p, 71)), 74, "B4 + 3rd should be D5");

        soloHarmony(p, "fifthAbove");
        expectEquals(single(harmonyFor(p, 60)), 67, "C4 + 5th should be G4");
        expectEquals(single(harmonyFor(p, 71)), 77, "B4 + 5th should be F5");

        soloHarmony(p, "thirdBelow");
        expectEquals(single(harmonyFor(p, 60)), 57, "C4 - 3rd should be A3");

        soloHarmony(p, "fifthBelow");
        expectEquals(single(harmonyFor(p, 60)), 53, "C4 - 5th should be F3");
    }

    void testDiatonicIntervalsInEveryKeyAndMode()
    {
        beginTest("Seven-note scales: diatonic 3rds and 5ths in every key");

        MIDIHarmonizerProcessor p;
        p.prepareToPlay(44100.0, 64);

        struct Voice { const char* id; int steps; };
        const Voice voices[] = { { "thirdAbove", 2 }, { "fifthAbove", 4 },
                                 { "thirdBelow", -2 }, { "fifthBelow", -4 } };

        int failures = 0;
        for (const auto& scale : heptatonicScales())
        {
            setParam(p, "scale", (float) scale.index);
            for (int key = 0; key < 12; ++key)
            {
                setParam(p, "key", (float) key);
                for (const auto& v : voices)
                {
                    soloHarmony(p, v.id);
                    for (int degree = 0; degree < 7; ++degree)
                    {
                        int note = 60 + key + scale.tones[(size_t) degree];
                        int target = degree + v.steps;
                        int octave = target >= 0 ? target / 7 : -1;
                        int expected = 60 + key + octave * 12 + scale.tones[(size_t) ((target + 7) % 7)];

                        int got = single(harmonyFor(p, note));
                        if (got != expected && ++failures <= 5)
                            expectEquals(got, expected, juce::String(scale.name) + " key " + juce::String(key)
                                         + " " + v.id + " from note " + juce::String(note));
                    }
                }
            }
        }
        expectEquals(failures, 0, "All diatonic harmony notes should match");
    }

    void testOutOfKeyInputGetsInKeyHarmony()
    {
        beginTest("Out-of-key input notes get in-key harmony");

        MIDIHarmonizerProcessor p;
        p.prepareToPlay(44100.0, 64);

        for (const auto& scale : heptatonicScales())
        {
            setParam(p, "scale", (float) scale.index);
            for (int key = 0; key < 12; ++key)
            {
                setParam(p, "key", (float) key);
                for (auto* id : { "thirdAbove", "fifthAbove", "thirdBelow", "fifthBelow" })
                {
                    soloHarmony(p, id);
                    bool above = juce::String(id).endsWith("Above");
                    for (int note = 60; note < 72; ++note)
                    {
                        if (inScale(note, key, scale.tones))
                            continue;
                        int got = single(harmonyFor(p, note));
                        int interval = std::abs(got - note);
                        expect(got >= 0 && inScale(got, key, scale.tones) && (got > note) == above
                                   && interval >= 2 && interval <= 9,
                               juce::String(scale.name) + " key " + juce::String(key) + " " + id
                                   + " from " + juce::String(note) + " gave " + juce::String(got));
                    }
                }
            }
        }
    }

    void testFiveAndSixNoteScalesStayInScale()
    {
        beginTest("Pentatonic and Blues harmony stays in scale");

        MIDIHarmonizerProcessor p;
        p.prepareToPlay(44100.0, 64);

        struct Case { int index; const char* name; std::vector<int> tones; };
        const Case cases[] = { { Pentatonic, "Pentatonic", { 0, 2, 4, 7, 9 } },
                               { Blues,      "Blues",      { 0, 3, 5, 6, 7, 10 } } };

        // C Major Pentatonic from C: E and G
        setParam(p, "scale", Pentatonic);
        setParam(p, "key", 0);
        soloHarmony(p, "thirdAbove");
        expectEquals(single(harmonyFor(p, 60)), 64);
        soloHarmony(p, "fifthAbove");
        expectEquals(single(harmonyFor(p, 60)), 67);

        for (const auto& c : cases)
        {
            setParam(p, "scale", (float) c.index);
            for (int key = 0; key < 12; ++key)
            {
                setParam(p, "key", (float) key);
                for (auto* id : { "thirdAbove", "fifthAbove", "thirdBelow", "fifthBelow" })
                {
                    soloHarmony(p, id);
                    bool above = juce::String(id).endsWith("Above");
                    bool fifth = juce::String(id).startsWith("fifth");
                    for (int note = 60; note < 72; ++note)
                    {
                        int got = single(harmonyFor(p, note));
                        int interval = std::abs(got - note);
                        bool sizeOk = fifth ? (interval >= 5 && interval <= 9) : (interval >= 2 && interval <= 6);
                        expect(got >= 0 && inScale(got, key, c.tones) && (got > note) == above && sizeOk,
                               juce::String(c.name) + " key " + juce::String(key) + " " + id
                                   + " from " + juce::String(note) + " gave " + juce::String(got));
                    }
                }
            }
        }
    }
};

static MIDIHarmonizerPluginTests midiHarmonizerPluginTests;
