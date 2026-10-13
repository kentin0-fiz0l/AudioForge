/**
 * VocalCompressor Tests
 *
 * The knee should round off the corner of the compression curve: gain
 * reduction starts knee/2 below the threshold and grows smoothly until it
 * meets the ratio line knee/2 above it. More input must never give less
 * output.
 *
 * The detector is linked across channels, so a signal on one side only
 * should be compressed just as hard as the same signal on both sides.
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <cmath>

#include "../../plugins/VocalCompressor/Source/PluginProcessor.h"

class VocalCompressorTests : public juce::UnitTest
{
public:
    VocalCompressorTests()
        : juce::UnitTest("VocalCompressor", "Plugins")
    {
    }

    void runTest() override
    {
        testSoftKneeFollowsQuadraticCurve();
        testSoftKneeNeverFoldsBack();
        testOneSidedSignalIsCompressedLikeCentred();
    }

private:
    static constexpr double sampleRate = 48000.0;
    static constexpr int blockSize = 512;

    static void setParam(VocalCompressorProcessor& processor, const char* id, float value)
    {
        auto* param = processor.getAPVTS().getParameter(id);
        param->setValueNotifyingHost(param->convertTo0to1(value));
    }

    // A compressor with a fast peak detector, so a steady input settles on
    // the static curve within a few samples.
    static void setUp(VocalCompressorProcessor& processor, float kneeDb, int detection)
    {
        processor.prepareToPlay(sampleRate, blockSize);
        setParam(processor, VocalCompressorProcessor::PARAM_THRESHOLD, -20.0f);
        setParam(processor, VocalCompressorProcessor::PARAM_RATIO, 20.0f);
        setParam(processor, VocalCompressorProcessor::PARAM_ATTACK, 0.1f);
        setParam(processor, VocalCompressorProcessor::PARAM_RELEASE, 10.0f);
        setParam(processor, VocalCompressorProcessor::PARAM_KNEE, kneeDb);
        setParam(processor, VocalCompressorProcessor::PARAM_MAKEUP, 0.0f);
        setParam(processor, VocalCompressorProcessor::PARAM_PARALLEL_MIX, 0.0f);
        setParam(processor, VocalCompressorProcessor::PARAM_DETECTION, static_cast<float>(detection));
    }

    // Feeds a steady level for a tenth of a second and returns the left
    // channel's output level in dB at the end.
    static float settledOutputDb(VocalCompressorProcessor& processor, float leftDb, float rightDb)
    {
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midi;
        const float left = juce::Decibels::decibelsToGain(leftDb, -200.0f);
        const float right = juce::Decibels::decibelsToGain(rightDb, -200.0f);

        for (int block = 0; block < static_cast<int>(0.1 * sampleRate / blockSize); ++block)
        {
            juce::FloatVectorOperations::fill(buffer.getWritePointer(0), left, blockSize);
            juce::FloatVectorOperations::fill(buffer.getWritePointer(1), right, blockSize);
            processor.processBlock(buffer, midi);
        }

        return juce::Decibels::gainToDecibels(buffer.getSample(0, blockSize - 1), -200.0f);
    }

    void testSoftKneeFollowsQuadraticCurve()
    {
        beginTest("Soft knee follows the standard quadratic curve");

        const float threshold = -20.0f, ratio = 20.0f, knee = 12.0f;
        VocalCompressorProcessor processor;
        setUp(processor, knee, 1);

        for (float inputDb : { -30.0f, -26.0f, -23.0f, -20.0f, -17.0f, -14.0f, -10.0f })
        {
            const float over = inputDb - threshold;
            float expectedDb = inputDb;
            if (2.0f * over >= knee)
                expectedDb = threshold + over / ratio;
            else if (2.0f * over > -knee)
                expectedDb = inputDb + (1.0f / ratio - 1.0f) * (over + knee / 2.0f) * (over + knee / 2.0f) / (2.0f * knee);

            const float outputDb = settledOutputDb(processor, inputDb, inputDb);
            expectWithinAbsoluteError(outputDb, expectedDb, 0.1f,
                                      "Output at " + juce::String(inputDb) + " dB in");
        }
    }

    void testSoftKneeNeverFoldsBack()
    {
        beginTest("Output rises steadily through the knee");

        VocalCompressorProcessor processor;
        setUp(processor, 12.0f, 1);

        float previousDb = settledOutputDb(processor, -30.0f, -30.0f);
        for (float inputDb = -29.5f; inputDb <= 0.0f; inputDb += 0.5f)
        {
            const float outputDb = settledOutputDb(processor, inputDb, inputDb);
            expect(outputDb >= previousDb - 0.01f,
                   "Output fell from " + juce::String(previousDb) + " to " + juce::String(outputDb)
                       + " dB as the input rose to " + juce::String(inputDb) + " dB");
            expect(outputDb - previousDb <= 0.51f,
                   "Output jumped from " + juce::String(previousDb) + " to " + juce::String(outputDb) + " dB");
            previousDb = outputDb;
        }
    }

    void testOneSidedSignalIsCompressedLikeCentred()
    {
        beginTest("RMS detector: one-sided signal is compressed like the same signal on both sides");

        VocalCompressorProcessor centred;
        setUp(centred, 0.0f, 0);
        const float centredDb = settledOutputDb(centred, -6.0f, -6.0f);

        VocalCompressorProcessor leftOnly;
        setUp(leftOnly, 0.0f, 0);
        const float leftOnlyDb = settledOutputDb(leftOnly, -6.0f, -200.0f);

        expectWithinAbsoluteError(leftOnlyDb, centredDb, 0.1f,
                                  "Left-only output level should match the centred output level");
        // 14 dB over a -20 dB threshold at 20:1 leaves -19.3 dB
        expectWithinAbsoluteError(centredDb, -19.3f, 0.1f, "Centred signal sits on the ratio line");
    }
};

static VocalCompressorTests vocalCompressorTests;
