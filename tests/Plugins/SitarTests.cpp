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
#include <complex>
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
        testDecayHoldsAtEveryPitchAndRate();
        testPitchIsInTune();
        testVeryLowNoteStaysInTheBuffer();
        testResetSilencesTheDrone();
    }

private:
    // Every render runs at this rate and block size; tests change them
    double sampleRate = 48000.0;
    int blockSize = 256;

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

    // Renders the given notes on a fresh Sitar and returns the left channel
    std::vector<float> render(const std::map<juce::String, float>& params,
                              const std::vector<Note>& notes, double seconds)
    {
        SitarProcessor processor;
        setUp(processor, params);
        return play(processor, notes, seconds);
    }

    void setUp(SitarProcessor& processor, const std::map<juce::String, float>& params)
    {
        for (const auto& [id, value] : params)
        {
            auto* param = processor.getValueTreeState().getParameter(id);
            expect(param != nullptr, "Sitar has no parameter " + id);
            if (param != nullptr)
                param->setValueNotifyingHost(param->convertTo0to1(value));
        }

        processor.setRateAndBufferSizeDetails(sampleRate, blockSize);
        processor.prepareToPlay(sampleRate, blockSize);
    }

    // Plays the given notes, timed from the start of this call, and returns
    // the left channel
    std::vector<float> play(SitarProcessor& processor, const std::vector<Note>& notes, double seconds)
    {
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
    float rms(const std::vector<float>& x, double fromSeconds, double toSeconds)
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
    float brightness(const std::vector<float>& x, double fromSeconds, double toSeconds)
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
    std::vector<float> lowpass(std::vector<float> x)
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

    // Complex amplitude of the frequency `hz` in a Hann-windowed stretch of
    // the signal, with its phase measured from time zero
    std::complex<double> partial(const std::vector<float>& x, double hz, double fromSeconds, double lengthSeconds) const
    {
        const auto from = static_cast<size_t>(fromSeconds * sampleRate);
        const auto count = static_cast<size_t>(lengthSeconds * sampleRate);
        std::complex<double> sum;
        for (size_t i = 0; i < count && from + i < x.size(); ++i)
        {
            const double window = 0.5 - 0.5 * std::cos(juce::MathConstants<double>::twoPi * static_cast<double>(i) / static_cast<double>(count));
            const double phase = -juce::MathConstants<double>::twoPi * hz * static_cast<double>(from + i) / sampleRate;
            sum += window * static_cast<double>(x[from + i]) * std::polar(1.0, phase);
        }
        return sum;
    }

    // The frequency actually sounding near `hz`, from how far the phase of
    // that partial moves in 5 ms
    double frequencyNear(const std::vector<float>& x, double hz, double fromSeconds) const
    {
        const auto step = static_cast<size_t>(0.005 * sampleRate);
        const double from = std::floor(fromSeconds * sampleRate) / sampleRate;
        const double later = from + static_cast<double>(step) / sampleRate;
        const double turned = std::arg(partial(x, hz, later + 0.5 / sampleRate, 0.05)
                                       / partial(x, hz, from + 0.5 / sampleRate, 0.05));
        return hz + turned * sampleRate / (juce::MathConstants<double>::twoPi * static_cast<double>(step));
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
        // The darkening is set in hertz, so it is the same at every rate
        for (double rate : { 44100.0, 48000.0, 192000.0 })
        {
            beginTest("The string loses its high harmonics as it rings, at " + juce::String(rate / 1000.0, 1) + " kHz");
            sampleRate = rate;

            const auto out = render(bareString(2.0f), { { 0.0, -1.0, 57 } }, 1.5);
            const float early = brightness(out, 0.02, 0.12);
            const float late = brightness(out, 1.2, 1.3);
            expect(late < early * 0.7f,
                   "The tone should darken: brightness went from " + juce::String(early, 3)
                       + " to " + juce::String(late, 3));

            // The 8th harmonic of the A3 (1760 Hz) loses about 9 dB a second
            // against the fundamental
            auto level = [&](double hz, double from) { return std::abs(partial(out, hz, from, 0.1)); };
            const float fade = db(static_cast<float>((level(1760.0, 1.1) / level(1760.0, 0.1))
                                                     / (level(220.0, 1.1) / level(220.0, 0.1))));
            expect(fade < -6.0f && fade > -13.0f,
                   "The 8th harmonic should fade about 9 dB a second faster than the fundamental, faded "
                       + juce::String(-fade, 1) + " dB");
        }
        sampleRate = 48000.0;
    }

    void testDroneStartsWithTheNote()
    {
        for (int size : { 256, 37, 1 })
        {
            blockSize = size;
            testDroneStartsWithTheNoteAtThisBlockSize();
        }
        blockSize = 256;
    }

    void testDroneStartsWithTheNoteAtThisBlockSize()
    {
        beginTest("The drone sounds with the note, even a short one, in blocks of " + juce::String(blockSize));

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

    void testDecayHoldsAtEveryPitchAndRate()
    {
        // Decay at 2.0 is 20 dB in 2 s, so the fundamental should fall 9 dB
        // in the 0.9 s between the two measurements, high notes included
        const std::vector<int> notes { 57, 72, 84, 96 };
        for (double rate : { 44100.0, 192000.0 })
        {
            sampleRate = rate;
            for (int note : notes)
            {
                beginTest("Decay is the same for note " + juce::String(note) + " at " + juce::String(rate / 1000.0, 1) + " kHz");

                const auto out = render(bareString(2.0f), { { 0.0, -1.0, note } }, 1.2);
                const double hz = juce::MidiMessage::getMidiNoteInHertz(note);
                const float drop = db(static_cast<float>(std::abs(partial(out, hz, 1.0, 0.1))
                                                         / std::max(std::abs(partial(out, hz, 0.1, 0.1)), 1.0e-12)));
                expect(drop < -8.0f && drop > -10.0f,
                       "The fundamental should fall 9 dB in 0.9 s, fell " + juce::String(-drop, 1) + " dB");
            }
        }
        sampleRate = 48000.0;
    }

    void testPitchIsInTune()
    {
        const std::vector<int> notes { 57, 72, 84, 96 };
        for (double rate : { 44100.0, 192000.0 })
        {
            sampleRate = rate;
            for (int note : notes)
            {
                beginTest("Note " + juce::String(note) + " is in tune at " + juce::String(rate / 1000.0, 1) + " kHz");

                const auto out = render(bareString(2.0f), { { 0.0, -1.0, note } }, 0.5);
                const double hz = juce::MidiMessage::getMidiNoteInHertz(note);
                const double cents = 1200.0 * std::log2(frequencyNear(out, hz, 0.3) / hz);
                expect(std::abs(cents) < 2.0, "Off by " + juce::String(cents, 1) + " cents");
            }
        }
        sampleRate = 48000.0;
    }

    void testVeryLowNoteStaysInTheBuffer()
    {
        beginTest("A note longer than the delay buffer is shortened to fit it");

        // MIDI note 0 bent down two semitones at 192 kHz is a period of about
        // 26,000 samples, more than the string's buffer holds
        SitarEngine::CombFilter string;
        string.tune(7.3f, 0.1f, 2.0f, 192000.0);
        expect(string.delayLength <= static_cast<float>(SitarEngine::DELAY_BUFFER_SIZE - 2),
               "Delay of " + juce::String(string.delayLength) + " samples overruns the buffer");

        sampleRate = 192000.0;
        auto params = bareString(2.0f);
        params["pitchBend"] = -2.0f;
        params["sympathetic"] = 1.0f;
        const auto out = render(params, { { 0.0, -1.0, 0 } }, 0.5);
        bool finite = true;
        for (float sample : out)
            finite = finite && std::isfinite(sample);
        expect(finite, "The lowest note made a non-finite sample");
        sampleRate = 48000.0;
    }

    void testResetSilencesTheDrone()
    {
        beginTest("Resetting the processor stops the notes and the drone");

        SitarProcessor processor;
        auto params = bareString(2.0f);
        params["droneLevel"] = 1.0f;
        setUp(processor, params);

        const auto before = play(processor, { { 0.0, -1.0, 57 } }, 0.5);
        expect(rms(before, 0.4, 0.5) > 1.0e-3f, "The note and drone made no sound");

        processor.reset();
        const auto after = play(processor, {}, 0.5);
        expect(rms(after, 0.0, 0.5) == 0.0f,
               "After reset the Sitar still sounded at " + juce::String(db(rms(after, 0.0, 0.5)), 1) + " dBFS");
    }
};

static SitarTests sitarTests;
