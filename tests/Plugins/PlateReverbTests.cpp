/**
 * PlateReverb Tests
 *
 * Mix runs from the dry signal alone to the reverb alone. With Mix at zero
 * the signal should come through exactly as it went in, and at the default
 * settings the plugin should not add several dB of level. PreDelay holds
 * the reverb back by its time, and leaves the dry signal where it is.
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <cmath>

#include "../../plugins/PlateReverb/Source/PluginProcessor.h"

class PlateReverbTests : public juce::UnitTest
{
public:
    PlateReverbTests()
        : juce::UnitTest("PlateReverb", "Plugins")
    {
    }

    void runTest() override
    {
        testMixAtZeroIsTheDrySignal();
        testDefaultSettingsKeepTheLevel();
        testFullyWetIsNearTheInputLevel();
        testPreDelayHoldsBackTheReverb();
        testPreDelayLeavesTheDrySignalAlone();
    }

private:
    static constexpr double sampleRate = 48000.0;
    static constexpr int blockSize = 512;

    struct Result
    {
        float inputPeak = 0.0f;
        float outputPeak = 0.0f;
        float worstDifference = 0.0f; // Between output and input, sample for sample
    };

    // One second of noise at 0.5 through the plugin. The first half second is
    // left out of the result, while the plugin's level smoothing settles.
    Result run(ReverbProcessor& processor)
    {
        processor.setRateAndBufferSizeDetails(sampleRate, blockSize);
        processor.prepareToPlay(sampleRate, blockSize);

        juce::Random random(7);
        juce::AudioBuffer<float> input(2, blockSize), buffer(2, blockSize);
        juce::MidiBuffer midi;
        Result result;

        const int numBlocks = static_cast<int>(sampleRate / blockSize);
        for (int block = 0; block < numBlocks; ++block)
        {
            for (int channel = 0; channel < 2; ++channel)
                for (int i = 0; i < blockSize; ++i)
                    input.setSample(channel, i, 0.5f * (2.0f * random.nextFloat() - 1.0f));

            buffer.makeCopyOf(input);
            processor.processBlock(buffer, midi);

            if (block < numBlocks / 2)
                continue;

            result.inputPeak = juce::jmax(result.inputPeak, input.getMagnitude(0, blockSize));
            result.outputPeak = juce::jmax(result.outputPeak, buffer.getMagnitude(0, blockSize));

            for (int channel = 0; channel < 2; ++channel)
                for (int i = 0; i < blockSize; ++i)
                    result.worstDifference = juce::jmax(result.worstDifference,
                                                        std::abs(buffer.getSample(channel, i) - input.getSample(channel, i)));
        }

        return result;
    }

    static void setParameter(ReverbProcessor& processor, const juce::String& id, float value)
    {
        auto* parameter = processor.getValueTreeState().getParameter(id);
        parameter->setValueNotifyingHost(parameter->convertTo0to1(value));
    }

    // Sends a single impulse through the plugin, after half a second of
    // silence for its smoothing to settle, and returns the sample at which
    // the output first goes above a small threshold
    int firstSoundAfterImpulse(ReverbProcessor& processor)
    {
        processor.setRateAndBufferSizeDetails(sampleRate, blockSize);
        processor.prepareToPlay(sampleRate, blockSize);

        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midi;

        const int silentBlocks = static_cast<int>(sampleRate / blockSize / 2);
        for (int block = 0; block < silentBlocks; ++block)
        {
            buffer.clear();
            processor.processBlock(buffer, midi);
        }

        const int numBlocks = static_cast<int>(sampleRate / blockSize / 4);
        for (int block = 0; block < numBlocks; ++block)
        {
            buffer.clear();
            if (block == 0)
                for (int channel = 0; channel < 2; ++channel)
                    buffer.setSample(channel, 0, 1.0f);

            processor.processBlock(buffer, midi);

            for (int i = 0; i < blockSize; ++i)
                for (int channel = 0; channel < 2; ++channel)
                    if (std::abs(buffer.getSample(channel, i)) > 1.0e-3f)
                        return block * blockSize + i;
        }

        return -1;
    }

    void testPreDelayHoldsBackTheReverb()
    {
        beginTest("PreDelay holds the reverb back by its time");

        ReverbProcessor withoutPreDelay, withPreDelay;
        for (auto* processor : { &withoutPreDelay, &withPreDelay })
            setParameter(*processor, "mix", 1.0f);
        setParameter(withPreDelay, "predelay", 50.0f);

        const int onsetWithout = firstSoundAfterImpulse(withoutPreDelay);
        const int onsetWith = firstSoundAfterImpulse(withPreDelay);
        const int expectedShift = static_cast<int>(sampleRate * 0.05);

        expect(onsetWithout >= 0 && onsetWith >= 0, "No reverb came out of the impulse");
        expectWithinAbsoluteError(onsetWith - onsetWithout, expectedShift, 2);
    }

    void testPreDelayLeavesTheDrySignalAlone()
    {
        beginTest("PreDelay does not delay the dry signal");

        ReverbProcessor processor;
        setParameter(processor, "mix", 0.0f);
        setParameter(processor, "predelay", 100.0f);

        const auto result = run(processor);

        expect(result.worstDifference < 0.001f,
               "Output differs from the input by up to " + juce::String(result.worstDifference, 4));
    }

    void testMixAtZeroIsTheDrySignal()
    {
        beginTest("With Mix at zero the signal passes through unchanged");

        ReverbProcessor processor;
        processor.getValueTreeState().getParameter("mix")->setValueNotifyingHost(0.0f);

        const auto result = run(processor);

        expect(result.worstDifference < 0.001f,
               "Output differs from the input by up to " + juce::String(result.worstDifference, 4)
                   + " (output peak " + juce::String(result.outputPeak, 3)
                   + " for an input peak of " + juce::String(result.inputPeak, 3) + ")");
    }

    void testFullyWetIsNearTheInputLevel()
    {
        beginTest("With Mix at one the reverb alone is within 6 dB of the input");

        ReverbProcessor processor;
        processor.getValueTreeState().getParameter("mix")->setValueNotifyingHost(1.0f);

        const auto result = run(processor);

        expectWithinAbsoluteError(juce::Decibels::gainToDecibels(result.outputPeak),
                                  juce::Decibels::gainToDecibels(result.inputPeak), 6.0f);
    }

    void testDefaultSettingsKeepTheLevel()
    {
        beginTest("At its default settings the output peak is within 3 dB of the input");

        ReverbProcessor processor;
        const auto result = run(processor);

        expectWithinAbsoluteError(juce::Decibels::gainToDecibels(result.outputPeak),
                                  juce::Decibels::gainToDecibels(result.inputPeak), 3.0f);
    }
};

static PlateReverbTests plateReverbTests;
