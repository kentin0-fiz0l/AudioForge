/**
 * GateEngine Tests
 *
 * Tests for the Gate plugin's processing engine:
 * - A gate only ever attenuates: output never exceeds input
 * - An open gate passes audio unchanged
 * - A closed gate attenuates by the range setting
 * - The gain-reduction meter reports a value between the range and 0 dB
 * - The sidechain filters shape what the detector hears, for both the main
 *   input and the external key input
 * - SC Listen plays the filtered detector signal
 */

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>
#include <cmath>
#include <utility>

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
        testFiltersShapeInternalDetector();
        testFiltersShapeKeyInput();
        testListenPlaysFilteredDetector();
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

    static void fillSine(juce::AudioBuffer<float>& buffer, int channel, float frequency,
                         float amplitude, int startSample)
    {
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            buffer.setSample(channel, i, amplitude * std::sin(juce::MathConstants<float>::twoPi * frequency
                                                               * static_cast<float>(startSample + i)
                                                               / static_cast<float>(sampleRate)));
    }

    // Runs a sine through the main input (and optionally a sine through the
    // key input) for half a second and returns the peak of each output
    // channel over the last half of the run
    static std::pair<float, float> processSines(audioforge::GateEngine& engine,
                                                float mainFrequency, float mainAmplitude,
                                                float keyLeftFrequency = 0.0f, float keyLeftAmplitude = 0.0f,
                                                float keyRightAmplitude = 0.0f)
    {
        constexpr int numBlocks = 48;
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::AudioBuffer<float> key(2, blockSize);
        float peakLeft = 0.0f, peakRight = 0.0f;

        for (int block = 0; block < numBlocks; ++block)
        {
            fillSine(buffer, 0, mainFrequency, mainAmplitude, block * blockSize);
            fillSine(buffer, 1, mainFrequency, mainAmplitude, block * blockSize);
            fillSine(key, 0, keyLeftFrequency, keyLeftAmplitude, block * blockSize);
            fillSine(key, 1, keyLeftFrequency, keyRightAmplitude, block * blockSize);

            engine.process(buffer, &key);

            if (block >= numBlocks / 2)
            {
                peakLeft = std::max(peakLeft, buffer.getMagnitude(0, 0, blockSize));
                peakRight = std::max(peakRight, buffer.getMagnitude(1, 0, blockSize));
            }
        }

        return { peakLeft, peakRight };
    }

    static void prepareSidechainGate(audioforge::GateEngine& engine, float highPassHz, float lowPassHz)
    {
        engine.prepare(sampleRate, blockSize);
        engine.setThreshold(-30.0f);
        engine.setRange(-60.0f);
        engine.setHold(0.0f);
        engine.setRelease(10.0f);
        engine.getSidechain().setHighPassFreq(highPassHz);
        engine.getSidechain().setLowPassFreq(lowPassHz);
        engine.reset();
    }

    void testFiltersShapeInternalDetector()
    {
        beginTest("Sidechain filters shape the detector for the main input");

        // A 50 Hz tone at -14 dB is well above the -30 dB threshold, but a
        // 2 kHz high-pass removes it from the detector, so the gate stays shut
        audioforge::GateEngine filtered;
        prepareSidechainGate(filtered, 2000.0f, 20000.0f);
        expectLessThan(processSines(filtered, 50.0f, 0.2f).first, 0.01f,
                       "A tone below the SC high-pass should not open the gate");

        // A 15 kHz tone is removed by a 200 Hz low-pass
        audioforge::GateEngine lowPassed;
        prepareSidechainGate(lowPassed, 20.0f, 200.0f);
        expectLessThan(processSines(lowPassed, 15000.0f, 0.2f).first, 0.01f,
                       "A tone above the SC low-pass should not open the gate");

        // Inside the band the gate opens as normal
        audioforge::GateEngine inBand;
        prepareSidechainGate(inBand, 80.0f, 8000.0f);
        expectGreaterThan(processSines(inBand, 1000.0f, 0.2f).first, 0.18f,
                          "A tone inside the SC band should open the gate");
    }

    void testFiltersShapeKeyInput()
    {
        beginTest("Sidechain filters shape the external key input");

        // The key is a loud 50 Hz tone; with a 2 kHz high-pass it must not
        // open the gate on the 1 kHz main signal
        audioforge::GateEngine filtered;
        prepareSidechainGate(filtered, 2000.0f, 20000.0f);
        filtered.getSidechain().setEnabled(true);
        expectLessThan(processSines(filtered, 1000.0f, 0.2f, 50.0f, 0.5f, 0.5f).first, 0.01f,
                       "A key tone below the SC high-pass should not open the gate");

        // With the default band the same key opens the gate
        audioforge::GateEngine unfiltered;
        prepareSidechainGate(unfiltered, 20.0f, 20000.0f);
        unfiltered.getSidechain().setEnabled(true);
        expectGreaterThan(processSines(unfiltered, 1000.0f, 0.2f, 50.0f, 0.5f, 0.5f).first, 0.18f,
                          "A key tone inside the SC band should open the gate");
    }

    void testListenPlaysFilteredDetector()
    {
        beginTest("SC Listen plays the filtered detector signal");

        // Key: 1 kHz on the left, silence on the right. Main input: silence.
        // Listening should play the key (left only), not the gated main input.
        audioforge::GateEngine listen;
        prepareSidechainGate(listen, 80.0f, 8000.0f);
        listen.getSidechain().setEnabled(true);
        listen.getSidechain().setListenMode(true);
        auto [left, right] = processSines(listen, 1000.0f, 0.0f, 1000.0f, 0.5f, 0.0f);
        expectWithinAbsoluteError(left, 0.5f, 0.05f, "Listen should play the key signal");
        expectLessThan(right, 0.001f, "Each channel of the key is filtered on its own");

        // A key tone below the high-pass is filtered out of what you hear
        audioforge::GateEngine listenFiltered;
        prepareSidechainGate(listenFiltered, 2000.0f, 20000.0f);
        listenFiltered.getSidechain().setEnabled(true);
        listenFiltered.getSidechain().setListenMode(true);
        expectLessThan(processSines(listenFiltered, 1000.0f, 0.0f, 50.0f, 0.5f, 0.5f).first, 0.01f,
                       "Listen should play the key after the SC filters");

        // Without an external key, listen plays the filtered main input
        audioforge::GateEngine listenInternal;
        prepareSidechainGate(listenInternal, 2000.0f, 20000.0f);
        listenInternal.getSidechain().setListenMode(true);
        expectLessThan(processSines(listenInternal, 50.0f, 0.5f).first, 0.01f,
                       "Listen on the main input should play it after the SC filters");
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
