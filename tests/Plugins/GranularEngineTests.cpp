/**
 * GranularEngine tests
 *
 * At its defaults GranularEngine came out about 21 dB under what went in.
 * Three things did it: the default grains, 50 ms ten times a second, left
 * half of every second silent; the grains were scaled down as if all 64 of
 * them were always playing; and a stereo input was written into the grain
 * buffer twice per block, left then right, so grains were cut from a stream
 * that jumped back half a block every 512 samples.
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <cmath>

#include "../../plugins/GranularEngine/Source/PluginProcessor.h"

class GranularEngineTests : public juce::UnitTest
{
public:
    GranularEngineTests()
        : juce::UnitTest("GranularEngine", "Plugins")
    {
    }

    void runTest() override
    {
        testDefaultsKeepTheLevel();
        testStereoInputIsOneStream();
    }

private:
    static constexpr double sampleRate = 48000.0;
    static constexpr int blockSize = 512;
    static constexpr int numBlocks = 188;            // Two seconds
    static constexpr int settledBlock = 94;          // Measure the second one, once the buffer has filled
    static constexpr float calibratedPeak = 0.355f;  // -9 dBFS, where every AudioForge instrument peaks

    // Runs two seconds of input, the same on both channels, and returns the
    // left channel's output over the second second
    template <typename Source>
    static std::vector<float> run(Source&& source, std::vector<float>* inputOut = nullptr)
    {
        GranularEngineProcessor processor;
        processor.setRateAndBufferSizeDetails(sampleRate, blockSize);
        processor.prepareToPlay(sampleRate, blockSize);

        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midi;
        std::vector<float> output;

        for (int block = 0; block < numBlocks; ++block)
        {
            for (int i = 0; i < blockSize; ++i)
            {
                const float x = source(block * blockSize + i);
                buffer.setSample(0, i, x);
                buffer.setSample(1, i, x);

                if (inputOut != nullptr && block >= settledBlock)
                    inputOut->push_back(x);
            }

            processor.processBlock(buffer, midi);

            if (block >= settledBlock)
                output.insert(output.end(), buffer.getReadPointer(0), buffer.getReadPointer(0) + blockSize);
        }

        return output;
    }

    static double meanSquare(const std::vector<float>& x)
    {
        double sum = 0.0;
        for (float v : x)
            sum += v * v;
        return sum / x.size();
    }

    void testDefaultsKeepTheLevel()
    {
        beginTest("At its defaults the grains come out near the level that went in");

        juce::Random random(7);
        std::vector<float> input;
        const auto output = run([&] (int) { return calibratedPeak * (2.0f * random.nextFloat() - 1.0f); }, &input);

        const float levelChangeDb = static_cast<float>(10.0 * std::log10(meanSquare(output) / meanSquare(input)));

        float inputPeak = 0.0f, outputPeak = 0.0f;
        for (size_t i = 0; i < input.size(); ++i)
        {
            inputPeak = juce::jmax(inputPeak, std::abs(input[i]));
            outputPeak = juce::jmax(outputPeak, std::abs(output[i]));
        }
        const float peakChangeDb = juce::Decibels::gainToDecibels(outputPeak / inputPeak);

        logMessage("Level " + juce::String(levelChangeDb, 1) + " dB, peak " + juce::String(peakChangeDb, 1) + " dB");

        // Fifty-millisecond grains twenty times a second just touch. Each is
        // shaped by its window, so it is not the full level, but near it.
        expect(levelChangeDb > -6.0f && levelChangeDb < 1.0f,
               "Level changed by " + juce::String(levelChangeDb, 1) + " dB");
        expect(peakChangeDb < 3.0f,
               "Peak rose by " + juce::String(peakChangeDb, 1) + " dB");
    }

    void testStereoInputIsOneStream()
    {
        beginTest("A stereo input reaches the grains as one unbroken stream");

        // A 1 kHz tone, the same on both sides. Grains of an unbroken tone are
        // tone too; grains of one that jumps back every block are smeared
        // across the spectrum.
        constexpr double frequency = 1000.0;
        const auto output = run([&] (int n)
        {
            return 0.3f * static_cast<float>(std::sin(juce::MathConstants<double>::twoPi * frequency * n / sampleRate));
        });

        constexpr int order = 15;
        constexpr int size = 1 << order;
        juce::dsp::FFT fft(order);
        std::vector<float> data(2 * size, 0.0f);
        std::copy(output.begin(), output.begin() + size, data.begin());
        fft.performFrequencyOnlyForwardTransform(data.data());

        // 100 Hz either side of the tone holds a 50 ms Hann grain's main lobe
        const double binWidth = sampleRate / size;
        double near = 0.0, total = 0.0;
        for (int bin = 1; bin < size / 2; ++bin)
        {
            const double energy = static_cast<double>(data[bin]) * data[bin];
            total += energy;
            if (std::abs(bin * binWidth - frequency) <= 100.0)
                near += energy;
        }

        expect(total > 0.0, "The grains are silent");
        expect(near / total > 0.95,
               "Only " + juce::String(100.0 * near / total, 1) + "% of the output is near the tone");
    }
};

static GranularEngineTests granularEngineTests;
