/**
 * ShimmerReverb Tests
 *
 * The shimmer is the reverb shifted up and/or down an octave. A sine played
 * into the plugin should come out with a tone an octave away from it, and
 * Feedback should send that shifted tone back into the reverb, where it is
 * shifted again. At the highest settings the loop must still die away.
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <algorithm>
#include <cmath>
#include <vector>

#include "../../plugins/ShimmerReverb/Source/PluginProcessor.h"

class ShimmerReverbTests : public juce::UnitTest
{
public:
    ShimmerReverbTests()
        : juce::UnitTest("ShimmerReverb", "Plugins")
    {
    }

    void runTest() override
    {
        testOctaveUpAddsAToneAnOctaveAbove();
        testOctaveDownAddsAToneAnOctaveBelow();
        testShimmerAtZeroAddsNoOctave();
        testFeedbackShiftsTheShimmerAgain();
        testMaximumFeedbackDiesAway();
    }

private:
    static constexpr double sampleRate = 48000.0;
    static constexpr int blockSize = 512;
    static constexpr float tone = 440.0f;

    enum PitchMode { up = 0, down = 1, both = 2 };

    struct Settings
    {
        float size = 0.5f, damping = 0.5f, shimmer = 1.0f, feedback = 0.0f, mix = 1.0f;
        int pitchMode = up;
    };

    static void set(ShimmerProcessor& processor, const char* id, float value)
    {
        auto* parameter = processor.getValueTreeState().getParameter(id);
        parameter->setValueNotifyingHost(parameter->convertTo0to1(value));
    }

    static void prepare(ShimmerProcessor& processor, const Settings& settings)
    {
        set(processor, "size", settings.size);
        set(processor, "damping", settings.damping);
        set(processor, "shimmer", settings.shimmer);
        set(processor, "pitchMode", static_cast<float>(settings.pitchMode));
        set(processor, "feedback", settings.feedback);
        set(processor, "mix", settings.mix);
        processor.setRateAndBufferSizeDetails(sampleRate, blockSize);
        processor.prepareToPlay(sampleRate, blockSize);
    }

    // Plays `input` (the same on both channels) through the plugin and
    // returns the left output.
    static std::vector<float> process(ShimmerProcessor& processor, const std::vector<float>& input)
    {
        std::vector<float> output(input.size());
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midi;

        for (size_t start = 0; start < input.size(); start += blockSize)
        {
            const int n = static_cast<int>(std::min(static_cast<size_t>(blockSize), input.size() - start));
            buffer.setSize(2, n, false, false, true);
            for (int channel = 0; channel < 2; ++channel)
                buffer.copyFrom(channel, 0, input.data() + start, n);

            processor.processBlock(buffer, midi);
            std::copy(buffer.getReadPointer(0), buffer.getReadPointer(0) + n, output.begin() + static_cast<long>(start));
        }
        return output;
    }

    static std::vector<float> sine(float frequency, double seconds)
    {
        std::vector<float> samples(static_cast<size_t>(seconds * sampleRate));
        for (size_t i = 0; i < samples.size(); ++i)
            samples[i] = 0.5f * std::sin(juce::MathConstants<float>::twoPi * frequency * static_cast<float>(i) / static_cast<float>(sampleRate));
        return samples;
    }

    // Amplitude of `frequency` in the last second of `samples` (Hann-windowed DFT bin).
    static double binAmplitude(const std::vector<float>& samples, double frequency)
    {
        const size_t length = static_cast<size_t>(sampleRate);
        const size_t start = samples.size() - length;
        double re = 0.0, im = 0.0;
        for (size_t i = 0; i < length; ++i)
        {
            const double window = 0.5 - 0.5 * std::cos(juce::MathConstants<double>::twoPi * static_cast<double>(i) / static_cast<double>(length));
            const double phase = juce::MathConstants<double>::twoPi * frequency * static_cast<double>(i) / sampleRate;
            re += window * samples[start + i] * std::cos(phase);
            im += window * samples[start + i] * std::sin(phase);
        }
        return 2.0 * std::sqrt(re * re + im * im) / (0.5 * static_cast<double>(length));
    }

    // Amplitude within 10% of `frequency`. A delay-line pitch shifter jumps
    // in phase from one grain to the next, which spreads a shifted tone a
    // few Hz either side of the exact octave, so a single bin would miss it.
    static float amplitudeAt(const std::vector<float>& samples, float frequency)
    {
        double power = 0.0;
        for (int f = static_cast<int>(0.9f * frequency); f <= static_cast<int>(1.1f * frequency); ++f)
            power += juce::square(binAmplitude(samples, f));
        return static_cast<float>(std::sqrt(power / 1.5)); // A Hann window spreads one tone over 1.5 bins of power
    }

    static std::vector<float> playTone(const Settings& settings)
    {
        ShimmerProcessor processor;
        prepare(processor, settings);
        return process(processor, sine(tone, 3.0));
    }

    static juce::String ratio(float a, float b)
    {
        return juce::String(juce::Decibels::gainToDecibels(a / b), 1) + " dB";
    }

    void testOctaveUpAddsAToneAnOctaveAbove()
    {
        beginTest("Octave Up adds a tone an octave above the input");

        const auto output = playTone({});
        const float fundamental = amplitudeAt(output, tone);
        const float octaveUp = amplitudeAt(output, 2.0f * tone);
        const float octaveDown = amplitudeAt(output, 0.5f * tone);

        expect(octaveUp > 0.25f * fundamental, "Octave above is " + ratio(octaveUp, fundamental));
        expect(octaveDown < 0.05f * fundamental, "Octave below is " + ratio(octaveDown, fundamental));
    }

    void testOctaveDownAddsAToneAnOctaveBelow()
    {
        beginTest("Octave Down adds a tone an octave below the input");

        Settings settings;
        settings.pitchMode = down;
        const auto output = playTone(settings);
        const float fundamental = amplitudeAt(output, tone);
        const float octaveUp = amplitudeAt(output, 2.0f * tone);
        const float octaveDown = amplitudeAt(output, 0.5f * tone);

        expect(octaveDown > 0.25f * fundamental, "Octave below is " + ratio(octaveDown, fundamental));
        expect(octaveUp < 0.05f * fundamental, "Octave above is " + ratio(octaveUp, fundamental));
    }

    void testShimmerAtZeroAddsNoOctave()
    {
        beginTest("With Shimmer at zero there is no octave, only the reverb");

        Settings settings;
        settings.shimmer = 0.0f;
        settings.pitchMode = both;
        settings.feedback = 0.85f;
        const auto output = playTone(settings);
        const float fundamental = amplitudeAt(output, tone);

        expect(fundamental > 0.05f, "The reverb is silent");
        expect(amplitudeAt(output, 2.0f * tone) < 0.01f * fundamental, "Octave above with no shimmer");
        expect(amplitudeAt(output, 0.5f * tone) < 0.01f * fundamental, "Octave below with no shimmer");
    }

    void testFeedbackShiftsTheShimmerAgain()
    {
        beginTest("Feedback sends the shimmer back into the reverb to be shifted again");

        // Two octaves up can only come from the octave-up tone going round
        // the loop a second time.
        Settings settings;
        const auto withoutFeedback = playTone(settings);
        settings.feedback = 0.85f;
        const auto withFeedback = playTone(settings);

        const float octave = amplitudeAt(withFeedback, 2.0f * tone);
        const float twoOctaves = amplitudeAt(withFeedback, 4.0f * tone);
        const float twoOctavesWithoutFeedback = amplitudeAt(withoutFeedback, 4.0f * tone);

        expect(twoOctaves > 0.03f * octave, "Two octaves up is " + ratio(twoOctaves, octave) + " against one octave");
        expect(twoOctaves > 10.0f * twoOctavesWithoutFeedback,
               "Feedback raised two octaves up by only " + ratio(twoOctaves, twoOctavesWithoutFeedback));
    }

    struct Peaks
    {
        float overall = 0.0f, lastSecond = 0.0f;
        bool finite = true;
    };

    // Half a second of loud noise, then 20 seconds of silence
    static Peaks playBurst(const Settings& settings)
    {
        ShimmerProcessor processor;
        prepare(processor, settings);

        std::vector<float> input(static_cast<size_t>(20.5 * sampleRate), 0.0f);
        juce::Random random(3);
        for (size_t i = 0; i < static_cast<size_t>(0.5 * sampleRate); ++i)
            input[i] = 2.0f * random.nextFloat() - 1.0f;

        const auto output = process(processor, input);
        Peaks peaks;
        for (size_t i = 0; i < output.size(); ++i)
        {
            peaks.finite = peaks.finite && std::isfinite(output[i]);
            peaks.overall = juce::jmax(peaks.overall, std::abs(output[i]));
            if (i + static_cast<size_t>(sampleRate) >= output.size())
                peaks.lastSecond = juce::jmax(peaks.lastSecond, std::abs(output[i]));
        }
        return peaks;
    }

    void testMaximumFeedbackDiesAway()
    {
        beginTest("At the highest settings the shimmer stays bounded and dies away");

        Settings settings;
        settings.size = 1.0f;
        settings.damping = 0.0f;
        settings.shimmer = 0.0f;
        const float reverbPeak = playBurst(settings).overall;

        settings.shimmer = 1.0f;
        settings.feedback = 0.85f;
        for (int mode : { up, down, both })
        {
            settings.pitchMode = mode;
            const auto peaks = playBurst(settings);

            const juce::String name = mode == up ? "Up" : mode == down ? "Down" : "Both";
            expect(peaks.finite, name + ": output is not finite");
            expect(peaks.overall < 1.5f * reverbPeak,
                   name + ": peak is " + ratio(peaks.overall, reverbPeak) + " above the reverb alone");
            expect(peaks.lastSecond < 0.01f, name + ": still at " + juce::String(peaks.lastSecond) + " after 20 s");
        }
    }
};

static ShimmerReverbTests shimmerReverbTests;
