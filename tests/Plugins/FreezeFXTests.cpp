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
        testFrozenSoundHoldsItsLevelAndPitch();
        testHoldsAtEveryFrameSizeAndOverlap();
        testHalfMixIsHalfTheFrozenSound();
        testTheSameFreezeSoundsTheSameTwice();
        testChannelsAreFrozenSeparately();
    }

private:
    static constexpr double sampleRate = 48000.0;
    static constexpr int blockSize = 512;
    static constexpr float toneHz = 1000.0f;
    static constexpr float toneLevel = 0.5f;

    // Runs the processor for the given time, feeding a 1 kHz tone or silence,
    // and returns the left channel of what came out
    // (or of another channel, if asked)
    std::vector<float> process(FreezeFXProcessor& processor, double seconds, bool withTone, int outputChannel = 0,
                               bool leftOnly = false)
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
                    buffer.setSample(1, i, leftOnly ? 0.0f : value);
                }
            }

            samplesFed += blockSize;
            processor.processBlock(buffer, midi);
            output.insert(output.end(), buffer.getReadPointer(outputChannel), buffer.getReadPointer(outputChannel) + blockSize);
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

        // A 1 kHz tone crosses zero 2000 times a second. This only asks for
        // the same neighbourhood; the test after this one asks for the pitch.
        expect(crossingsPerSecond > 1500.0 && crossingsPerSecond < 2500.0,
               "Frozen sound is not centred on the 1 kHz tone: "
                   + juce::String(static_cast<int>(crossingsPerSecond)) + " zero crossings a second");

        // Switching Freeze off returns to the (silent) input
        setFreeze(processor, false);
        const auto released = process(processor, 0.1, false);
        expectWithinAbsoluteError(rms(released, released.size() / 2), 0.0f, 0.0001f);
    }

    struct Held
    {
        float levelDb;              // Against the level of the tone that was playing
        double crossingsPerSecond;  // A tone at f Hz crosses zero 2f times a second
        std::vector<float> samples;
    };

    static void set(FreezeFXProcessor& processor, const char* parameter, float normalisedValue)
    {
        processor.getAPVTS().getParameter(parameter)->setValueNotifyingHost(normalisedValue);
    }

    // Play the tone, freeze it, stop the tone, and measure the second half
    // of the next second: only the frozen sound is left by then
    Held freezeAndMeasure(FreezeFXProcessor& processor)
    {
        prepare(processor);

        process(processor, 0.5, true);
        setFreeze(processor, true);
        process(processor, 0.1, true);

        auto frozen = process(processor, 1.0, false);
        const auto from = frozen.size() / 2;

        int crossings = 0;
        for (size_t i = from + 1; i < frozen.size(); ++i)
            if ((frozen[i] < 0.0f) != (frozen[i - 1] < 0.0f))
                ++crossings;

        const float playing = toneLevel / std::sqrt(2.0f);
        return { juce::Decibels::gainToDecibels(rms(frozen, from) / playing),
                 crossings * sampleRate / static_cast<double>(frozen.size() - from),
                 std::move(frozen) };
    }

    void expectHeld(const Held& held)
    {
        logMessage("Held at " + juce::String(held.levelDb, 1) + " dB, "
                       + juce::String(held.crossingsPerSecond / 2.0, 1) + " Hz");

        expect(std::abs(held.levelDb) < 2.0f,
               "The frozen sound is " + juce::String(held.levelDb, 1) + " dB from the level that was playing");
        expect(held.crossingsPerSecond > 1990.0 && held.crossingsPerSecond < 2010.0,
               "The frozen 1 kHz tone has " + juce::String(static_cast<int>(held.crossingsPerSecond))
                   + " zero crossings a second");
    }

    void testFrozenSoundHoldsItsLevelAndPitch()
    {
        // 0.5 is the default. At zero the frozen sound holds exactly as it
        // was; above it the phases drift, the same way on every run. Before
        // the frozen sound kept its phase turning it sat 12 dB down, and
        // 3% sharp, at every setting.
        for (float phaseRandom : { 0.0f, 0.2f, 0.5f, 1.0f })
        {
            beginTest("The frozen sound holds the level and pitch that were playing, with Phase Random at "
                          + juce::String(phaseRandom, 1));

            FreezeFXProcessor processor;
            set(processor, FreezeFXProcessor::PARAM_PHASE_RANDOM, phaseRandom);
            expectHeld(freezeAndMeasure(processor));
        }
    }

    void testHoldsAtEveryFrameSizeAndOverlap()
    {
        // Both are choices: four frame sizes from 1024 to 8192, and
        // overlaps of 2, 4 and 8. The phase moves on by a different amount
        // per frame in each, often by more than a full turn.
        for (int fftChoice = 0; fftChoice < 4; ++fftChoice)
        {
            for (int overlapChoice = 0; overlapChoice < 3; ++overlapChoice)
            {
                beginTest("The frozen sound holds with a frame of " + juce::String(1024 << fftChoice)
                              + " and an overlap of " + juce::String(2 << overlapChoice));

                FreezeFXProcessor processor;
                set(processor, FreezeFXProcessor::PARAM_FFT_SIZE, static_cast<float>(fftChoice) / 3.0f);
                set(processor, FreezeFXProcessor::PARAM_OVERLAP, static_cast<float>(overlapChoice) / 2.0f);
                set(processor, FreezeFXProcessor::PARAM_PHASE_RANDOM, 0.0f);
                expectHeld(freezeAndMeasure(processor));
            }
        }
    }

    void testHalfMixIsHalfTheFrozenSound()
    {
        beginTest("With Freeze Mix at half and the input silent, the frozen sound is 6 dB down");

        FreezeFXProcessor processor;
        set(processor, FreezeFXProcessor::PARAM_PHASE_RANDOM, 0.0f);
        set(processor, FreezeFXProcessor::PARAM_FREEZE_MIX, 0.5f);

        const auto held = freezeAndMeasure(processor);

        expectWithinAbsoluteError(held.levelDb, -6.0f, 0.5f);
    }

    void testTheSameFreezeSoundsTheSameTwice()
    {
        beginTest("Freezing the same sound twice gives the same result, drift included");

        FreezeFXProcessor first, second;
        const auto a = freezeAndMeasure(first).samples;
        const auto b = freezeAndMeasure(second).samples;

        float difference = 0.0f;
        for (size_t i = 0; i < juce::jmin(a.size(), b.size()); ++i)
            difference = juce::jmax(difference, std::abs(a[i] - b[i]));

        expect(a.size() == b.size() && difference < 1.0e-6f,
               "Two renders differ by up to " + juce::String(difference, 6));
    }

    void testChannelsAreFrozenSeparately()
    {
        beginTest("Each channel freezes its own sound");

        // The tone is in the left channel only. The right was silent when
        // Freeze was switched on, so it has to stay silent while the left
        // holds its tone.
        for (int channel : { 0, 1 })
        {
            FreezeFXProcessor processor;
            prepare(processor);

            process(processor, 0.5, true, 0, true);
            setFreeze(processor, true);
            process(processor, 0.1, true, 0, true);

            const auto frozen = process(processor, 0.5, false, channel, true);
            const float level = rms(frozen, frozen.size() / 2);

            if (channel == 0)
                expect(level > 0.25f, "The left channel should hold its tone: RMS " + juce::String(level, 4));
            else
                expectWithinAbsoluteError(level, 0.0f, 0.001f);
        }
    }

    juce::int64 samplesFed = 0;
};

static FreezeFXTests freezeFXTests;
