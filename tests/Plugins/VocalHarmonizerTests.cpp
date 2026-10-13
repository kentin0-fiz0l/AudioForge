/**
 * VocalHarmonizer Tests
 *
 * Each harmony voice should come out at the interval it is set to,
 * in the right direction, and without clicks.
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <cmath>
#include <vector>

#include "../../plugins/VocalHarmonizer/Source/PluginProcessor.h"

class VocalHarmonizerTests : public juce::UnitTest
{
public:
    VocalHarmonizerTests()
        : juce::UnitTest("VocalHarmonizer", "Plugins")
    {
    }

    void runTest() override
    {
        testVoicesComeOutAtTheirInterval();
        testShiftedVoiceHasNoClicks();
    }

private:
    static constexpr double sampleRate = 48000.0;
    static constexpr int blockSize = 512;
    // Bb3: not a whole number of cycles in any round number of milliseconds,
    // so a jump in the read position shows up as a jump in the output
    static constexpr float toneHz = 233.08f;
    static constexpr float toneLevel = 0.5f;

    static void set(VocalHarmonizerProcessor& processor, const char* parameter, float value)
    {
        auto* param = processor.getAPVTS().getParameter(parameter);
        param->setValueNotifyingHost(param->convertTo0to1(value));
    }

    // One voice at the given interval, panned centre, all wet, filters wide open
    static void prepareOneVoice(VocalHarmonizerProcessor& processor, float semitones)
    {
        set(processor, VocalHarmonizerProcessor::PARAM_VOICES, 1.0f);
        set(processor, VocalHarmonizerProcessor::PARAM_VOICE1_PITCH, semitones);
        set(processor, VocalHarmonizerProcessor::PARAM_VOICE1_PAN, 0.0f);
        set(processor, VocalHarmonizerProcessor::PARAM_HIGHPASS, 20.0f);
        set(processor, VocalHarmonizerProcessor::PARAM_LOWPASS, 20000.0f);
        set(processor, VocalHarmonizerProcessor::PARAM_MIX, 100.0f);

        processor.setRateAndBufferSizeDetails(sampleRate, blockSize);
        processor.prepareToPlay(sampleRate, blockSize);
    }

    // Feeds the tone for the given time and returns the left channel,
    // skipping the first 0.2 s while the delay lines fill
    static std::vector<float> process(VocalHarmonizerProcessor& processor, double seconds)
    {
        std::vector<float> output;
        juce::MidiBuffer midi;
        const int skip = static_cast<int>(0.2 * sampleRate);
        const int numBlocks = static_cast<int>(seconds * sampleRate / blockSize);

        for (int block = 0; block < numBlocks; ++block)
        {
            juce::AudioBuffer<float> buffer(2, blockSize);

            for (int i = 0; i < blockSize; ++i)
            {
                const int n = block * blockSize + i;
                const float value = toneLevel * std::sin(juce::MathConstants<float>::twoPi * toneHz
                                                         * static_cast<float>(n) / static_cast<float>(sampleRate));
                buffer.setSample(0, i, value);
                buffer.setSample(1, i, value);
            }

            processor.processBlock(buffer, midi);

            for (int i = 0; i < blockSize; ++i)
                if (block * blockSize + i >= skip)
                    output.push_back(buffer.getSample(0, i));
        }

        return output;
    }

    void testVoicesComeOutAtTheirInterval()
    {
        for (float semitones : { 12.0f, 7.0f, 3.0f, 0.0f, -5.0f, -12.0f })
        {
            beginTest("A voice set to " + juce::String(semitones, 0) + " semitones sounds at that interval");

            VocalHarmonizerProcessor processor;
            prepareOneVoice(processor, semitones);
            const auto output = process(processor, 1.2);

            int crossings = 0;
            float sumSquares = 0.0f;

            for (size_t i = 1; i < output.size(); ++i)
            {
                if ((output[i] < 0.0f) != (output[i - 1] < 0.0f))
                    ++crossings;
                sumSquares += output[i] * output[i];
            }

            // A tone at f Hz crosses zero 2f times a second
            const double measuredHz = crossings * sampleRate / (2.0 * static_cast<double>(output.size()));
            const double expectedHz = toneHz * std::pow(2.0, semitones / 12.0);
            const float level = std::sqrt(sumSquares / static_cast<float>(output.size()));

            expect(level > 0.05f, "Voice is too quiet: RMS " + juce::String(level, 4));
            expect(std::abs(measuredHz / expectedHz - 1.0) < 0.03,
                   "Expected " + juce::String(expectedHz, 1) + " Hz, measured " + juce::String(measuredHz, 1) + " Hz");
        }
    }

    void testShiftedVoiceHasNoClicks()
    {
        for (float semitones : { 7.0f, -12.0f })
        {
            beginTest("A voice shifted " + juce::String(semitones, 0) + " semitones has no clicks");

            VocalHarmonizerProcessor processor;
            prepareOneVoice(processor, semitones);
            const auto output = process(processor, 1.2);

            // The steepest a smooth tone can move from one sample to the next is
            // its peak level times 2 pi f / sampleRate. The centre-panned voice
            // peaks at toneLevel * cos(pi/4); allow twice that slope for the
            // crossfade and filters. A click is a jump far beyond it.
            const double outputHz = toneHz * std::pow(2.0, semitones / 12.0);
            const float smoothLimit = 2.0f * toneLevel * std::sqrt(0.5f)
                                    * static_cast<float>(juce::MathConstants<double>::twoPi * outputHz / sampleRate);

            float largestStep = 0.0f;
            for (size_t i = 1; i < output.size(); ++i)
                largestStep = juce::jmax(largestStep, std::abs(output[i] - output[i - 1]));

            expect(largestStep < smoothLimit,
                   "Largest sample-to-sample step " + juce::String(largestStep, 4)
                       + " exceeds what a smooth tone allows (" + juce::String(smoothLimit, 4) + ")");
        }
    }
};

static VocalHarmonizerTests vocalHarmonizerTests;
