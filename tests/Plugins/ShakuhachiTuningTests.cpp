/**
 * Shakuhachi Tuning Tests
 *
 * Renders held notes through the Shakuhachi engine across its upper range and
 * checks the pitch of each one. The bore is a delay line, so a delay length
 * rounded to whole samples puts short (high) notes audibly out of tune.
 */

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <cmath>
#include <vector>

#include "../../plugins/Shakuhachi/Source/ShakuhachiEngine.h"

class ShakuhachiTuningTests : public juce::UnitTest
{
public:
    ShakuhachiTuningTests()
        : juce::UnitTest("Shakuhachi Tuning", "Plugins")
    {
    }

    void runTest() override
    {
        for (double sampleRate : { 44100.0, 48000.0 })
            testHeldNotesAreInTune(sampleRate);
    }

    // Pitch of a held note in cents relative to the MIDI note, from the peak
    // of an averaged (Welch) spectrum near the fundamental
    static double measureCentsError(double sampleRate, int midiNote)
    {
        ShakuhachiEngine engine;
        engine.prepareToPlay(sampleRate, 512);
        engine.setVibratoDepth(0.0f);
        engine.setEmbouchure(0.0f);

        const auto frequency = static_cast<float>(juce::MidiMessage::getMidiNoteInHertz(midiNote));
        const int settle = static_cast<int>(sampleRate * 0.5);
        const int length = static_cast<int>(sampleRate * 12.0);

        std::vector<float> signal(static_cast<size_t>(length));
        for (int i = 0; i < settle + length; ++i)
        {
            const float s = engine.processSample(frequency, 1.0f, true, i);
            if (i >= settle)
                signal[static_cast<size_t>(i - settle)] = s;
        }

        constexpr int fftOrder = 15;
        constexpr int fftSize = 1 << fftOrder;
        juce::dsp::FFT fft(fftOrder);
        juce::dsp::WindowingFunction<float> window(static_cast<size_t>(fftSize),
                                                   juce::dsp::WindowingFunction<float>::hann, false);

        std::vector<double> power(fftSize / 2 + 1, 0.0);
        std::vector<float> frame(static_cast<size_t>(fftSize * 2));

        for (int start = 0; start + fftSize <= length; start += fftSize / 4)
        {
            std::fill(frame.begin(), frame.end(), 0.0f);
            std::copy(signal.begin() + start, signal.begin() + start + fftSize, frame.begin());
            window.multiplyWithWindowingTable(frame.data(), static_cast<size_t>(fftSize));
            fft.performFrequencyOnlyForwardTransform(frame.data(), true);

            for (size_t bin = 0; bin < power.size(); ++bin)
                power[bin] += static_cast<double>(frame[bin]) * frame[bin];
        }

        // Search within a semitone of the expected fundamental
        const double binHz = sampleRate / fftSize;
        const int lo = static_cast<int>(frequency * std::pow(2.0, -1.0 / 12.0) / binHz);
        const int hi = static_cast<int>(frequency * std::pow(2.0, 1.0 / 12.0) / binHz) + 1;

        int peak = lo;
        for (int bin = lo; bin <= hi; ++bin)
            if (power[static_cast<size_t>(bin)] > power[static_cast<size_t>(peak)])
                peak = bin;

        // Parabolic interpolation on the log spectrum
        const double a = std::log(power[static_cast<size_t>(peak - 1)] + 1e-30);
        const double b = std::log(power[static_cast<size_t>(peak)] + 1e-30);
        const double c = std::log(power[static_cast<size_t>(peak + 1)] + 1e-30);
        const double denom = a - 2.0 * b + c;
        const double offset = denom != 0.0 ? 0.5 * (a - c) / denom : 0.0;

        const double measuredHz = (peak + offset) * binHz;
        return 1200.0 * std::log2(measuredHz / frequency);
    }

private:
    void testHeldNotesAreInTune(double sampleRate)
    {
        beginTest("Held notes C5 to C7 are within 10 cents at " + juce::String(sampleRate) + " Hz");

        for (int note = 72; note <= 96; ++note)
        {
            const double cents = measureCentsError(sampleRate, note);
            logMessage("  " + juce::MidiMessage::getMidiNoteName(note, true, true, 4)
                       + " (" + juce::String(note) + "): " + juce::String(cents, 1) + " cents");
            expect(std::abs(cents) <= 10.0,
                   "MIDI " + juce::String(note) + " is " + juce::String(cents, 1)
                       + " cents out at " + juce::String(sampleRate) + " Hz");
        }
    }
};

static ShakuhachiTuningTests shakuhachiTuningTests;
