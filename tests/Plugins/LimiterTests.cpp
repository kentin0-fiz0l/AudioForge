/**
 * Limiter Tests
 *
 * A limiter has one job: nothing comes out above the ceiling. That has to
 * hold whatever the threshold, with Auto Makeup on or off. And dropping it
 * on a track at its default settings should not change the level of a
 * signal that is already under the ceiling.
 *
 * Lookahead delays the signal so the gain can come down before a peak
 * arrives, instead of at the instant it does.
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <cmath>

#include "../../plugins/Limiter/Source/LimiterEngine.h"
#include "../../plugins/Limiter/Source/PluginProcessor.h"

class LimiterTests : public juce::UnitTest
{
public:
    LimiterTests()
        : juce::UnitTest("Limiter", "Plugins")
    {
    }

    void runTest() override
    {
        testNothingPassesTheCeiling();
        testMakeupRaisesQuietSignals();
        testDefaultSettingsLeaveLevelAlone();
        testLookaheadDelaysTheSignal();
        testGainComesDownBeforeThePeak();
        testNoLookaheadActsAtThePeak();
        testCeilingHoldsWithLookahead();
        testCeilingHoldsThroughALongDecay();
        testPluginReportsItsLatency();
    }

private:
    static constexpr double sampleRate = 48000.0;
    static constexpr int blockSize = 512;

    static void fillWithNoise(juce::AudioBuffer<float>& buffer, juce::Random& random, float level)
    {
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            for (int i = 0; i < buffer.getNumSamples(); ++i)
                buffer.setSample(channel, i, level * (2.0f * random.nextFloat() - 1.0f));
    }

    // Half a second of noise through the engine, set up the way the plugin
    // sets it up before every block. Returns the output peak.
    static float runEngine(float thresholdDb, float ceilingDb, bool autoMakeup, float inputLevel)
    {
        audioforge::LimiterEngine engine;
        engine.prepare(sampleRate, blockSize);

        juce::Random random(7);
        juce::AudioBuffer<float> buffer(2, blockSize);
        float peak = 0.0f;

        for (int block = 0; block < static_cast<int>(0.5 * sampleRate / blockSize); ++block)
        {
            engine.setCeiling(ceilingDb);
            engine.setThreshold(thresholdDb);
            engine.setRelease(100.0f);
            engine.setLookahead(5.0f);
            engine.setAutoMakeupEnabled(autoMakeup);
            engine.setOutputTrim(0.0f);

            fillWithNoise(buffer, random, inputLevel);
            engine.process(buffer);
            peak = juce::jmax(peak, buffer.getMagnitude(0, blockSize));
        }

        return peak;
    }

    // A steady level with one loud sample in it, through the engine in
    // blocks. The output is returned whole, left channel.
    static std::vector<float> runSpike(float lookaheadMs, int spikeAt, float steady, float spike, int total = 4096)
    {
        audioforge::LimiterEngine engine;
        engine.prepare(sampleRate, blockSize);

        std::vector<float> output;
        juce::AudioBuffer<float> buffer(2, blockSize);

        for (int start = 0; start < total; start += blockSize)
        {
            // Set up before every block, the way the plugin does it
            engine.setCeiling(0.0f);
            engine.setThreshold(0.0f);
            engine.setRelease(100.0f);
            engine.setLookahead(lookaheadMs);
            engine.setAutoMakeupEnabled(false);
            engine.setOutputTrim(0.0f);

            for (int channel = 0; channel < 2; ++channel)
                for (int i = 0; i < blockSize; ++i)
                    buffer.setSample(channel, i, start + i == spikeAt ? spike : steady);

            engine.process(buffer);

            for (int i = 0; i < blockSize; ++i)
                output.push_back(buffer.getSample(0, i));
        }

        return output;
    }

    static constexpr float lookaheadMs = 5.0f;
    static constexpr int lookaheadSamples = 240; // 5 ms at 48 kHz

    void testLookaheadDelaysTheSignal()
    {
        beginTest("Lookahead delays the signal by the lookahead time");

        // A quiet click, which the limiter has no reason to touch, placed
        // so that its delayed copy lands in the next block
        const int clickAt = 400;
        const auto output = runSpike(lookaheadMs, clickAt, 0.0f, 0.5f);

        expectEquals(output[static_cast<size_t>(clickAt)], 0.0f, "Nothing comes out at the moment the click goes in");
        expectWithinAbsoluteError(output[static_cast<size_t>(clickAt + lookaheadSamples)], 0.5f, 1.0e-6f,
                                  "The click comes out one lookahead later");

        audioforge::LimiterEngine engine;
        engine.prepare(sampleRate, blockSize);
        engine.setLookahead(lookaheadMs);
        expectEquals(engine.getLookaheadSamples(), lookaheadSamples);
    }

    void testGainComesDownBeforeThePeak()
    {
        beginTest("With lookahead the gain is already coming down before a peak arrives");

        // A steady 0.5 with one sample at 2.0, which needs the gain halved
        const int spikeAt = 2000;
        const auto output = runSpike(lookaheadMs, spikeAt, 0.5f, 2.0f);
        const auto at = [&] (int offsetFromSpike) { return output[static_cast<size_t>(spikeAt + lookaheadSamples + offsetFromSpike)]; };

        expectWithinAbsoluteError(at(0), 1.0f, 1.0e-4f, "The peak itself comes out at the ceiling");
        expectWithinAbsoluteError(at(-lookaheadSamples - 10), 0.5f, 1.0e-6f, "Well before the peak the signal is untouched");

        // Half way through the lookahead the gain is on its way down
        expect(at(-lookaheadSamples / 2) < 0.45f && at(-lookaheadSamples / 2) > 0.26f,
               "Half a lookahead before the peak the steady 0.5 came out at " + juce::String(at(-lookaheadSamples / 2), 3));

        // And it gets there gradually, not in one step
        float largestStep = 0.0f;
        for (int offset = -lookaheadSamples; offset < 0; ++offset)
            largestStep = juce::jmax(largestStep, std::abs(at(offset) - at(offset - 1)));

        expect(largestStep < 0.002f, "The gain moved in a step of " + juce::String(largestStep, 4));
    }

    void testNoLookaheadActsAtThePeak()
    {
        beginTest("With lookahead at zero there is no delay and the gain drops at the peak");

        const int spikeAt = 2000;
        const auto output = runSpike(0.0f, spikeAt, 0.5f, 2.0f);

        expectWithinAbsoluteError(output[static_cast<size_t>(spikeAt)], 1.0f, 1.0e-4f, "The peak comes out at once, at the ceiling");
        expectWithinAbsoluteError(output[static_cast<size_t>(spikeAt - 1)], 0.5f, 1.0e-6f, "The sample before it is untouched");
    }

    void testCeilingHoldsWithLookahead()
    {
        beginTest("Nothing passes the ceiling at any lookahead");

        for (float lookahead : { 0.0f, 0.1f, 1.0f, 5.0f, 10.0f })
        {
            audioforge::LimiterEngine engine;
            engine.prepare(sampleRate, blockSize);

            juce::Random random(11);
            juce::AudioBuffer<float> buffer(2, blockSize);
            float peak = 0.0f;

            for (int block = 0; block < static_cast<int>(0.5 * sampleRate / blockSize); ++block)
            {
                engine.setCeiling(-1.0f);
                engine.setThreshold(-12.0f);
                engine.setRelease(50.0f);
                engine.setLookahead(lookahead);
                engine.setAutoMakeupEnabled(true);
                engine.setOutputTrim(0.0f);

                // Noise with bursts well over the ceiling
                fillWithNoise(buffer, random, block % 3 == 0 ? 2.0f : 0.3f);
                engine.process(buffer);
                peak = juce::jmax(peak, buffer.getMagnitude(0, blockSize));
            }

            expect(peak <= juce::Decibels::decibelsToGain(-1.0f) * 1.0001f,
                   "Peak of " + juce::String(juce::Decibels::gainToDecibels(peak), 2)
                       + " dBFS with a lookahead of " + juce::String(lookahead, 1) + " ms");
        }
    }

    void testCeilingHoldsThroughALongDecay()
    {
        beginTest("Nothing passes the ceiling when a loud sound holds and then drops away, at the longest lookahead");

        // A level that only ever falls asks for a little more gain on every
        // sample, so each one has to be remembered until its delayed copy
        // has gone by: a full lookahead's worth. Here it holds at four
        // times the ceiling for most of the lookahead and then drops away
        // in under 2 ms. The gain has to stay down for the loud samples
        // still inside the delay.
        audioforge::LimiterEngine engine;
        engine.prepare(sampleRate, blockSize);

        const auto levelAt = [] (int n)
        {
            if (n <= 400)
                return 4.0f - static_cast<float>(n) * 1.0e-6f;
            if (n <= 481)
                return 3.9996f - static_cast<float>(n - 400) * (2.99f / 81.0f);
            return 0.5f;
        };

        juce::AudioBuffer<float> buffer(2, blockSize);
        float peak = 0.0f;
        int n = 0;

        for (int block = 0; block < 8; ++block)
        {
            engine.setCeiling(0.0f);
            engine.setThreshold(0.0f);
            engine.setRelease(10.0f);
            engine.setLookahead(10.0f);
            engine.setAutoMakeupEnabled(false);
            engine.setOutputTrim(0.0f);

            for (int i = 0; i < blockSize; ++i, ++n)
            {
                buffer.setSample(0, i, levelAt(n));
                buffer.setSample(1, i, levelAt(n));
            }

            engine.process(buffer);
            peak = juce::jmax(peak, buffer.getMagnitude(0, blockSize));
        }

        expect(peak <= 1.0001f, "Peak of " + juce::String(peak, 4));
    }

    void testPluginReportsItsLatency()
    {
        beginTest("The plugin tells the host how much delay the lookahead adds");

        LimiterAudioProcessor processor;
        processor.setRateAndBufferSizeDetails(sampleRate, blockSize);
        processor.prepareToPlay(sampleRate, blockSize);

        expectEquals(processor.getLatencySamples(), lookaheadSamples, "The default lookahead is 5 ms");

        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midi;

        // The plugin has a getParameters() of its own, for its editor
        for (auto* param : static_cast<juce::AudioProcessor&>(processor).getParameters())
            if (param->getName(64) == "Lookahead")
                param->setValueNotifyingHost(0.0f);

        buffer.clear();
        processor.processBlock(buffer, midi);

        expectEquals(processor.getLatencySamples(), 0);
    }

    void testNothingPassesTheCeiling()
    {
        beginTest("Output never exceeds the ceiling, with Auto Makeup on or off");

        bool allUnderCeiling = true;
        juce::String firstOver;

        for (bool autoMakeup : { false, true })
        {
            for (float thresholdDb : { -24.0f, -12.0f, -6.0f, 0.0f })
            {
                for (float ceilingDb : { -12.0f, -3.0f, -0.3f, 0.0f })
                {
                    for (float inputLevel : { 0.25f, 0.5f, 1.0f, 2.0f })
                    {
                        const float peak = runEngine(thresholdDb, ceilingDb, autoMakeup, inputLevel);
                        const float ceiling = juce::Decibels::decibelsToGain(ceilingDb);

                        if (peak > ceiling * 1.001f && firstOver.isEmpty())
                            firstOver = "peak " + juce::String(peak, 3) + " against a ceiling of "
                                        + juce::String(ceiling, 3) + ": threshold " + juce::String(thresholdDb, 1)
                                        + " dB, makeup " + (autoMakeup ? "on" : "off")
                                        + ", input " + juce::String(inputLevel, 2);

                        allUnderCeiling = allUnderCeiling && peak <= ceiling * 1.001f;
                    }
                }
            }
        }

        expect(allUnderCeiling, "Over the ceiling: " + firstOver);
    }

    void testMakeupRaisesQuietSignals()
    {
        beginTest("Auto Makeup raises a quiet signal by ceiling over threshold");

        // Threshold -12 dB and ceiling -1 dB: 11 dB of makeup
        const float withMakeup = runEngine(-12.0f, -1.0f, true, 0.1f);
        const float without = runEngine(-12.0f, -1.0f, false, 0.1f);

        expectWithinAbsoluteError(juce::Decibels::gainToDecibels(without), juce::Decibels::gainToDecibels(0.1f), 0.2f);
        expectWithinAbsoluteError(juce::Decibels::gainToDecibels(withMakeup / without), 11.0f, 0.2f);
    }

    void testDefaultSettingsLeaveLevelAlone()
    {
        beginTest("At its default settings the plugin does not change a signal under the ceiling");

        LimiterAudioProcessor processor;
        processor.setRateAndBufferSizeDetails(sampleRate, blockSize);
        processor.prepareToPlay(sampleRate, blockSize);

        juce::Random random(7);
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midi;
        float peak = 0.0f;

        for (int block = 0; block < static_cast<int>(0.5 * sampleRate / blockSize); ++block)
        {
            fillWithNoise(buffer, random, 0.5f);
            processor.processBlock(buffer, midi);
            peak = juce::jmax(peak, buffer.getMagnitude(0, blockSize));
        }

        expectWithinAbsoluteError(juce::Decibels::gainToDecibels(peak), juce::Decibels::gainToDecibels(0.5f), 0.2f);
    }
};

static LimiterTests limiterTests;
