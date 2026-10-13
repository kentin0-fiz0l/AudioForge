/**
 * Koto Tests
 *
 * Plays the Koto processor through its public API, MIDI in and audio out:
 * - The Scale control moves notes onto the chosen scale
 * - New instances start on Chromatic; saved sessions keep their scale
 * - Releasing a long-held note fades it out instead of cutting it off
 * - The MIDI pitch wheel bends the note
 * - High notes are in tune and bend by fine steps, not whole samples
 * - The release takes the same time at every sample rate
 * - Sweeping the wheel over its whole range stays sane on any note and rate
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
        testNewInstancesPlayNotesAsWritten();
        testReleaseCountsFromNoteOff();
        testPitchWheelBendsNote();
        testHighNotesAreInTune();
        testWheelBendsHighNoteFinely();
        testReleaseTimeIsTheSameAtEveryRate();
        testWheelSweepStaysSaneEverywhere();
        testLowNoteAtHighRateIsInTune();
    }

private:
    struct Player
    {
        KotoProcessor processor;
        std::vector<float> output;
        const double sampleRate;
        const int blockSize;

        explicit Player(double rate = 48000.0, int block = 512)
            : sampleRate(rate), blockSize(block)
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

        size_t index(double seconds) const
        {
            return std::min(output.size(), static_cast<size_t>(seconds * sampleRate));
        }

        float rms(double fromSeconds, double toSeconds) const
        {
            const auto from = index(fromSeconds);
            const auto to = index(toSeconds);
            double sum = 0.0;

            for (auto i = from; i < to; ++i)
                sum += output[i] * output[i];

            return to > from ? static_cast<float>(std::sqrt(sum / static_cast<double>(to - from))) : 0.0f;
        }

        float peak() const
        {
            float result = 0.0f;
            for (float sample : output)
                result = std::max(result, std::abs(sample));
            return result;
        }

        // Pitch of the output from a given time, by autocorrelation: the first
        // lag that correlates nearly as well as the best one, so a period of
        // two cycles is not taken for one, refined between lags with a parabola
        float pitchFrom(double fromSeconds, double lowestHz = 100.0, double highestHz = 1000.0) const
        {
            const int window = static_cast<int>(sampleRate * 0.08);
            const auto start = static_cast<size_t>(fromSeconds * sampleRate);
            const int minLag = static_cast<int>(sampleRate / highestHz);
            const int maxLag = static_cast<int>(sampleRate / lowestHz);

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
                const double before = correlation[static_cast<size_t>(lag - 1)];
                const double after = correlation[static_cast<size_t>(lag + 1)];

                if (c >= 0.9 * best && c >= before && c >= after)
                {
                    const double curve = before - 2.0 * c + after;
                    const double offset = curve < 0.0 ? 0.5 * (before - after) / curve : 0.0;
                    return static_cast<float>(sampleRate / (lag + offset));
                }
            }

            return 0.0f;
        }
    };

    static constexpr int chromatic = 4;
    static constexpr int inSen = 0;

    static float cents(float ratio) { return 1200.0f * std::log2(ratio); }
    static float noteHz(int note) { return static_cast<float>(juce::MidiMessage::getMidiNoteInHertz(note)); }

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

    void testNewInstancesPlayNotesAsWritten()
    {
        beginTest("New instances start on Chromatic; a saved In-sen session stays In-sen");

        // A fresh Koto plays C#4 as C#4, not moved to D4
        Player fresh;
        fresh.render(0.3, Player::message(juce::MidiMessage::noteOn(1, 61, 1.0f)));
        expectWithinAbsoluteError(fresh.pitchFrom(0.1), pitchOfNote(61, chromatic), 2.0f,
                                  "A new instance should play notes as written");

        // A session saved with In-sen loads back as In-sen
        Player saved;
        saved.setParameter("scale", static_cast<float>(inSen));
        juce::MemoryBlock state;
        saved.processor.getStateInformation(state);

        Player loaded;
        loaded.processor.setStateInformation(state.getData(), static_cast<int>(state.getSize()));
        expectEquals(juce::roundToInt(loaded.processor.getValueTreeState().getRawParameterValue("scale")->load()), inSen,
                     "A saved In-sen session should keep In-sen");
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

        const float start = player.rms(0.0, 0.1);
        const float held = player.rms(1.4, 1.5);
        const float justReleased = player.rms(1.51, 1.56);
        const float longAfter = player.rms(2.9, 3.0);

        // The pluck is random noise, so the level is compared with the note's own start
        expect(held > start * 0.02f, "The held note should still sound: " + juce::String(start)
                                         + " at the start, " + juce::String(held) + " held");
        expect(justReleased > held * 0.3f, "The note should fade, not stop, on release: "
                                               + juce::String(held) + " -> " + juce::String(justReleased));
        expect(longAfter < held * 0.001f, "The note should have died away 1.5 s after release: "
                                               + juce::String(longAfter));
    }

    void testPitchWheelBendsNote()
    {
        beginTest("MIDI pitch wheel bends the note");

        for (int block : { 1, 512 })
        {
            Player player(48000.0, block);
            player.setParameter("scale", static_cast<float>(chromatic));
            player.render(0.3, Player::message(juce::MidiMessage::noteOn(1, 57, 1.0f)));
            player.render(0.3, Player::message(juce::MidiMessage::pitchWheel(1, 16383)));

            const float before = player.pitchFrom(0.1);
            const float bent = player.pitchFrom(0.4);
            const float wholeTone = std::pow(2.0f, 2.0f / 12.0f);

            expect(before > 0.0f, "The note should have a pitch");
            expectWithinAbsoluteError(bent / before, wholeTone, 0.02f,
                                      "Full wheel up should bend a whole tone (block " + juce::String(block)
                                          + "): " + juce::String(before) + " Hz -> " + juce::String(bent) + " Hz");
        }
    }

    void testHighNotesAreInTune()
    {
        beginTest("High notes are in tune");

        // Regression test: the string's delay was a whole number of samples,
        // so high notes went flat by up to a sample. A6 at 44.1 kHz is 25.06
        // samples and played as 26, 64 cents flat.
        for (double rate : { 44100.0, 48000.0 })
        {
            for (int note : { 81, 93, 96 })
            {
                Player player(rate);
                player.setParameter("scale", static_cast<float>(chromatic));
                player.render(0.3, Player::message(juce::MidiMessage::noteOn(1, note, 1.0f)));

                const float pitch = player.pitchFrom(0.1, 500.0, 4000.0);
                expectWithinAbsoluteError(cents(pitch / noteHz(note)), 0.0f, 6.0f,
                                          "Note " + juce::String(note) + " at " + juce::String(rate)
                                              + " Hz should be in tune: " + juce::String(pitch) + " Hz");
            }
        }
    }

    void testWheelBendsHighNoteFinely()
    {
        beginTest("A small wheel move bends a high note by a small amount");

        // Regression test: with a whole-sample delay, a high note bent in
        // jumps of a sample (60 cents or more), not by what the wheel asked
        Player player(44100.0);
        player.setParameter("scale", static_cast<float>(chromatic));
        player.render(0.3, Player::message(juce::MidiMessage::noteOn(1, 93, 1.0f)));
        player.render(0.3, Player::message(juce::MidiMessage::pitchWheel(1, 8192 + 1024)));

        const float bend = cents(player.pitchFrom(0.4, 500.0, 4000.0) / player.pitchFrom(0.1, 500.0, 4000.0));
        expectWithinAbsoluteError(bend, 25.0f, 8.0f, "An eighth of the wheel's travel should bend 25 cents: "
                                                         + juce::String(bend));
    }

    void testReleaseTimeIsTheSameAtEveryRate()
    {
        beginTest("The release takes the same time at every sample rate");

        // Regression test: the release shrank by a fixed factor every sample,
        // so it lasted 0.42 s at 44.1 kHz but 0.1 s at 192 kHz
        auto fadeAfterRelease = [](double rate)
        {
            Player player(rate);
            player.setParameter("scale", static_cast<float>(chromatic));
            player.setParameter("decay", 2.0f);
            player.render(0.5, Player::message(juce::MidiMessage::noteOn(1, 45, 1.0f)));
            player.render(0.5, Player::message(juce::MidiMessage::noteOff(1, 45)));
            return player.rms(0.6, 0.65) / player.rms(0.45, 0.5);
        };

        const float at44 = fadeAfterRelease(44100.0);

        for (double rate : { 48000.0, 96000.0, 192000.0 })
        {
            const float fade = fadeAfterRelease(rate);
            expect(fade > at44 * 0.5f && fade < at44 * 2.0f,
                   "0.1 s after release, the level at " + juce::String(rate) + " Hz should match 44.1 kHz: "
                       + juce::String(fade) + " vs " + juce::String(at44));
        }
    }

    void testWheelSweepStaysSaneEverywhere()
    {
        beginTest("Sweeping the wheel over its whole range stays sane on any note and rate");

        // Security regression: a low note at a high rate needed a longer delay
        // than the buffer held, and the read index left the buffer
        for (double rate : { 44100.0, 48000.0, 88200.0, 96000.0, 192000.0 })
        {
            for (int note : { 0, 12, 24, 60, 108, 127 })
            {
                for (float pitchBend : { -2.0f, 2.0f })
                {
                    Player player(rate, 64);
                    player.setParameter("scale", static_cast<float>(chromatic));
                    player.setParameter("pitchBend", pitchBend);
                    player.setParameter("playStyle", 2.0f);  // Scrape keeps exciting the string
                    player.render(0.01, Player::message(juce::MidiMessage::noteOn(1, note, 1.0f)));

                    for (int wheel = 0; wheel <= 16384; wheel += 512)
                        player.render(0.003, Player::message(juce::MidiMessage::pitchWheel(1, juce::jmin(wheel, 16383))));
                    for (int wheel : { 0, 16383, 0, 8192 })
                        player.render(0.02, Player::message(juce::MidiMessage::pitchWheel(1, wheel)));

                    bool finite = true;
                    for (float sample : player.output)
                        finite = finite && std::isfinite(sample);

                    const float peak = player.peak();
                    expect(finite && peak < 2.0f, "Note " + juce::String(note) + " at " + juce::String(rate)
                                                      + " Hz, bend " + juce::String(pitchBend)
                                                      + ": peak " + juce::String(peak));
                }
            }
        }
    }

    void testLowNoteAtHighRateIsInTune()
    {
        beginTest("A low note at 192 kHz, bent down, is in tune");

        // Regression test: C0 bent down 4 semitones needs a delay of about
        // 14800 samples at 192 kHz, more than the 8192-sample buffer held
        Player player(192000.0, 64);
        player.setParameter("scale", static_cast<float>(chromatic));
        player.setParameter("pitchBend", -2.0f);
        player.render(0.01, Player::message(juce::MidiMessage::pitchWheel(1, 0)));
        player.render(0.7, Player::message(juce::MidiMessage::noteOn(1, 12, 1.0f)));

        const float expected = noteHz(8);
        const float pitch = player.pitchFrom(0.3, 10.0, 100.0);
        expectWithinAbsoluteError(cents(pitch / expected), 0.0f, 10.0f,
                                  "Expected " + juce::String(expected) + " Hz, got " + juce::String(pitch));
    }
};

static KotoTests kotoTests;
