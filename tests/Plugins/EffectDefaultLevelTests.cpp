/**
 * Effect default level tests
 *
 * Loading an effect should not change the level of a track before anything
 * is adjusted. These check the effects that did: a reverb that added level,
 * two compressors that started out squashing, and a wave shaper that was
 * not transparent with its drive at zero.
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <cmath>

#include <dsp/ReverbLevels.h>
#include <dsp/WaveShaping.h>

#include "../../plugins/ShimmerReverb/Source/ShimmerEngine.h"
#include "../../plugins/VintageCompressor/Source/PluginProcessor.h"
#include "../../plugins/SimpleComp/Source/PluginProcessor.h"
#include "../../plugins/WaveShaper/Source/PluginProcessor.h"

class EffectDefaultLevelTests : public juce::UnitTest
{
public:
    EffectDefaultLevelTests()
        : juce::UnitTest("Effect Default Levels", "Plugins")
    {
    }

    void runTest() override
    {
        testReverbLevelsAllowForJucesScaling();
        testShimmerReverbKeepsTheLevel();
        testShimmerReverbMixAtZeroIsTheDrySignal();
        testVintageCompressorStartsOutGentle();
        testVintageCompressorStillCompresses();
        testSimpleCompStartsOutGentle();
        testSimpleCompStillCompresses();
        testWaveShaperIsTransparentAtDefaults();
        testSoftClipHasNoJump();
    }

private:
    static constexpr double sampleRate = 48000.0;
    static constexpr int blockSize = 512;

    // The level every AudioForge instrument is calibrated to: peaks at -9 dBFS
    static constexpr float calibratedPeak = 0.355f;

    struct Result
    {
        float peakChangeDb = 0.0f;      // Loudest output sample against loudest input sample, whole run
        float levelChangeDb = 0.0f;     // RMS of the output against the input, after the first half second
        float worstDifference = 0.0f;   // Between output and input, sample for sample
    };

    // A second and a half of noise through anything that processes a buffer
    template <typename Process>
    static Result run(float noisePeak, Process&& process)
    {
        juce::Random random(7);
        juce::AudioBuffer<float> input(2, blockSize), buffer(2, blockSize);

        float inputPeak = 0.0f, outputPeak = 0.0f;
        double inputSquares = 0.0, outputSquares = 0.0;
        Result result;

        const int numBlocks = static_cast<int>(1.5 * sampleRate / blockSize);
        const int settledBlock = static_cast<int>(0.5 * sampleRate / blockSize);

        for (int block = 0; block < numBlocks; ++block)
        {
            for (int channel = 0; channel < 2; ++channel)
                for (int i = 0; i < blockSize; ++i)
                    input.setSample(channel, i, noisePeak * (2.0f * random.nextFloat() - 1.0f));

            buffer.makeCopyOf(input);
            process(buffer);

            inputPeak = juce::jmax(inputPeak, input.getMagnitude(0, blockSize));
            outputPeak = juce::jmax(outputPeak, buffer.getMagnitude(0, blockSize));

            for (int channel = 0; channel < 2; ++channel)
            {
                for (int i = 0; i < blockSize; ++i)
                {
                    const float in = input.getSample(channel, i);
                    const float out = buffer.getSample(channel, i);
                    result.worstDifference = juce::jmax(result.worstDifference, std::abs(out - in));

                    if (block >= settledBlock)
                    {
                        inputSquares += in * in;
                        outputSquares += out * out;
                    }
                }
            }
        }

        result.peakChangeDb = juce::Decibels::gainToDecibels(outputPeak / inputPeak);
        result.levelChangeDb = static_cast<float>(10.0 * std::log10(outputSquares / inputSquares));
        return result;
    }

    static Result runProcessor(juce::AudioProcessor& processor, float noisePeak)
    {
        processor.setRateAndBufferSizeDetails(sampleRate, blockSize);
        processor.prepareToPlay(sampleRate, blockSize);

        juce::MidiBuffer midi;
        return run(noisePeak, [&] (juce::AudioBuffer<float>& buffer) { processor.processBlock(buffer, midi); });
    }

    static void set(juce::AudioProcessor& processor, const juce::String& name, float value)
    {
        for (auto* param : processor.getParameters())
        {
            if (param->getName(64) == name)
            {
                auto* ranged = dynamic_cast<juce::RangedAudioParameter*>(param);
                ranged->setValueNotifyingHost(ranged->convertTo0to1(value));
                return;
            }
        }

        jassertfalse;
    }

    void testReverbLevelsAllowForJucesScaling()
    {
        beginTest("Reverb levels: a mix of zero is the dry signal, untouched");

        juce::dsp::Reverb reverb;
        reverb.prepare({ sampleRate, static_cast<juce::uint32>(blockSize), 2 });

        const auto levels = AudioForge::DSP::reverbLevelsForMix(0.0f);
        juce::dsp::Reverb::Parameters params;
        params.wetLevel = levels.wet;
        params.dryLevel = levels.dry;
        reverb.setParameters(params);

        const auto result = run(0.5f, [&] (juce::AudioBuffer<float>& buffer)
        {
            juce::dsp::AudioBlock<float> block(buffer);
            reverb.process(juce::dsp::ProcessContextReplacing<float>(block));
        });

        // JUCE's reverb glides to new levels, so the first block is still on its way
        expectWithinAbsoluteError(result.levelChangeDb, 0.0f, 0.01f);

        // JUCE multiplies the wet level by 3 and the dry level by 2
        const auto wet = AudioForge::DSP::reverbLevelsForMix(1.0f);
        expectEquals(wet.dry, 0.0f, "A mix of one has no dry signal");
        expectWithinAbsoluteError(wet.wet * 3.0f, 1.0f, 1.0e-6f, "A mix of one is the reverb at its nominal level");
        expectWithinAbsoluteError(levels.dry * 2.0f, 1.0f, 1.0e-6f, "A mix of zero is the dry signal at its own level");
    }

    void testShimmerReverbKeepsTheLevel()
    {
        beginTest("ShimmerReverb at its defaults does not add level");

        ShimmerEngine engine;
        engine.prepareToPlay(sampleRate, blockSize);

        const auto result = run(0.5f, [&] (juce::AudioBuffer<float>& buffer) { engine.processBlock(buffer); });

        // Peaks stay where they were, as with PlateReverb. Half of the dry
        // signal is swapped for reverb, which is quieter on noise, so the
        // level comes down; it must not come down by much.
        expect(std::abs(result.peakChangeDb) < 1.5f,
               "Peak changed by " + juce::String(result.peakChangeDb, 1) + " dB");
        expect(result.levelChangeDb < 1.0f && result.levelChangeDb > -6.0f,
               "Level changed by " + juce::String(result.levelChangeDb, 1) + " dB");
    }

    void testShimmerReverbMixAtZeroIsTheDrySignal()
    {
        beginTest("ShimmerReverb with Mix at zero is the dry signal");

        ShimmerEngine engine;
        engine.prepareToPlay(sampleRate, blockSize);
        engine.setMix(0.0f);

        const auto result = run(0.5f, [&] (juce::AudioBuffer<float>& buffer) { engine.processBlock(buffer); });

        expectWithinAbsoluteError(result.worstDifference, 0.0f, 1.0e-6f);
    }

    void testVintageCompressorStartsOutGentle()
    {
        beginTest("VintageCompressor at its defaults leaves a calibrated track within 1 dB");

        CompressorProcessor processor;
        const auto result = runProcessor(processor, calibratedPeak);

        expect(std::abs(result.levelChangeDb) < 1.0f,
               "Level changed by " + juce::String(result.levelChangeDb, 1) + " dB");
    }

    void testVintageCompressorStillCompresses()
    {
        beginTest("VintageCompressor compresses once the threshold comes down");

        CompressorProcessor processor;
        set(processor, "Threshold", -30.0f);
        const auto result = runProcessor(processor, calibratedPeak);

        expect(result.levelChangeDb < -6.0f,
               "Level changed by " + juce::String(result.levelChangeDb, 1) + " dB");
    }

    void testSimpleCompStartsOutGentle()
    {
        beginTest("SimpleComp at its defaults leaves a calibrated track within 1 dB");

        SimpleCompProcessor processor;
        const auto result = runProcessor(processor, calibratedPeak);

        expect(std::abs(result.levelChangeDb) < 1.0f,
               "Level changed by " + juce::String(result.levelChangeDb, 1) + " dB");

        // Makeup gain reaches the start of a note before the compression does
        expect(result.peakChangeDb < 0.5f,
               "Peak rose by " + juce::String(result.peakChangeDb, 1) + " dB");
    }

    void testSimpleCompStillCompresses()
    {
        beginTest("SimpleComp compresses once the threshold comes down");

        // Makeup gain hides the reduction in the level, so look at how far
        // a loud signal is brought towards a quiet one instead
        SimpleCompProcessor forLoud, forSoft;
        set(forLoud, "Threshold", -30.0f);
        set(forSoft, "Threshold", -30.0f);

        const auto loud = runProcessor(forLoud, calibratedPeak);
        const auto soft = runProcessor(forSoft, calibratedPeak * 0.1f);

        expect(loud.levelChangeDb < soft.levelChangeDb - 6.0f,
               "A loud signal should be turned down by much more than a quiet one: "
                   + juce::String(loud.levelChangeDb, 1) + " dB against " + juce::String(soft.levelChangeDb, 1) + " dB");
    }

    void testWaveShaperIsTransparentAtDefaults()
    {
        beginTest("WaveShaper with its drive at zero and its tone open leaves the signal alone");

        WaveShaperAudioProcessor processor;
        const auto result = runProcessor(processor, 0.5f);

        // What is left is the DC blocker, a high-pass at 20 Hz. It moves the
        // peaks of noise by under a dB and leaves the level alone.
        expect(std::abs(result.peakChangeDb) < 1.0f,
               "Peak changed by " + juce::String(result.peakChangeDb, 1) + " dB");
        expect(std::abs(result.levelChangeDb) < 0.2f,
               "Level changed by " + juce::String(result.levelChangeDb, 2) + " dB");

        // The tone control still works below fully open
        WaveShaperAudioProcessor dark;
        set(dark, "Tone", 0.0f);
        const auto filtered = runProcessor(dark, 0.5f);

        expect(filtered.levelChangeDb < -10.0f,
               "Tone at zero took off " + juce::String(-filtered.levelChangeDb, 1) + " dB");
    }

    void testSoftClipHasNoJump()
    {
        beginTest("Soft clip rises smoothly through full scale");

        using AudioForge::DSP::WaveShaping;

        // It used to fall from 1.0 to 0.5 as the input passed 1.0
        float previous = WaveShaping::softClip(0.0f);
        for (float input = 0.001f; input <= 3.0f; input += 0.001f)
        {
            const float output = WaveShaping::softClip(input);
            expect(output >= previous, "Output fell at an input of " + juce::String(input, 3));
            expect(output - previous < 0.002f, "Output jumped at an input of " + juce::String(input, 3));
            expectWithinAbsoluteError(WaveShaping::softClip(-input), -output, 1.0e-6f);
            previous = output;
        }

        expectEquals(WaveShaping::softClip(0.9f), 0.9f, "Below full scale the signal passes untouched");
        expectWithinAbsoluteError(WaveShaping::softClip(1.001f) - WaveShaping::softClip(1.0f), 0.001f, 0.0001f,
                                  "The curve leaves full scale at the slope it arrived with");
        expect(WaveShaping::softClip(3.0f) <= 1.5f, "The output stays under 1.5");
    }
};

static EffectDefaultLevelTests effectDefaultLevelTests;
