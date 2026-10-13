/**
 * VocalDeEsser Plugin Tests
 *
 * Behavioural tests for the VocalDeEsser processor:
 * - Gain reduction is applied to the sibilant band only
 * - Signal passes unchanged when nothing crosses the threshold, or at 0% mix
 * - processBlock does not allocate memory
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <atomic>
#include <cstdlib>
#include <new>

#include "../../plugins/VocalDeEsser/Source/PluginProcessor.h"

//==============================================================================
// Counts global operator new calls made on the current thread while enabled.
// Replacing the global operators applies to the whole TestRunner, so they only
// count, and otherwise behave like the defaults.
namespace
{
    thread_local bool countAllocations = false;
    std::atomic<int> allocationCount { 0 };

    struct ScopedAllocationCounter
    {
        ScopedAllocationCounter()  { allocationCount = 0; countAllocations = true; }
        ~ScopedAllocationCounter() { countAllocations = false; }
        int count() const          { return allocationCount.load(); }
    };
}

void* operator new (std::size_t size)
{
    if (countAllocations)
        ++allocationCount;

    if (auto* p = std::malloc (size == 0 ? 1 : size))
        return p;

    throw std::bad_alloc();
}

void* operator new[] (std::size_t size)                   { return operator new (size); }
void operator delete (void* p) noexcept                   { std::free (p); }
void operator delete[] (void* p) noexcept                 { std::free (p); }
void operator delete (void* p, std::size_t) noexcept      { std::free (p); }
void operator delete[] (void* p, std::size_t) noexcept    { std::free (p); }

//==============================================================================
class VocalDeEsserPluginTests : public juce::UnitTest
{
public:
    VocalDeEsserPluginTests()
        : juce::UnitTest("VocalDeEsser Plugin", "Plugins")
    {
    }

    void runTest() override
    {
        testOnlySibilantBandIsReduced();
        testBelowThresholdPassesUnchanged();
        testZeroMixPassesUnchanged();
        testProcessBlockDoesNotAllocate();
    }

private:
    static constexpr double sampleRate = 48000.0;
    static constexpr int blockSize = 512;

    static void setParam(VocalDeEsserProcessor& processor, const char* id, float value)
    {
        auto* param = processor.getAPVTS().getParameter(id);
        param->setValueNotifyingHost(param->convertTo0to1(value));
    }

    // Fills a stereo buffer with a 200 Hz tone (body of the voice) and a 7 kHz
    // tone (sibilance), continuing the phase from startSample.
    static void fillTones(juce::AudioBuffer<float>& buffer, int startSample,
                          float lowAmp, float highAmp)
    {
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            const double t = (startSample + i) / sampleRate;
            const float value = lowAmp * (float) std::sin(juce::MathConstants<double>::twoPi * 200.0 * t)
                              + highAmp * (float) std::sin(juce::MathConstants<double>::twoPi * 7000.0 * t);

            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
                buffer.setSample(ch, i, value);
        }
    }

    // Amplitude of one frequency in a signal spanning a whole number of its cycles.
    static float amplitudeAt(const std::vector<float>& signal, double freq)
    {
        double re = 0.0, im = 0.0;

        for (size_t i = 0; i < signal.size(); ++i)
        {
            const double phase = juce::MathConstants<double>::twoPi * freq * (double) i / sampleRate;
            re += signal[i] * std::cos(phase);
            im += signal[i] * std::sin(phase);
        }

        return (float) (2.0 * std::sqrt(re * re + im * im) / (double) signal.size());
    }

    // Runs one second of the two tones through the processor and returns the
    // last 4800 samples (0.1 s: whole cycles of both 200 Hz and 7 kHz) of the
    // left channel, along with the matching input.
    static void runTones(VocalDeEsserProcessor& processor, float lowAmp, float highAmp,
                         std::vector<float>& inputTail, std::vector<float>& outputTail)
    {
        const int totalSamples = (int) sampleRate;
        const int tailLength = 4800;
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midi;

        inputTail.clear();
        outputTail.clear();

        for (int start = 0; start < totalSamples; start += blockSize)
        {
            const int n = std::min(blockSize, totalSamples - start);
            buffer.setSize(2, n, false, false, true);
            fillTones(buffer, start, lowAmp, highAmp);

            for (int i = 0; i < n; ++i)
                if (start + i >= totalSamples - tailLength)
                    inputTail.push_back(buffer.getSample(0, i));

            processor.processBlock(buffer, midi);

            for (int i = 0; i < n; ++i)
                if (start + i >= totalSamples - tailLength)
                    outputTail.push_back(buffer.getSample(0, i));
        }
    }

    void testOnlySibilantBandIsReduced()
    {
        beginTest("Gain reduction applies to the sibilant band only");

        VocalDeEsserProcessor processor;
        processor.prepareToPlay(sampleRate, blockSize);
        setParam(processor, VocalDeEsserProcessor::PARAM_THRESHOLD, -40.0f);
        setParam(processor, VocalDeEsserProcessor::PARAM_RATIO, 10.0f);

        std::vector<float> input, output;
        runTones(processor, 0.5f, 0.5f, input, output);

        const float lowIn = amplitudeAt(input, 200.0);
        const float lowOut = amplitudeAt(output, 200.0);
        const float highIn = amplitudeAt(input, 7000.0);
        const float highOut = amplitudeAt(output, 7000.0);

        const float lowChangeDb = juce::Decibels::gainToDecibels(lowOut / lowIn);
        const float highChangeDb = juce::Decibels::gainToDecibels(highOut / highIn);

        expect(std::abs(lowChangeDb) < 0.5f,
               "200 Hz should pass through the de-esser untouched, changed by "
               + juce::String(lowChangeDb, 2) + " dB");
        expect(highChangeDb < -12.0f,
               "7 kHz sibilance should be reduced by more than 12 dB, changed by "
               + juce::String(highChangeDb, 2) + " dB");
        expect(processor.getGainReduction() > 0.5f, "Meter should show the gain reduction");
    }

    void testBelowThresholdPassesUnchanged()
    {
        beginTest("Signal below the threshold passes unchanged");

        VocalDeEsserProcessor processor;
        processor.prepareToPlay(sampleRate, blockSize);
        setParam(processor, VocalDeEsserProcessor::PARAM_THRESHOLD, 0.0f);

        std::vector<float> input, output;
        runTones(processor, 0.3f, 0.3f, input, output);

        float maxDiff = 0.0f;
        for (size_t i = 0; i < input.size(); ++i)
            maxDiff = std::max(maxDiff, std::abs(input[i] - output[i]));

        expect(maxDiff < 1.0e-5f, "Band split should sum back to the input, max difference "
                                  + juce::String(maxDiff));
    }

    void testZeroMixPassesUnchanged()
    {
        beginTest("0% mix passes the signal unchanged");

        VocalDeEsserProcessor processor;
        processor.prepareToPlay(sampleRate, blockSize);
        setParam(processor, VocalDeEsserProcessor::PARAM_THRESHOLD, -60.0f);
        setParam(processor, VocalDeEsserProcessor::PARAM_RATIO, 10.0f);
        setParam(processor, VocalDeEsserProcessor::PARAM_MIX, 0.0f);

        std::vector<float> input, output;
        runTones(processor, 0.5f, 0.5f, input, output);

        float maxDiff = 0.0f;
        for (size_t i = 0; i < input.size(); ++i)
            maxDiff = std::max(maxDiff, std::abs(input[i] - output[i]));

        expect(maxDiff < 1.0e-5f, "Dry signal should be unchanged, max difference "
                                  + juce::String(maxDiff));
    }

    void testProcessBlockDoesNotAllocate()
    {
        beginTest("processBlock does not allocate, even when the band moves");

        VocalDeEsserProcessor processor;
        processor.prepareToPlay(sampleRate, blockSize);
        setParam(processor, VocalDeEsserProcessor::PARAM_THRESHOLD, -40.0f);

        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midi;
        int allocations = 0;

        for (int block = 0; block < 8; ++block)
        {
            // Parameter changes happen outside the counted region: the host
            // notification path is not part of the audio callback.
            setParam(processor, VocalDeEsserProcessor::PARAM_FREQUENCY, 5000.0f + 500.0f * (float) block);
            setParam(processor, VocalDeEsserProcessor::PARAM_BANDWIDTH, 1.0f + 0.25f * (float) block);
            fillTones(buffer, block * blockSize, 0.5f, 0.5f);

            ScopedAllocationCounter counter;
            processor.processBlock(buffer, midi);
            allocations += counter.count();
        }

        expectEquals(allocations, 0, "processBlock should not allocate on the audio thread");
    }
};

static VocalDeEsserPluginTests vocalDeEsserPluginTests;
