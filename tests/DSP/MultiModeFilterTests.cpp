/**
 * MultiModeFilter Tests
 *
 * Tests for the shared state-variable filter used by the synth voices:
 * - Stability across the whole cutoff, resonance and sample-rate range
 * - Unity gain in the passband
 * - Attenuation in the stopband for each filter type
 * - Resonant gain at the cutoff frequency
 * - State reset
 */

#include <juce_core/juce_core.h>
#include <cmath>

#include "../../shared/synth/MultiModeFilter.h"

class MultiModeFilterTests : public juce::UnitTest
{
public:
    MultiModeFilterTests()
        : juce::UnitTest("MultiModeFilter", "DSP")
    {
    }

    void runTest() override
    {
        testWideOpenLowPassIsTransparent();
        testStableAcrossParameterRange();
        testLowPassAttenuatesAboveCutoff();
        testHighPassAttenuatesBelowCutoff();
        testResonanceBoostsAtCutoff();
        testReset();
    }

private:
    using FilterType = MultiModeFilter::FilterType;

    // Runs a sine through the filter and returns the output peak once the
    // filter has settled (the first half of the run is discarded).
    static float steadyStatePeak(MultiModeFilter& filter, float frequency, float amplitude,
                                 double sampleRate, int numSamples = 48000)
    {
        const float twoPi = juce::MathConstants<float>::twoPi;
        float peak = 0.0f;

        for (int i = 0; i < numSamples; ++i)
        {
            const float input = amplitude * std::sin(twoPi * frequency * static_cast<float>(i)
                                                     / static_cast<float>(sampleRate));
            const float output = filter.processSample(input);

            if (i >= numSamples / 2)
                peak = std::max(peak, std::abs(output));
        }

        return peak;
    }

    void testWideOpenLowPassIsTransparent()
    {
        beginTest("Wide-open low-pass passes audio at unity gain");

        // BasicSynth's default: low-pass at 20 kHz with no resonance. A 1 kHz
        // tone is far inside the passband and should come out unchanged.
        for (double sampleRate : { 44100.0, 48000.0, 96000.0 })
        {
            MultiModeFilter filter;
            filter.setFilterType(FilterType::LowPass);
            filter.setParameters(20000.0f, 0.707f, sampleRate);

            const float peak = steadyStatePeak(filter, 1000.0f, 0.5f, sampleRate);

            expectWithinAbsoluteError(peak, 0.5f, 0.02f,
                                      "1 kHz should pass unchanged at " + juce::String(sampleRate) + " Hz");
        }
    }

    void testStableAcrossParameterRange()
    {
        beginTest("Stable at every cutoff, resonance and sample rate");

        const float amplitude = 0.25f;
        bool allFinite = true;
        bool allBounded = true;
        juce::String firstProblem;

        for (double sampleRate : { 44100.0, 48000.0, 96000.0 })
        {
            for (float cutoff : { 20.0f, 200.0f, 2000.0f, 8000.0f, 12000.0f, 20000.0f })
            {
                for (float resonance : { 0.5f, 0.707f, 2.0f, 10.0f })
                {
                    for (auto type : { FilterType::LowPass, FilterType::HighPass,
                                       FilterType::BandPass, FilterType::Notch })
                    {
                        MultiModeFilter filter;
                        filter.setFilterType(type);
                        filter.setParameters(cutoff, resonance, sampleRate);

                        // Deterministic noise: broadband, so it excites the
                        // filter at whatever frequency it resonates
                        juce::Random random(1234);
                        float peak = 0.0f;
                        bool finite = true;

                        for (int i = 0; i < 24000; ++i)
                        {
                            const float input = amplitude * (2.0f * random.nextFloat() - 1.0f);
                            const float output = filter.processSample(input);

                            finite = finite && std::isfinite(output);
                            peak = std::max(peak, std::abs(output));
                        }

                        // A resonant filter can ring above its input level, but
                        // only in proportion to its Q
                        const float bound = amplitude * 4.0f * std::max(1.0f, resonance);

                        if ((! finite || peak >= bound) && firstProblem.isEmpty())
                        {
                            firstProblem = "type " + juce::String(static_cast<int>(type))
                                           + ", cutoff " + juce::String(cutoff)
                                           + ", Q " + juce::String(resonance)
                                           + ", " + juce::String(sampleRate) + " Hz: peak "
                                           + juce::String(peak);
                        }

                        allFinite = allFinite && finite;
                        allBounded = allBounded && peak < bound;
                    }
                }
            }
        }

        expect(allFinite, "Output should always be finite. First problem: " + firstProblem);
        expect(allBounded, "Output should stay bounded. First problem: " + firstProblem);
    }

    void testLowPassAttenuatesAboveCutoff()
    {
        beginTest("Low-pass attenuates above the cutoff");

        MultiModeFilter filter;
        filter.setFilterType(FilterType::LowPass);
        filter.setParameters(500.0f, 0.707f, 48000.0);

        // 8 kHz is four octaves above a 500 Hz cutoff: about -48 dB at 12 dB/octave
        const float peak = steadyStatePeak(filter, 8000.0f, 0.5f, 48000.0);

        expect(peak < 0.5f * 0.02f, "8 kHz should be heavily attenuated by a 500 Hz low-pass");
    }

    void testHighPassAttenuatesBelowCutoff()
    {
        beginTest("High-pass attenuates below the cutoff");

        MultiModeFilter filter;
        filter.setFilterType(FilterType::HighPass);
        filter.setParameters(5000.0f, 0.707f, 48000.0);

        const float low = steadyStatePeak(filter, 300.0f, 0.5f, 48000.0);
        filter.reset();
        const float high = steadyStatePeak(filter, 15000.0f, 0.5f, 48000.0);

        expect(low < 0.5f * 0.02f, "300 Hz should be heavily attenuated by a 5 kHz high-pass");
        expectWithinAbsoluteError(high, 0.5f, 0.05f, "15 kHz should pass a 5 kHz high-pass");
    }

    void testResonanceBoostsAtCutoff()
    {
        beginTest("Resonance boosts the cutoff frequency by Q");

        const float resonance = 4.0f;

        for (auto type : { FilterType::LowPass, FilterType::BandPass })
        {
            MultiModeFilter filter;
            filter.setFilterType(type);
            filter.setParameters(1000.0f, resonance, 48000.0);

            const float peak = steadyStatePeak(filter, 1000.0f, 0.1f, 48000.0);

            expectWithinAbsoluteError(peak, 0.1f * resonance, 0.1f * resonance * 0.1f,
                                      "Gain at the cutoff should equal Q");
        }
    }

    void testReset()
    {
        beginTest("Reset clears the filter state");

        MultiModeFilter filter;
        filter.setFilterType(FilterType::LowPass);
        filter.setParameters(1000.0f, 5.0f, 48000.0);

        for (int i = 0; i < 1000; ++i)
            filter.processSample(i % 2 == 0 ? 1.0f : -1.0f);

        filter.reset();

        expectEquals(filter.processSample(0.0f), 0.0f, "Silence in should give silence out after reset");
    }
};

static MultiModeFilterTests multiModeFilterTests;
