/**
 * BrassEngine Tests
 *
 * Tests for the BrassSection synthesis engine:
 * - Output stays finite and bounded for every instrument and articulation
 * - The engine recovers and stays usable from one note to the next
 * - Loudness does not depend on the sample rate
 * - Fall-off holds its pitch while the note is held and falls away on release
 */

#include <juce_core/juce_core.h>
#include <cmath>
#include <vector>

#include "../../plugins/BrassSection/Source/BrassEngine.h"

class BrassEngineTests : public juce::UnitTest
{
public:
    BrassEngineTests()
        : juce::UnitTest("BrassEngine", "Plugins")
    {
    }

    void runTest() override
    {
        testOutputIsFiniteAtFullBrightness();
        testConsistentAcrossSampleRates();
        testFallOffHoldsPitchWhileHeld();
        testFallOffFallsOnRelease();
        testReleaseIsNotSilentUntilTheTailHasFaded();
    }

private:
    struct Measurement
    {
        bool finite = true;
        float peak = 0.0f;
        double rms = 0.0;
    };

    // Plays one held note at full velocity and measures the output
    static Measurement playNote(BrassEngine& engine, double sampleRate, float frequency, double seconds)
    {
        Measurement result;
        const int numSamples = static_cast<int>(sampleRate * seconds);
        double sumOfSquares = 0.0;

        engine.reset();

        for (int i = 0; i < numSamples; ++i)
        {
            const float sample = engine.processSample(frequency, 1.0f, true, i);

            result.finite = result.finite && std::isfinite(sample);
            result.peak = std::max(result.peak, std::abs(sample));
            sumOfSquares += static_cast<double>(sample) * sample;
        }

        result.rms = std::sqrt(sumOfSquares / numSamples);
        return result;
    }

    static constexpr double rate = 44100.0;

    static BrassEngine makeFallOffEngine()
    {
        BrassEngine engine;
        engine.prepareToPlay(rate, 512);
        engine.setInstrumentType(BrassEngine::InstrumentType::Trumpet);
        engine.setArticulation(BrassEngine::Articulation::FallOff);
        engine.setBreathNoise(0.0f);
        engine.setExpression(1.0f);
        engine.reset();
        return engine;
    }

    // Renders a note held for `held` seconds and then released for `released` seconds
    static std::vector<float> render(BrassEngine& engine, float frequency, double held, double released)
    {
        std::vector<float> out;
        const int heldSamples = static_cast<int>(held * rate);
        const int total = heldSamples + static_cast<int>(released * rate);
        for (int i = 0; i < total; ++i)
            out.push_back(engine.processSample(frequency, 1.0f, i < heldSamples, i));
        return out;
    }

    // The period between 80 and 1000 Hz, by autocorrelation over a window. A
    // periodic wave correlates as well at two or three periods as at one, so
    // take the shortest lag that comes within 10% of the best.
    static double pitchAt(const std::vector<float>& x, double seconds, double window = 0.05)
    {
        const int start = static_cast<int>(seconds * rate);
        const int n = static_cast<int>(window * rate);
        const int minLag = static_cast<int>(rate / 1000.0);
        const int maxLag = static_cast<int>(rate / 80.0);
        std::vector<double> corr(static_cast<size_t>(maxLag + 1), 0.0);
        double best = 0.0;
        for (int lag = minLag; lag <= maxLag; ++lag)
        {
            double sum = 0.0;
            for (int i = start; i < start + n; ++i)
                sum += static_cast<double>(x[static_cast<size_t>(i)]) * x[static_cast<size_t>(i + lag)];
            corr[static_cast<size_t>(lag)] = sum;
            best = std::max(best, sum);
        }
        for (int lag = minLag + 1; lag < maxLag; ++lag)
        {
            const double c = corr[static_cast<size_t>(lag)];
            if (c >= 0.9 * best && c >= corr[static_cast<size_t>(lag - 1)] && c >= corr[static_cast<size_t>(lag + 1)])
                return rate / lag;
        }
        return 0.0;
    }

    static double levelAt(const std::vector<float>& x, double seconds, double window = 0.05)
    {
        const int start = static_cast<int>(seconds * rate);
        const int n = static_cast<int>(window * rate);
        double sum = 0.0;
        for (int i = start; i < start + n; ++i)
            sum += static_cast<double>(x[static_cast<size_t>(i)]) * x[static_cast<size_t>(i)];
        return 20.0 * std::log10(std::sqrt(sum / n) + 1e-12);
    }

    void testFallOffHoldsPitchWhileHeld()
    {
        beginTest("Fall-off holds its pitch while the note is held");

        // It used to bend 10% flat half a second after the note started and stay there
        auto engine = makeFallOffEngine();
        const auto note = render(engine, 440.0f, 2.0, 0.0);

        expectWithinAbsoluteError(pitchAt(note, 0.2), 440.0, 440.0 * 0.01, "Early in the note");
        expectWithinAbsoluteError(pitchAt(note, 1.5), 440.0, 440.0 * 0.01, "Late in the held note");
    }

    void testFallOffFallsOnRelease()
    {
        beginTest("Fall-off falls away in pitch when the note is released, and is heard doing it");

        auto engine = makeFallOffEngine();
        const auto note = render(engine, 440.0f, 1.0, 1.0);
        const double heldLevel = levelAt(note, 0.8);

        const double startOfFall = pitchAt(note, 1.0);
        const double laterInFall = pitchAt(note, 1.3);
        const double semitones = 12.0 * std::log2(startOfFall / laterInFall);

        expectWithinAbsoluteError(startOfFall, 440.0, 440.0 * 0.03, "The fall starts from the note");
        expect(semitones >= 3.0, "It should fall at least 3 semitones within 0.3 s; it fell "
                                     + juce::String(semitones, 1));
        expect(levelAt(note, 1.3) > heldLevel - 24.0,
               "The fall should still be heard 0.3 s after release: "
                   + juce::String(levelAt(note, 1.3) - heldLevel, 1) + " dB from the held level");
    }

    void testReleaseIsNotSilentUntilTheTailHasFaded()
    {
        beginTest("A released note is not reported silent while its tail is still sounding");

        // The voice used to stop a released note at the first sample under
        // 0.001, which every waveform reaches at a zero crossing
        auto engine = makeFallOffEngine();
        render(engine, 440.0f, 0.5, 0.0);
        bool silentTooSoon = false;
        for (int i = 0; i < static_cast<int>(0.2 * rate); ++i)
        {
            engine.processSample(440.0f, 1.0f, false, i);
            silentTooSoon = silentTooSoon || engine.isSilent();
        }
        expect(! silentTooSoon, "Still sounding 0.2 s into the release");

        for (int i = 0; i < static_cast<int>(3.0 * rate); ++i)
            engine.processSample(440.0f, 1.0f, false, i);
        expect(engine.isSilent(), "Silent once the tail has faded");
    }

    void testOutputIsFiniteAtFullBrightness()
    {
        beginTest("Output is finite and bounded at full brightness");

        // The filter cutoff follows the note's amplitude, and Marcato pushes
        // the amplitude above 1.0 at the start of the note. That is the
        // highest cutoff the engine reaches.
        bool allFinite = true;
        bool allBounded = true;
        juce::String firstProblem;

        for (double sampleRate : { 44100.0, 48000.0, 96000.0 })
        {
            for (auto instrument : { BrassEngine::InstrumentType::Trumpet,
                                     BrassEngine::InstrumentType::Trombone,
                                     BrassEngine::InstrumentType::Saxophone })
            {
                for (auto articulation : { BrassEngine::Articulation::Sustain,
                                           BrassEngine::Articulation::Staccato,
                                           BrassEngine::Articulation::Marcato,
                                           BrassEngine::Articulation::FallOff })
                {
                    BrassEngine engine;
                    engine.prepareToPlay(sampleRate, 512);
                    engine.setInstrumentType(instrument);
                    engine.setArticulation(articulation);
                    engine.setBrightness(1.0f);
                    engine.setExpression(1.0f);

                    // Two notes in a row: a note must not leave the engine in
                    // a state that breaks the next one
                    const auto first = playNote(engine, sampleRate, 440.0f, 0.5);
                    const auto second = playNote(engine, sampleRate, 440.0f, 0.5);

                    const bool finite = first.finite && second.finite;
                    const bool bounded = first.peak < 4.0f && second.peak < 4.0f;

                    if ((! finite || ! bounded) && firstProblem.isEmpty())
                    {
                        firstProblem = "instrument " + juce::String(static_cast<int>(instrument))
                                       + ", articulation " + juce::String(static_cast<int>(articulation))
                                       + ", " + juce::String(sampleRate) + " Hz: peaks "
                                       + juce::String(first.peak) + " / " + juce::String(second.peak);
                    }

                    allFinite = allFinite && finite;
                    allBounded = allBounded && bounded;
                }
            }
        }

        expect(allFinite, "Output should always be finite. First problem: " + firstProblem);
        expect(allBounded, "Output should stay bounded. First problem: " + firstProblem);
    }

    void testConsistentAcrossSampleRates()
    {
        beginTest("Loudness does not depend on the sample rate");

        double rmsAt44k = 0.0;
        double rmsAt48k = 0.0;

        for (double sampleRate : { 44100.0, 48000.0 })
        {
            BrassEngine engine;
            engine.prepareToPlay(sampleRate, 512);
            engine.setInstrumentType(BrassEngine::InstrumentType::Trumpet);
            engine.setArticulation(BrassEngine::Articulation::Sustain);
            engine.setBrightness(1.0f);
            engine.setExpression(1.0f);
            engine.setBreathNoise(0.0f);

            const auto note = playNote(engine, sampleRate, 440.0f, 1.0);
            (sampleRate < 46000.0 ? rmsAt44k : rmsAt48k) = note.rms;
        }

        expect(rmsAt48k > 0.0, "The engine should produce output");
        expectWithinAbsoluteError(rmsAt44k / rmsAt48k, 1.0, 0.1,
                                  "The same note should be within about 1 dB at 44.1 and 48 kHz");
    }
};

static BrassEngineTests brassEngineTests;
