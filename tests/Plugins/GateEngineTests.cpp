/**
 * GateEngine Tests
 *
 * Tests for the Gate plugin's processing engine:
 * - A gate only ever attenuates: output never exceeds input
 * - An open gate passes audio unchanged
 * - A closed gate attenuates by the range setting
 * - The gain-reduction meter reports a value between the range and 0 dB
 */

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>
#include <cmath>

#include "../../plugins/Gate/Source/GateEngine.h"

class GateEngineTests : public juce::UnitTest
{
public:
    GateEngineTests()
        : juce::UnitTest("GateEngine", "Plugins")
    {
    }

    void runTest() override
    {
        testNeverAmplifies();
        testOpenGatePassesAudio();
        testClosedGateAttenuatesByRange();
        testGainReductionMeter();
    }

private:
    static constexpr double sampleRate = 48000.0;
    static constexpr int blockSize = 512;

    using Mode = audioforge::GateEngine::ProcessingMode;

    // Feeds noise of the given amplitude through the engine for a number of
    // blocks and returns the output peak over the last half of the run
    static float processNoise(audioforge::GateEngine& engine, float amplitude, int numBlocks,
                              bool* allFinite = nullptr)
    {
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::Random random(99);
        float peak = 0.0f;

        for (int block = 0; block < numBlocks; ++block)
        {
            for (int channel = 0; channel < 2; ++channel)
                for (int i = 0; i < blockSize; ++i)
                    buffer.setSample(channel, i, amplitude * (2.0f * random.nextFloat() - 1.0f));

            engine.process(buffer);

            for (int channel = 0; channel < 2; ++channel)
            {
                for (int i = 0; i < blockSize; ++i)
                {
                    const float sample = buffer.getSample(channel, i);

                    if (allFinite != nullptr && ! std::isfinite(sample))
                        *allFinite = false;

                    if (block >= numBlocks / 2)
                        peak = std::max(peak, std::abs(sample));
                }
            }
        }

        return peak;
    }

    void testNeverAmplifies()
    {
        beginTest("Output never exceeds input");

        // Regression test: the engine used to overwrite its gain state with a
        // metering value at the end of every block, which sent the gain into
        // the thousands at the default settings.
        const float amplitude = 0.5f;
        bool allFinite = true;
        float worstPeak = 0.0f;
        juce::String worstSettings;

        for (auto mode : { Mode::Gate, Mode::Expander })
        {
            for (float thresholdDb : { -60.0f, -20.0f, -6.0f, 0.0f })
            {
                for (float rangeDb : { -96.0f, -60.0f, -20.0f, 0.0f })
                {
                    audioforge::GateEngine engine;
                    engine.prepare(sampleRate, blockSize);
                    engine.setMode(mode);
                    engine.setThreshold(thresholdDb);
                    engine.setRange(rangeDb);
                    engine.reset();

                    const float peak = processNoise(engine, amplitude, 94, &allFinite);

                    if (peak > worstPeak)
                    {
                        worstPeak = peak;
                        worstSettings = "mode " + juce::String(static_cast<int>(mode))
                                        + ", threshold " + juce::String(thresholdDb)
                                        + " dB, range " + juce::String(rangeDb) + " dB";
                    }
                }
            }
        }

        expect(allFinite, "Output should always be finite");
        expect(worstPeak <= amplitude * 1.001f,
               "A gate should never be louder than its input. Worst: peak "
                   + juce::String(worstPeak) + " with " + worstSettings);
    }

    void testOpenGatePassesAudio()
    {
        beginTest("Open gate passes audio unchanged");

        audioforge::GateEngine engine;
        engine.prepare(sampleRate, blockSize);
        engine.setThreshold(-60.0f);   // Far below the signal: the gate stays open
        engine.setRange(-60.0f);
        engine.setAttack(0.1f);
        engine.reset();

        const float peak = processNoise(engine, 0.5f, 94);

        expectWithinAbsoluteError(peak, 0.5f, 0.02f, "Signal above the threshold should pass at full level");
    }

    void testClosedGateAttenuatesByRange()
    {
        beginTest("Closed gate attenuates by the range");

        audioforge::GateEngine engine;
        engine.prepare(sampleRate, blockSize);
        engine.setThreshold(0.0f);     // Above the signal: the gate stays closed
        engine.setRange(-20.0f);
        engine.setHold(0.0f);
        engine.setRelease(10.0f);
        engine.reset();

        const float peak = processNoise(engine, 0.1f, 94);

        // -20 dB of 0.1 is 0.01
        expectWithinAbsoluteError(peak, 0.01f, 0.003f, "Signal below the threshold should be reduced by the range");
    }

    void testGainReductionMeter()
    {
        beginTest("Gain reduction meter stays between the range and 0 dB");

        audioforge::GateEngine open;
        open.prepare(sampleRate, blockSize);
        open.setThreshold(-60.0f);
        open.setRange(-40.0f);
        open.setAttack(0.1f);
        open.reset();
        processNoise(open, 0.5f, 94);

        expectWithinAbsoluteError(open.getGainReductionDb(), 0.0f, 0.5f, "An open gate shows no reduction");

        audioforge::GateEngine closed;
        closed.prepare(sampleRate, blockSize);
        closed.setThreshold(0.0f);
        closed.setRange(-40.0f);
        closed.setHold(0.0f);
        closed.setRelease(10.0f);
        closed.reset();
        processNoise(closed, 0.1f, 94);

        expectWithinAbsoluteError(closed.getGainReductionDb(), -40.0f, 1.0f, "A closed gate shows the full range");
    }
};

static GateEngineTests gateEngineTests;
