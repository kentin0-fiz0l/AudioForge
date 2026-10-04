/**
 * FreezeFX Tests
 *
 * Freeze should hold the sound that was playing when it was switched on,
 * and leave the audio alone when it is off.
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <cmath>
#include <vector>

#include "../../plugins/FreezeFX/Source/PluginProcessor.h"

class FreezeFXTests : public juce::UnitTest
{
public:
    FreezeFXTests()
        : juce::UnitTest("FreezeFX", "Plugins")
    {
    }

    void runTest() override
    {
        testOffLeavesAudioUntouched();
        testFreezeSustainsTheSound();
    }

private:
    static constexpr double sampleRate = 48000.0;
    static constexpr int blockSize = 512;
    static constexpr float toneHz = 1000.0f;
    static constexpr float toneLevel = 0.5f;

    // Runs the processor for the given time, feeding a 1 kHz tone or silence,
    // and returns the left channel of what came out
    std::vector<float> process(FreezeFXProcessor& processor, double seconds, bool withTone)
    {
        std::vector<float> output;
        juce::MidiBuffer midi;

        const int numBlocks = static_cast<int>(seconds * sampleRate / blockSize);

        for (int block = 0; block < numBlocks; ++block)
        {
            juce::AudioBuffer<float> buffer(2, blockSize);
            buffer.clear();

            if (withTone)
            {
                for (int i = 0; i < blockSize; ++i)
                {
                    const float value = toneLevel * std::sin(juce::MathConstants<float>::twoPi * toneHz
                                                             * static_cast<float>(samplesFed + i) / static_cast<float>(sampleRate));
                    buffer.setSample(0, i, value);
                    buffer.setSample(1, i, value);
                }
            }

            samplesFed += blockSize;
            processor.processBlock(buffer, midi);
            output.insert(output.end(), buffer.getReadPointer(0), buffer.getReadPointer(0) + blockSize);
        }

        return output;
    }

    // As a host does: tell the processor its rate and block size, then prepare
    void prepare(FreezeFXProcessor& processor)
    {
        processor.setRateAndBufferSizeDetails(sampleRate, blockSize);
        processor.prepareToPlay(sampleRate, blockSize);
        samplesFed = 0;
    }

    static void setFreeze(FreezeFXProcessor& processor, bool on)
    {
        processor.getAPVTS().getParameter(FreezeFXProcessor::PARAM_FREEZE)->setValueNotifyingHost(on ? 1.0f : 0.0f);
    }

    static float rms(const std::vector<float>& samples, size_t from)
    {
        double sum = 0.0;
        for (size_t i = from; i < samples.size(); ++i)
            sum += samples[i] * samples[i];

        return static_cast<float>(std::sqrt(sum / static_cast<double>(samples.size() - from)));
    }

    void testOffLeavesAudioUntouched()
    {
        beginTest("With Freeze off, the tone passes through at its own level");

        FreezeFXProcessor processor;
        prepare(processor);

        const auto output = process(processor, 0.5, true);

        expectWithinAbsoluteError(rms(output, 0), toneLevel / std::sqrt(2.0f), 0.001f);
    }

    void testFreezeSustainsTheSound()
    {
        beginTest("Freeze keeps the tone sounding after the input stops");

        FreezeFXProcessor processor;
        prepare(processor);

        process(processor, 0.5, true);
        setFreeze(processor, true);
        process(processor, 0.1, true);

        // The input goes silent: anything heard from here on is the frozen sound
        const auto frozen = process(processor, 1.0, false);
        const auto lastHalf = frozen.size() / 2;

        bool finite = true;
        float peak = 0.0f;
        int crossings = 0;

        for (size_t i = lastHalf; i < frozen.size(); ++i)
        {
            finite = finite && std::isfinite(frozen[i]);
            peak = juce::jmax(peak, std::abs(frozen[i]));

            if (i > lastHalf && (frozen[i] < 0.0f) != (frozen[i - 1] < 0.0f))
                ++crossings;
        }

        const float level = rms(frozen, lastHalf);
        const double crossingsPerSecond = crossings * sampleRate / static_cast<double>(frozen.size() - lastHalf);

        expect(finite, "Non-finite output while frozen");
        expect(level > 0.05f, "Frozen sound is too quiet to hear: RMS " + juce::String(level, 5));
        expect(peak < 1.0f, "Frozen sound peaks at " + juce::String(peak, 3));

        // A 1 kHz tone crosses zero 2000 times a second. The frozen sound has
        // its phases scrambled, so only ask for the same neighbourhood.
        expect(crossingsPerSecond > 1500.0 && crossingsPerSecond < 2500.0,
               "Frozen sound is not centred on the 1 kHz tone: "
                   + juce::String(static_cast<int>(crossingsPerSecond)) + " zero crossings a second");

        // Switching Freeze off returns to the (silent) input
        setFreeze(processor, false);
        const auto released = process(processor, 0.1, false);
        expectWithinAbsoluteError(rms(released, released.size() / 2), 0.0f, 0.0001f);
    }

    juce::int64 samplesFed = 0;
};

static FreezeFXTests freezeFXTests;
