/**
 * BassLine Plugin Tests
 *
 * - A bass line in a key stays in one register: the editor hands the processor
 *   the key root as a MIDI note (48 + key), and bars 1 and 4 used to treat that
 *   note number as a pitch class and play about four octaves above bars 2 and 3.
 * - A saved session plays back the same: the synth settings, effects, key,
 *   style and the generated bass line itself are saved and restored.
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <cmath>
#include <thread>
#include <atomic>

#include "../../plugins/BassLine/Source/PluginProcessor.h"

namespace
{
    constexpr double sampleRate = 44100.0;
    constexpr int blockSize = 512;
    constexpr double bpm = 120.0;
    constexpr int samplesPerBeat = static_cast<int>(sampleRate * 60.0 / bpm);

    // Reports a playing transport that advances as blocks are processed
    class FakePlayHead : public juce::AudioPlayHead
    {
    public:
        juce::Optional<PositionInfo> getPosition() const override
        {
            PositionInfo info;
            info.setIsPlaying(true);
            info.setPpqPosition(ppq);
            info.setBpm(bpm);
            return info;
        }

        double ppq = 0.0;
    };

    // Plays the processor's bass line from beat 0 and returns the left channel
    std::vector<float> render(BassLineProcessor& processor, int numBeats)
    {
        FakePlayHead playHead;
        processor.setPlayHead(&playHead);
        processor.prepareToPlay(sampleRate, blockSize);

        std::vector<float> output;
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midi;

        while (static_cast<int>(output.size()) < numBeats * samplesPerBeat)
        {
            processor.processBlock(buffer, midi);
            output.insert(output.end(), buffer.getReadPointer(0), buffer.getReadPointer(0) + blockSize);
            playHead.ppq += blockSize * bpm / (60.0 * sampleRate);
        }

        processor.setPlayHead(nullptr);
        return output;
    }

    // Pitch of a stretch of output, from its rising zero crossings
    double frequencyBetween(const std::vector<float>& signal, int start, int end)
    {
        int first = -1, last = -1, crossings = 0;
        for (int i = start + 1; i < end; ++i)
        {
            if (signal[(size_t) i - 1] < 0.0f && signal[(size_t) i] >= 0.0f)
            {
                if (first < 0)
                    first = i;
                last = i;
                ++crossings;
            }
        }
        if (crossings < 2)
            return 0.0;
        return (crossings - 1) * sampleRate / (last - first);
    }
}

class BassLinePluginTests : public juce::UnitTest
{
public:
    BassLinePluginTests()
        : juce::UnitTest("BassLine Plugin", "Plugins")
    {
    }

    void runTest() override
    {
        testEveryBarInTheSameRegister();
        testStateRoundTrip();
        testIgnoresStateItCannotRead();
        testDamagedSessionIsMadeSafe();
        testSavingWhileTheLineChanges();
        testOpeningEditorKeepsSession();
    }

private:
    void testEveryBarInTheSameRegister()
    {
        beginTest("Every bar of a bass line in a key plays in the same register");

        for (int key = 0; key < 12; ++key)
        {
            BassLineProcessor processor;
            processor.setWaveform(0); // sine, so the pitch is easy to read
            processor.setKey(48 + key, true); // as the editor passes it
            processor.generateNewBassLine("EDM", 4); // one held root per bar, I-IV-V-I

            const auto output = render(processor, 16);

            for (int bar = 0; bar < 4; ++bar)
            {
                const int barStart = bar * 4 * samplesPerBeat;
                const double frequency = frequencyBetween(output, barStart + samplesPerBeat,
                                                          barStart + 3 * samplesPerBeat);

                // Octave 2 (C2 = 65 Hz): the I, IV and V roots all sit between C2 and C3
                expect(frequency > 60.0 && frequency < 135.0,
                       "Key " + juce::String(key) + " bar " + juce::String(bar + 1)
                           + " plays at " + juce::String(frequency, 1) + " Hz");
            }
        }
    }

    void testStateRoundTrip()
    {
        beginTest("Saved settings and bass line are restored");

        BassLineProcessor original;
        original.setWaveform(1);
        original.setAttack(0.02f);
        original.setDecay(0.3f);
        original.setSustain(0.5f);
        original.setRelease(0.4f);
        original.setFilterCutoff(1500.0f);
        original.setOctave(1);
        original.setKey(48 + 9, false);
        original.setReverbMix(0.3f);
        original.setDelayMix(0.25f);
        original.generateNewBassLine("Funk", 4);

        juce::MemoryBlock state;
        original.getStateInformation(state);

        BassLineProcessor restored;
        restored.setStateInformation(state.getData(), static_cast<int>(state.getSize()));

        expectEquals(restored.getWaveform(), 1);
        expectEquals(restored.getAttack(), 0.02f);
        expectEquals(restored.getDecay(), 0.3f);
        expectEquals(restored.getSustain(), 0.5f);
        expectEquals(restored.getRelease(), 0.4f);
        expectEquals(restored.getFilterCutoff(), 1500.0f);
        expectEquals(restored.getOctave(), 1);
        expectEquals(restored.getKeyRoot() % 12, 9);
        expect(!restored.getKeyIsMajor(), "Minor key should be restored");
        expectEquals(restored.getReverbMix(), 0.3f);
        expectEquals(restored.getDelayMix(), 0.25f);
        expectEquals(juce::String(restored.getStyle()), juce::String("Funk"));

        // The generated (and humanised) line itself comes back, so it sounds the same
        const auto expected = render(original, 16);
        const auto actual = render(restored, 16);
        expectEquals(static_cast<int>(actual.size()), static_cast<int>(expected.size()));

        float maxDifference = 0.0f, peak = 0.0f;
        for (size_t i = 0; i < expected.size() && i < actual.size(); ++i)
        {
            maxDifference = std::max(maxDifference, std::abs(expected[i] - actual[i]));
            peak = std::max(peak, std::abs(expected[i]));
        }
        expect(peak > 0.01f, "The bass line should be audible");
        expect(maxDifference < 1.0e-6f,
               "Restored session should sound the same, max difference " + juce::String(maxDifference));
    }

    void testIgnoresStateItCannotRead()
    {
        beginTest("A state it cannot read leaves the settings alone");

        BassLineProcessor processor;
        processor.setWaveform(2);
        processor.setFilterCutoff(400.0f);
        const auto lineBefore = processor.getCurrentBassLine().size();

        const char junk[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
        processor.setStateInformation(junk, sizeof(junk));
        processor.setStateInformation(nullptr, 0);

        juce::XmlElement other("SomeOtherPlugin");
        juce::MemoryBlock otherState;
        juce::AudioProcessor::copyXmlToBinary(other, otherState);
        processor.setStateInformation(otherState.getData(), static_cast<int>(otherState.getSize()));

        expectEquals(processor.getWaveform(), 2);
        expectEquals(processor.getFilterCutoff(), 400.0f);
        expectEquals(static_cast<int>(processor.getCurrentBassLine().size()), static_cast<int>(lineBefore));
    }

    // A project file is outside the plugin's control: a damaged or hand-edited one must not put
    // out-of-range notes, timings or settings into the audio thread, or allocate without bound
    void testDamagedSessionIsMadeSafe()
    {
        beginTest("A damaged session is clamped to safe values");

        juce::XmlElement xml("BassLineState");
        xml.setAttribute("attack", -5.0);
        xml.setAttribute("decay", 1.0e9);
        xml.setAttribute("sustain", 7.0);
        xml.setAttribute("release", "nan");
        xml.setAttribute("filterCutoff", 1.0e12);
        xml.setAttribute("reverbMix", -1.0);
        xml.setAttribute("delayMix", 50.0);
        xml.setAttribute("keyRoot", 5000);
        xml.setAttribute("style", "<not a style>");
        auto* notes = xml.createNewChildElement("Notes");
        // pitch, start, duration, velocity: two with usable timings, two with impossible ones
        const double rows[][4] = { { -40, 1.0, 0.5, 999 }, { 300, 2.0, 1.0e30, -7 },
                                   { 64, std::nan(""), 1.0, 64 }, { 64, -3.0, -1.0, 64 } };
        for (int i = 0; i < 100000; ++i)
        {
            auto* n = notes->createNewChildElement("Note");
            n->setAttribute("pitch", static_cast<int>(rows[i % 4][0]));
            n->setAttribute("start", rows[i % 4][1]);
            n->setAttribute("duration", rows[i % 4][2]);
            n->setAttribute("velocity", static_cast<int>(rows[i % 4][3]));
        }
        juce::MemoryBlock damaged;
        juce::AudioProcessor::copyXmlToBinary(xml, damaged);

        BassLineProcessor processor;
        processor.setStateInformation(damaged.getData(), static_cast<int>(damaged.getSize()));

        auto inRange = [](float v, float lo, float hi) { return std::isfinite(v) && v >= lo && v <= hi; };
        expect(inRange(processor.getAttack(), 0.001f, 0.5f), "Attack " + juce::String(processor.getAttack()));
        expect(inRange(processor.getDecay(), 0.001f, 1.0f), "Decay " + juce::String(processor.getDecay()));
        expect(inRange(processor.getSustain(), 0.0f, 1.0f), "Sustain " + juce::String(processor.getSustain()));
        expect(inRange(processor.getRelease(), 0.001f, 1.0f), "Release " + juce::String(processor.getRelease()));
        expect(inRange(processor.getFilterCutoff(), 100.0f, 2000.0f), "Cutoff " + juce::String(processor.getFilterCutoff()));
        expect(inRange(processor.getReverbMix(), 0.0f, 1.0f), "Reverb mix " + juce::String(processor.getReverbMix()));
        expect(inRange(processor.getDelayMix(), 0.0f, 1.0f), "Delay mix " + juce::String(processor.getDelayMix()));
        expect(processor.getKeyRoot() >= 0 && processor.getKeyRoot() <= 127, "Key root " + juce::String(processor.getKeyRoot()));
        expectEquals(juce::String(processor.getStyle()), juce::String("Rock"), "An unknown style should be ignored");

        const auto line = processor.getCurrentBassLine();
        expect(! line.empty(), "Notes with usable timings should be kept, clamped");
        expect(line.size() <= 4096, "The line should be capped, not " + juce::String((int) line.size()) + " notes");
        for (const auto& note : line)
        {
            expect(note.pitch >= 0 && note.pitch <= 127, "Pitch out of range: " + juce::String(note.pitch));
            expect(note.velocity >= 1 && note.velocity <= 127, "Velocity out of range: " + juce::String(note.velocity));
            expect(std::isfinite(note.startTime) && note.startTime >= 0.0f, "Bad start time");
            expect(std::isfinite(note.duration) && note.duration > 0.0f && note.duration <= 64.0f, "Bad duration");
        }
    }

    // Hosts may save, or load, from a background thread while the line is being replaced
    void testSavingWhileTheLineChanges()
    {
        beginTest("Saving on another thread while the bass line is replaced is safe");

        BassLineProcessor processor;
        std::atomic<bool> done { false };
        std::atomic<int> saves { 0 };
        std::thread saver([&] {
            while (! done.load())
            {
                juce::MemoryBlock state;
                processor.getStateInformation(state);
                saves.fetch_add(1);
            }
        });

        const char* styles[] = { "Rock", "Funk", "Jazz", "EDM" };
        for (int i = 0; i < 2000; ++i)
            processor.generateNewBassLine(styles[i % 4], 4);

        done.store(true);
        saver.join();
        expect(saves.load() > 0, "The saving thread should have run");
    }

    void testOpeningEditorKeepsSession()
    {
        beginTest("The editor shows the session's settings");

        BassLineProcessor processor;
        processor.setWaveform(3);
        processor.setFilterCutoff(1200.0f);
        processor.setOctave(3);
        processor.setKey(48 + 7, false);
        processor.generateNewBassLine("Jazz", 4);

        juce::MemoryBlock before;
        processor.getStateInformation(before);

        std::unique_ptr<juce::AudioProcessorEditor> editor(processor.createEditor());

        juce::StringArray selections;
        bool showsCutoff = false;
        for (auto* child : editor->getChildren())
        {
            if (auto* box = dynamic_cast<juce::ComboBox*>(child))
                selections.add(box->getText());
            else if (auto* slider = dynamic_cast<juce::Slider*>(child))
                showsCutoff = showsCutoff || slider->getValue() == 1200.0;
        }

        for (auto* expected : { "Jazz", "G", "Minor", "C3 (High)", "Triangle (Warm)" })
            expect(selections.contains(expected), juce::String("Editor should show ") + expected
                                                      + ", shows " + selections.joinIntoString(", "));
        expect(showsCutoff, "Editor should show the 1200 Hz filter cutoff");

        editor.reset();

        juce::MemoryBlock after;
        processor.getStateInformation(after);
        expect(before == after, "Opening the editor should not change the session");
    }
};

static BassLinePluginTests bassLinePluginTests;
