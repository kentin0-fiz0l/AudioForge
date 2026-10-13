/**
 * VocalDoubler Plugin Tests
 *
 * Drives the processor through its public API (parameters + processBlock)
 * and checks the doubled voices:
 * - Each voice is pitch shifted by the amount Pitch Variation asks for
 * - The pitch shifter's delay wraps do not click
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <cmath>

#include "../../plugins/VocalDoubler/Source/PluginProcessor.h"

class VocalDoublerTests : public juce::UnitTest
{
public:
    VocalDoublerTests()
        : juce::UnitTest("VocalDoubler", "Plugins")
    {
    }

    void runTest() override
    {
        testVoicesArePitchShifted();
        testPitchShiftIsSmooth();
    }

private:
    static constexpr double kSampleRate = 44100.0;
    static constexpr int kBlockSize = 512;

    static void setParam(VocalDoublerProcessor& p, const char* id, float value)
    {
        auto* param = p.getAPVTS().getParameter(id);
        param->setValueNotifyingHost(param->convertTo0to1(value));
    }

    // Fully wet, no humanize randomness, no timing offset, filters wide open.
    static void setUpDryRun(VocalDoublerProcessor& p, int voices, float pitchCents)
    {
        setParam(p, VocalDoublerProcessor::PARAM_VOICES, static_cast<float>(voices));
        setParam(p, VocalDoublerProcessor::PARAM_WIDTH, 100.0f);
        setParam(p, VocalDoublerProcessor::PARAM_PITCH_VAR, pitchCents);
        setParam(p, VocalDoublerProcessor::PARAM_TIMING, 0.0f);
        setParam(p, VocalDoublerProcessor::PARAM_HUMANIZE, 0.0f);
        setParam(p, VocalDoublerProcessor::PARAM_HIGHPASS, 20.0f);
        setParam(p, VocalDoublerProcessor::PARAM_LOWPASS, 20000.0f);
        setParam(p, VocalDoublerProcessor::PARAM_MIX, 100.0f);
        setParam(p, VocalDoublerProcessor::PARAM_GAIN, 0.0f);
        p.prepareToPlay(kSampleRate, kBlockSize);
    }

    // Feeds a stereo sine and returns the stereo output.
    static juce::AudioBuffer<float> render(VocalDoublerProcessor& p, double freqHz, double seconds)
    {
        const int total = static_cast<int>(seconds * kSampleRate);
        juce::AudioBuffer<float> out(2, total);
        juce::AudioBuffer<float> block(2, kBlockSize);
        juce::MidiBuffer midi;

        for (int start = 0; start < total; start += kBlockSize)
        {
            const int n = juce::jmin(kBlockSize, total - start);
            block.setSize(2, n, false, false, true);
            for (int i = 0; i < n; ++i)
            {
                const float s = 0.5f * static_cast<float>(
                    std::sin(juce::MathConstants<double>::twoPi * freqHz * (start + i) / kSampleRate));
                block.setSample(0, i, s);
                block.setSample(1, i, s);
            }
            p.processBlock(block, midi);
            for (int ch = 0; ch < 2; ++ch)
                out.copyFrom(ch, start, block, ch, 0, n);
        }
        return out;
    }

    // Frequency from interpolated rising zero crossings, skipping the first `skip` samples.
    static double measureFrequency(const juce::AudioBuffer<float>& buf, int ch, int skip)
    {
        const float* d = buf.getReadPointer(ch);
        double first = -1.0, last = -1.0;
        int crossings = 0;
        for (int i = skip + 1; i < buf.getNumSamples(); ++i)
        {
            if (d[i - 1] < 0.0f && d[i] >= 0.0f)
            {
                const double t = (i - 1) + d[i - 1] / static_cast<double>(d[i - 1] - d[i]);
                if (first < 0.0) first = t;
                last = t;
                ++crossings;
            }
        }
        if (crossings < 2) return 0.0;
        return (crossings - 1) * kSampleRate / (last - first);
    }

    void testVoicesArePitchShifted()
    {
        beginTest("Voices are pitch shifted by the Pitch Variation amount");

        // Two voices at full width: voice 1 (+10 cents) is hard left,
        // voice 2 (-20 cents) is hard right.
        VocalDoublerProcessor processor;
        setUpDryRun(processor, 2, 20.0f);
        const auto out = render(processor, 500.0, 4.0);

        const int skip = static_cast<int>(0.25 * kSampleRate);
        const double left = measureFrequency(out, 0, skip);
        const double right = measureFrequency(out, 1, skip);
        const double expectedLeft = 500.0 * std::pow(2.0, 10.0 / 1200.0);   // ~502.9 Hz
        const double expectedRight = 500.0 * std::pow(2.0, -20.0 / 1200.0); // ~494.3 Hz

        expectWithinAbsoluteError(left, expectedLeft, 0.5,
                                  "Left voice should sound 10 cents sharp, measured " + juce::String(left));
        expectWithinAbsoluteError(right, expectedRight, 0.5,
                                  "Right voice should sound 20 cents flat, measured " + juce::String(right));
    }

    void testPitchShiftIsSmooth()
    {
        beginTest("Pitch shifter does not click when its delay wraps");

        // 440 Hz does not line up with the shifter's window, the worst case
        // for a wrap. 3 s covers more than one full wrap at 20 cents.
        VocalDoublerProcessor processor;
        setUpDryRun(processor, 1, 20.0f);
        const auto out = render(processor, 440.0, 3.0);

        const int skip = static_cast<int>(0.1 * kSampleRate);
        const float* d = out.getReadPointer(0);
        float maxStep = 0.0f;
        float peak = 0.0f;
        for (int i = skip + 1; i < out.getNumSamples(); ++i)
        {
            maxStep = juce::jmax(maxStep, std::abs(d[i] - d[i - 1]));
            peak = juce::jmax(peak, std::abs(d[i]));
        }

        // A 0.5 amplitude sine near 443 Hz moves at most ~0.032 per sample
        // (one centred voice is -3 dB, so less). Anything well above is a click.
        const float sineStep = 0.5f * juce::MathConstants<float>::twoPi * 445.0f / static_cast<float>(kSampleRate);
        expect(maxStep < 1.25f * sineStep,
               "Largest sample step " + juce::String(maxStep) + " should stay below " + juce::String(1.25f * sineStep));
        expect(peak > 0.1f, "Voice should be audible, peak " + juce::String(peak));
    }
};

static VocalDoublerTests vocalDoublerTests;
