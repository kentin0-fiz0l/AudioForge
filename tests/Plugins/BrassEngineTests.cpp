/**
 * BrassEngine Tests
 *
 * Tests for the BrassSection synthesis engine:
 * - Output stays finite and bounded for every instrument and articulation
 * - The engine recovers and stays usable from one note to the next
 * - Loudness does not depend on the sample rate
 */

#include <juce_core/juce_core.h>
#include <cmath>

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
