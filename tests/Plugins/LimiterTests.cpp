/**
 * Limiter Tests
 *
 * A limiter has one job: nothing comes out above the ceiling. That has to
 * hold whatever the threshold, with Auto Makeup on or off. And dropping it
 * on a track at its default settings should not change the level of a
 * signal that is already under the ceiling.
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
