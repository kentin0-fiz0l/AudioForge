/**
 * Spectral Processor Tests
 *
 * Tests for the FFT analysis and resynthesis FreezeFX runs while frozen.
 * With the spectrum left untouched, what comes out should be what went in,
 * delayed by a fixed latency, at every FFT size and overlap.
 */

#include <juce_audio_basics/juce_audio_basics.h>
#include <cmath>
#include <vector>

#include "../../plugins/FreezeFX/Source/SpectralProcessor.h"

class SpectralProcessorTests : public juce::UnitTest
{
public:
    SpectralProcessorTests()
        : juce::UnitTest("Spectral Processor", "Plugins")
    {
    }

    void runTest() override
    {
        testReconstructsInput();
        testSpectralChangesReachOutput();
    }

private:
    static constexpr int numChannels = 2;

    struct Result
    {
        bool finite = true;
        float worstError = 0.0f; // Largest difference from gain * delayed input
    };

    // Runs noise through the processor in blocks and compares the output with
    // the input delayed by the processor's latency and scaled by expectedGain.
    // The two channels carry different noise, so one leaking into the other
    // shows up as error.
    Result run(SpectralProcessor& processor, int blockSize, float expectedGain)
    {
        const int fftSize = processor.getFFTSize();
        const int latency = fftSize - 1;
        const int numSamples = fftSize * 6;

        std::vector<std::vector<float>> input(numChannels), output(numChannels);
        juce::Random random(1234);

        for (auto& channel : input)
            for (int i = 0; i < numSamples; ++i)
                channel.push_back(random.nextFloat() - 0.5f);

        for (int start = 0; start < numSamples; start += blockSize)
        {
            const int length = juce::jmin(blockSize, numSamples - start);
            juce::AudioBuffer<float> buffer(numChannels, length);

            for (int ch = 0; ch < numChannels; ++ch)
                buffer.copyFrom(ch, 0, input[(size_t) ch].data() + start, length);

            processor.processBlock(buffer);

            for (int ch = 0; ch < numChannels; ++ch)
                output[(size_t) ch].insert(output[(size_t) ch].end(),
                                           buffer.getReadPointer(ch),
                                           buffer.getReadPointer(ch) + length);
        }

        Result result;

        // Skip the first window, which is still filling
        for (int ch = 0; ch < numChannels; ++ch)
        {
            for (int i = fftSize; i < numSamples - latency; ++i)
            {
                const float out = output[(size_t) ch][(size_t) (i + latency)];
                result.finite = result.finite && std::isfinite(out);
                result.worstError = juce::jmax(result.worstError,
                                               std::abs(out - expectedGain * input[(size_t) ch][(size_t) i]));
            }
        }

        return result;
    }

    void testReconstructsInput()
    {
        beginTest("An untouched spectrum gives back the input, delayed");

        bool allFinite = true;
        bool allAccurate = true;
        juce::String firstNonFinite, firstInaccurate;

        for (int fftSize : { 1024, 2048, 4096, 8192 })
        {
            for (int overlap : { 2, 4, 8 })
            {
                // 441 does not divide any hop size, so frames fall mid-block
                for (int blockSize : { 512, 441 })
                {
                    SpectralProcessor processor;
                    processor.setFFTSize(fftSize);
                    processor.setOverlapFactor(overlap);
                    processor.prepare(48000.0, blockSize, numChannels);

                    const auto result = run(processor, blockSize, 1.0f);
                    const bool accurate = result.finite && result.worstError < 0.001f;

                    const auto where = "FFT " + juce::String(fftSize) + ", overlap " + juce::String(overlap)
                                       + ", block " + juce::String(blockSize);

                    if (! result.finite && firstNonFinite.isEmpty())
                        firstNonFinite = where;
                    if (! accurate && firstInaccurate.isEmpty())
                        firstInaccurate = where + ": error " + juce::String(result.worstError, 4);

                    allFinite = allFinite && result.finite;
                    allAccurate = allAccurate && accurate;
                }
            }
        }

        expect(allFinite, "Non-finite output at " + firstNonFinite);
        expect(allAccurate, "Output differs from the delayed input at " + firstInaccurate);
    }

    void testSpectralChangesReachOutput()
    {
        beginTest("Halving every magnitude halves the output");

        SpectralProcessor processor;
        processor.setFFTSize(2048);
        processor.setOverlapFactor(4);
        processor.prepare(48000.0, 512, numChannels);
        processor.setSpectralCallback([] (int, std::vector<float>& magnitude, std::vector<float>&)
        {
            for (auto& value : magnitude)
                value *= 0.5f;
        });

        const auto result = run(processor, 512, 0.5f);

        expect(result.finite, "Non-finite output");
        expect(result.worstError < 0.001f,
               "Output is not half the delayed input: error " + juce::String(result.worstError, 4));
    }
};

static SpectralProcessorTests spectralProcessorTests;
