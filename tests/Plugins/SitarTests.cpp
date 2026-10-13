/**
 * Sitar Tests
 *
 * Plays the Sitar through its processor and listens to what comes out:
 * - the Decay knob sets how long the string rings
 * - a released note fades out from the moment it is released
 * - the string darkens as it rings, the way a real plucked string does
 * - the tanpura drone starts with the note, even a short one
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <cmath>
#include <map>
#include <vector>

#include "../../plugins/Sitar/Source/PluginProcessor.h"

class SitarTests : public juce::UnitTest
{
public:
    SitarTests()
        : juce::UnitTest("Sitar", "Plugins")
    {
    }

    void runTest() override
    {
        testDecaySetsRingTime();
        testReleaseStartsAtNoteOff();
        testStringDarkensAsItRings();
        testDroneStartsWithTheNote();
    }

private:
    static constexpr double sampleRate = 48000.0;
    static constexpr int blockSize = 256;

    struct Note
    {
        double onSeconds;
        double offSeconds;  // negative: held to the end
        int noteNumber;
    };

    // The main string on its own: no sympathetic strings, no drone, no buzz
    static std::map<juce::String, float> bareString(float decay)
    {
        return { { "decay", decay }, { "sympathetic", 0.0f }, { "droneLevel", 0.0f }, { "jawari", 0.0f } };
    }

    // Renders the given notes and returns the left channel
    std::vector<float> render(const std::map<juce::String, float>& params,
                              const std::vector<Note>& notes, double seconds)
    {
        SitarProcessor processor;

        for (const auto& [id, value] : params)
        {
            auto* param = processor.getValueTreeState().getParameter(id);
            expect(param != nullptr, "Sitar has no parameter " + id);
            if (param != nullptr)
                param->setValueNotifyingHost(param->convertTo0to1(value));
        }

        processor.setRateAndBufferSizeDetails(sampleRate, blockSize);
        processor.prepareToPlay(sampleRate, blockSize);

        const int totalSamples = static_cast<int>(seconds * sampleRate);
        std::vector<float> out;
        out.reserve(static_cast<size_t>(totalSamples));
        juce::AudioBuffer<float> buffer(2, blockSize);

        for (int start = 0; start < totalSamples; start += blockSize)
        {
            juce::MidiBuffer midi;
            auto addIfInBlock = [&](double when, const juce::MidiMessage& message)
            {
                const int at = static_cast<int>(when * sampleRate) - start;
                if (when >= 0.0 && at >= 0 && at < blockSize)
                    midi.addEvent(message, at);
            };

            for (const auto& note : notes)
            {
                addIfInBlock(note.onSeconds, juce::MidiMessage::noteOn(1, note.noteNumber, (juce::uint8) 100));
                addIfInBlock(note.offSeconds, juce::MidiMessage::noteOff(1, note.noteNumber));
            }

            buffer.clear();
            processor.processBlock(buffer, midi);

            for (int i = 0; i < blockSize && start + i < totalSamples; ++i)
                out.push_back(buffer.getSample(0, i));
        }

        return out;
    }

    // RMS level of a stretch of the signal, leaving out its DC offset: a
    // pluck's noise burst leaves a little DC circulating in the string
    static float rms(const std::vector<float>& x, double fromSeconds, double toSeconds)
    {
        const auto from = static_cast<size_t>(fromSeconds * sampleRate);
        const auto to = std::min(x.size(), static_cast<size_t>(toSeconds * sampleRate));
        const double count = static_cast<double>(std::max<size_t>(1, to - from));
        double sum = 0.0, sumOfSquares = 0.0;
        for (size_t i = from; i < to; ++i)
        {
            sum += x[i];
            sumOfSquares += x[i] * x[i];
        }
        const double mean = sum / count;
        return static_cast<float>(std::sqrt(std::max(0.0, sumOfSquares / count - mean * mean)));
    }

    // Level of the sample-to-sample differences relative to the level of the
    // signal: rises with the share of high harmonics
    static float brightness(const std::vector<float>& x, double fromSeconds, double toSeconds)
    {
        const auto from = static_cast<size_t>(fromSeconds * sampleRate);
        const auto to = std::min(x.size(), static_cast<size_t>(toSeconds * sampleRate));
        double signal = 0.0, difference = 0.0;
        for (size_t i = from + 1; i < to; ++i)
        {
            signal += x[i] * x[i];
            difference += (x[i] - x[i - 1]) * (x[i] - x[i - 1]);
        }
        return signal > 0.0 ? static_cast<float>(std::sqrt(difference / signal)) : 0.0f;
    }

    // The signal through four one-pole lowpasses at 250 Hz: keeps the
    // fundamental of an A3, strips its harmonics
    static std::vector<float> lowpass(std::vector<float> x)
    {
        const float a = std::exp(-juce::MathConstants<float>::twoPi * 250.0f / static_cast<float>(sampleRate));
        for (int pass = 0; pass < 4; ++pass)
        {
            float y = 0.0f;
            for (auto& sample : x)
                sample = y = (1.0f - a) * sample + a * y;
        }
        return x;
    }

    static float db(float ratio) { return juce::Decibels::gainToDecibels(ratio, -200.0f); }

    void testDecaySetsRingTime()
    {
        beginTest("Decay sets how long a held note rings");

        const std::vector<Note> held { { 0.0, -1.0, 57 } };
        const auto shortDecay = render(bareString(0.2f), held, 1.5);
        const auto longDecay = render(bareString(2.0f), held, 1.5);

        const float shortLate = rms(shortDecay, 1.0, 1.2);
        const float longLate = rms(longDecay, 1.0, 1.2);
        expect(rms(longDecay, 0.0, 0.1) > 0.001f, "The Sitar made no sound");
        expect(db(longLate / std::max(shortLate, 1.0e-9f)) > 20.0f,
               "A second in, Decay at 2.0 should ring far louder than Decay at 0.2, got "
                   + juce::String(db(longLate / std::max(shortLate, 1.0e-9f)), 1) + " dB louder");

        // Decay is the time, in seconds, for the string to fall 20 dB. The
        // high harmonics fall faster, so measure the fundamental.
        const auto halfSecond = lowpass(render(bareString(0.5f), held, 1.0));
        const float drop = db(rms(halfSecond, 0.6, 0.7) / rms(halfSecond, 0.1, 0.2));
        expect(drop < -16.0f && drop > -24.0f,
               "With Decay at 0.5 the string should fall about 20 dB in half a second, fell "
                   + juce::String(-drop, 1) + " dB");
    }

    void testReleaseStartsAtNoteOff()
    {
        beginTest("A released note fades from the moment of release, not from note-on");

        const auto out = render(bareString(2.0f), { { 0.0, 2.0, 57 } }, 4.0);

        const float before = rms(out, 1.95, 2.0);
        const float justAfter = rms(out, 2.0, 2.05);
        expect(before > 0.0001f, "The held note had died before it was released");
        expect(db(justAfter / std::max(before, 1.0e-9f)) > -6.0f,
               "Releasing the note should start a fade, not cut it: the first 50 ms after release were "
                   + juce::String(db(justAfter / std::max(before, 1.0e-9f)), 1) + " dB below the 50 ms before");

        expect(rms(out, 3.5, 4.0) < 1.0e-5f, "The note should have faded out within 1.5 s of release");
    }

    void testStringDarkensAsItRings()
    {
        beginTest("The string loses its high harmonics as it rings");

        const auto out = render(bareString(2.0f), { { 0.0, -1.0, 57 } }, 1.5);
        const float early = brightness(out, 0.02, 0.12);
        const float late = brightness(out, 1.2, 1.3);
        expect(late < early * 0.7f,
               "The tone should darken: brightness went from " + juce::String(early, 3)
                   + " to " + juce::String(late, 3));
    }

    void testDroneStartsWithTheNote()
    {
        beginTest("The drone sounds with the note, even a short one");

        // A short note, released after 0.15 s. The slowest attack keeps the
        // note itself quiet for its first few milliseconds.
        const std::vector<Note> shortNote { { 0.0, 0.15, 69 } };
        auto params = bareString(0.7f);
        params["attack"] = 0.1f;
        const auto silentDrone = render(params, shortNote, 2.0);
        params["droneLevel"] = 1.0f;
        const auto fullDrone = render(params, shortNote, 2.0);

        const float addedDb = db(rms(fullDrone, 0.0, 0.02) / std::max(rms(silentDrone, 0.0, 0.02), 1.0e-9f));
        expect(addedDb > 12.0f,
               "The drone should be heard within 20 ms of the note, it added " + juce::String(addedDb, 1) + " dB");
        expect(rms(fullDrone, 1.5, 2.0) > 1.0e-4f, "The drone should ring on after a short note ends");
        expect(rms(silentDrone, 1.5, 2.0) < 1.0e-5f, "With the drone off the short note should be over");

        beginTest("The drone does not play without a note");

        const auto noNotes = render(params, {}, 1.0);
        expect(rms(noNotes, 0.0, 1.0) == 0.0f, "The Sitar made sound with no notes played");
    }
};

static SitarTests sitarTests;
