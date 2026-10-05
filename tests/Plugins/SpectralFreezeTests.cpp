/**
 * SpectralFreeze Tests
 *
 * With Freeze off the sound should come through as it went in, only later.
 * With Freeze on it should hold what was playing, at the level and pitch
 * it had, on each channel separately.
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <cmath>
#include <vector>

#include "../../plugins/SpectralFreeze/Source/PluginProcessor.h"

class SpectralFreezeTests : public juce::UnitTest
{
public:
    SpectralFreezeTests()
        : juce::UnitTest("SpectralFreeze", "Plugins")
    {
    }

    void runTest() override
    {
        testPassesSoundThrough();
        testFrozenSoundHoldsItsLevelAndPitch();
        testChannelsAreFrozenSeparately();
        testUnfreezingReturnsToTheInput();
    }

private:
    static constexpr double sampleRate = 48000.0;
    static constexpr int blockSize = 512;
    static constexpr float toneHz = 1000.0f;
    static constexpr float toneLevel = 0.5f;

    void prepare(SpectralFreezeProcessor& processor)
    {
        processor.setRateAndBufferSizeDetails(sampleRate, blockSize);
        processor.prepareToPlay(sampleRate, blockSize);
        samplesFed = 0;
    }

    static void setFreeze(SpectralFreezeProcessor& processor, bool on)
    {
        for (auto* param : processor.getParameters())
            if (param->getName(64) == "Freeze")
                param->setValueNotifyingHost(on ? 1.0f : 0.0f);
    }

    // Runs the processor for the given time, feeding a 1 kHz tone or
    // silence, and returns one channel of what came out
    std::vector<float> process(SpectralFreezeProcessor& processor, double seconds, bool withTone,
                               int outputChannel = 0, bool leftOnly = false)
    {
        std::vector<float> output;
        juce::MidiBuffer midi;
        juce::AudioBuffer<float> buffer(2, blockSize);

        for (int block = 0; block < static_cast<int>(seconds * sampleRate / blockSize); ++block)
        {
            buffer.clear();

            if (withTone)
            {
                for (int i = 0; i < blockSize; ++i)
                {
                    const float value = toneLevel * std::sin(juce::MathConstants<float>::twoPi * toneHz
                                                             * static_cast<float>(samplesFed + i) / static_cast<float>(sampleRate));
                    buffer.setSample(0, i, value);
                    buffer.setSample(1, i, leftOnly ? 0.0f : value);
                }
            }

            samplesFed += blockSize;
            processor.processBlock(buffer, midi);
            output.insert(output.end(), buffer.getReadPointer(outputChannel), buffer.getReadPointer(outputChannel) + blockSize);
        }

        return output;
    }

    struct Measured
    {
        float levelDb;       // Against the level of the tone
        double frequency;    // From zero crossings
    };

    // The second half of what came out
    static Measured measure(const std::vector<float>& samples)
    {
        const auto from = samples.size() / 2;

        double sum = 0.0;
        int crossings = 0;
        for (size_t i = from; i < samples.size(); ++i)
        {
            sum += samples[i] * samples[i];
            if (i > from && (samples[i] < 0.0f) != (samples[i - 1] < 0.0f))
                ++crossings;
        }

        const auto count = static_cast<double>(samples.size() - from);
        const float rms = static_cast<float>(std::sqrt(sum / count));

        return { juce::Decibels::gainToDecibels(rms / (toneLevel / std::sqrt(2.0f))),
                 crossings * sampleRate / count / 2.0 };
    }

    void expectTheTone(const Measured& measured, const juce::String& what)
    {
        logMessage(what + ": " + juce::String(measured.levelDb, 1) + " dB, " + juce::String(measured.frequency, 1) + " Hz");

        expect(std::abs(measured.levelDb) < 2.0f,
               what + " is " + juce::String(measured.levelDb, 1) + " dB from the level of the tone");
        expect(measured.frequency > 995.0 && measured.frequency < 1005.0,
               what + " is at " + juce::String(measured.frequency, 1) + " Hz");
    }

    void testPassesSoundThrough()
    {
        beginTest("With Freeze off, a tone comes through at its own level and pitch");

        SpectralFreezeProcessor processor;
        prepare(processor);

        expectTheTone(measure(process(processor, 1.0, true)), "The tone passed through");
    }

    void testFrozenSoundHoldsItsLevelAndPitch()
    {
        beginTest("Freeze holds the tone at the level and pitch it had, after the input stops");

        SpectralFreezeProcessor processor;
        prepare(processor);

        process(processor, 0.5, true);
        setFreeze(processor, true);
        process(processor, 0.1, true);

        expectTheTone(measure(process(processor, 1.0, false)), "The frozen tone");
    }

    void testChannelsAreFrozenSeparately()
    {
        beginTest("Each channel freezes its own sound");

        // The tone is in the left channel only, so the right has nothing to hold
        for (int channel : { 0, 1 })
        {
            SpectralFreezeProcessor processor;
            prepare(processor);

            process(processor, 0.5, true, 0, true);
            setFreeze(processor, true);
            process(processor, 0.1, true, 0, true);

            const auto measured = measure(process(processor, 0.5, false, channel, true));

            if (channel == 0)
                expect(measured.levelDb > -3.0f, "The left channel should hold its tone: " + juce::String(measured.levelDb, 1) + " dB");
            else
                expect(measured.levelDb < -60.0f, "The right channel should stay silent: " + juce::String(measured.levelDb, 1) + " dB");
        }
    }

    void testUnfreezingReturnsToTheInput()
    {
        beginTest("Switching Freeze off returns to the input");

        SpectralFreezeProcessor processor;
        prepare(processor);

        process(processor, 0.5, true);
        setFreeze(processor, true);
        process(processor, 0.2, false);
        setFreeze(processor, false);

        // The input is silent now, so once the last frozen frames have
        // played out there should be nothing
        const auto measured = measure(process(processor, 0.5, false));
        expect(measured.levelDb < -60.0f, "Still sounding at " + juce::String(measured.levelDb, 1) + " dB after unfreezing");
    }

    juce::int64 samplesFed = 0;
};

static SpectralFreezeTests spectralFreezeTests;
