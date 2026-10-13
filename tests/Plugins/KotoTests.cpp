/**
 * Koto Tests
 *
 * Plays the Koto processor through its public API, MIDI in and audio out:
 * - The Scale control moves notes onto the chosen scale
 * - Releasing a long-held note fades it out instead of cutting it off
 * - The MIDI pitch wheel bends the note
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <cmath>
#include <vector>

#include "../../plugins/Koto/Source/PluginProcessor.h"

class KotoTests : public juce::UnitTest
{
public:
    KotoTests()
        : juce::UnitTest("Koto", "Plugins")
    {
    }

    void runTest() override
    {
        testScaleSnapsNotes();
        testReleaseCountsFromNoteOff();
        testPitchWheelBendsNote();
    }

private:
    static constexpr double sampleRate = 48000.0;
    static constexpr int blockSize = 512;

    struct Player
    {
        KotoProcessor processor;
        std::vector<float> output;

        Player()
        {
            processor.setPlayConfigDetails(0, 2, sampleRate, blockSize);
            processor.prepareToPlay(sampleRate, blockSize);
        }

        void setParameter(const char* id, float value)
        {
            auto* param = processor.getValueTreeState().getParameter(id);
            param->setValueNotifyingHost(param->convertTo0to1(value));
        }

        // Renders the given time, with any MIDI placed at the start of it
        void render(double seconds, juce::MidiBuffer midi = {})
        {
            juce::AudioBuffer<float> buffer(2, blockSize);
            const int total = static_cast<int>(seconds * sampleRate);

            for (int done = 0; done < total; done += blockSize)
            {
                processor.processBlock(buffer, midi);
                midi.clear();
                output.insert(output.end(), buffer.getReadPointer(0), buffer.getReadPointer(0) + blockSize);
            }
        }

        static juce::MidiBuffer message(const juce::MidiMessage& m)
        {
            juce::MidiBuffer midi;
            midi.addEvent(m, 0);
            return midi;
        }

        float rms(double fromSeconds, double toSeconds) const
        {
            const auto from = static_cast<size_t>(fromSeconds * sampleRate);
            const auto to = std::min(output.size(), static_cast<size_t>(toSeconds * sampleRate));
            double sum = 0.0;

            for (auto i = from; i < to; ++i)
                sum += output[i] * output[i];

            return to > from ? static_cast<float>(std::sqrt(sum / static_cast<double>(to - from))) : 0.0f;
        }

        // Pitch of the output from a given time, by autocorrelation: the first
        // lag that correlates nearly as well as the best one, so a period of
        // two cycles is not taken for one
        float pitchFrom(double fromSeconds) const
        {
            constexpr int window = 4096;
            const auto start = static_cast<size_t>(fromSeconds * sampleRate);
            const int minLag = static_cast<int>(sampleRate / 1000.0);
            const int maxLag = static_cast<int>(sampleRate / 100.0);

            std::vector<double> correlation(static_cast<size_t>(maxLag + 1), 0.0);
            double best = 0.0;

            for (int lag = minLag; lag <= maxLag; ++lag)
            {
                double sum = 0.0;
                for (int i = 0; i < window; ++i)
                    sum += output[start + static_cast<size_t>(i)] * output[start + static_cast<size_t>(i + lag)];
                correlation[static_cast<size_t>(lag)] = sum;
                best = std::max(best, sum);
            }

            for (int lag = minLag + 1; lag < maxLag; ++lag)
            {
                const double c = correlation[static_cast<size_t>(lag)];
                if (c >= 0.9 * best && c >= correlation[static_cast<size_t>(lag - 1)]
                    && c >= correlation[static_cast<size_t>(lag + 1)])
                    return static_cast<float>(sampleRate / lag);
            }

            return 0.0f;
        }
    };

    static constexpr int chromatic = 4;
    static constexpr int inSen = 0;

    float pitchOfNote(int note, int scale)
    {
        Player player;
        player.setParameter("scale", static_cast<float>(scale));
        player.render(0.3, Player::message(juce::MidiMessage::noteOn(1, note, 1.0f)));
        return player.pitchFrom(0.1);
    }

    void testScaleSnapsNotes()
    {
        beginTest("Scale moves notes onto the chosen scale");

        // In-sen is E F A B D. C#4 is not in it, and its nearest note is D4.
        const float cSharp = pitchOfNote(61, chromatic);
        const float d = pitchOfNote(62, chromatic);
        const float cSharpInSen = pitchOfNote(61, inSen);
        const float aInSen = pitchOfNote(69, inSen);
        const float a = pitchOfNote(69, chromatic);

        expect(std::abs(cSharp - d) > 10.0f, "Chromatic should keep C# and D apart: "
                                                 + juce::String(cSharp) + " vs " + juce::String(d));
        expectWithinAbsoluteError(cSharpInSen, d, 2.0f, "In-sen should play C# as D");
        expectWithinAbsoluteError(aInSen, a, 2.0f, "In-sen should leave A, which is in the scale, alone");
    }

    void testReleaseCountsFromNoteOff()
    {
        beginTest("Releasing a long-held note fades it out");

        // Regression test: the release envelope counted from note-on, so a note
        // held for a second or more was already at nothing when it was let go.
        Player player;
        player.setParameter("scale", static_cast<float>(chromatic));
        player.render(1.5, Player::message(juce::MidiMessage::noteOn(1, 45, 1.0f)));
        player.render(1.5, Player::message(juce::MidiMessage::noteOff(1, 45)));

        const float held = player.rms(1.4, 1.5);
        const float justReleased = player.rms(1.51, 1.56);
        const float longAfter = player.rms(2.9, 3.0);

        expect(held > 0.001f, "The held note should still sound: " + juce::String(held));
        expect(justReleased > held * 0.3f, "The note should fade, not stop, on release: "
                                               + juce::String(held) + " -> " + juce::String(justReleased));
        expect(longAfter < held * 0.001f, "The note should have died away 1.5 s after release: "
                                               + juce::String(longAfter));
    }

    void testPitchWheelBendsNote()
    {
        beginTest("MIDI pitch wheel bends the note");

        Player player;
        player.setParameter("scale", static_cast<float>(chromatic));
        player.render(0.3, Player::message(juce::MidiMessage::noteOn(1, 57, 1.0f)));
        player.render(0.3, Player::message(juce::MidiMessage::pitchWheel(1, 16383)));

        const float before = player.pitchFrom(0.1);
        const float bent = player.pitchFrom(0.4);
        const float wholeTone = std::pow(2.0f, 2.0f / 12.0f);

        expect(before > 0.0f, "The note should have a pitch");
        expectWithinAbsoluteError(bent / before, wholeTone, 0.02f,
                                  "Full wheel up should bend a whole tone: " + juce::String(before)
                                      + " Hz -> " + juce::String(bent) + " Hz");
    }
};

static KotoTests kotoTests;
