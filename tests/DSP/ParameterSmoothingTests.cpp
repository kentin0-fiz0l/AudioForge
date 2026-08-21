/**
 * Parameter Smoothing Tests
 *
 * Tests for smooth parameter transitions to avoid clicks and pops:
 * - Smoothing initialization
 * - Ramp time accuracy
 * - Target value convergence
 * - No discontinuities in output
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class ParameterSmoothingTests : public juce::UnitTest
{
public:
    ParameterSmoothingTests()
        : juce::UnitTest("Parameter Smoothing", "DSP")
    {
    }

    void runTest() override
    {
        testSmoothingInitialization();
        testRampTime();
        testTargetConvergence();
        testNoDiscontinuities();
        testMultipleParameterChanges();
    }

private:
    void testSmoothingInitialization()
    {
        beginTest("Smoothing initialization");

        juce::SmoothedValue<float> smoother;
        const double sampleRate = 48000.0;
        const double rampTime = 0.05; // 50 ms

        smoother.reset(sampleRate, rampTime);
        smoother.setCurrentAndTargetValue(1.0f);

        // Initial value should be set immediately
        expectEquals(smoother.getCurrentValue(), 1.0f,
                   "Initial value should be set");
        expectEquals(smoother.getTargetValue(), 1.0f,
                   "Target should match current on init");

        // Should not be smoothing if current == target
        expect(!smoother.isSmoothing(), "Should not be smoothing when current == target");
    }

    void testRampTime()
    {
        beginTest("Ramp time accuracy");

        juce::SmoothedValue<float> smoother;
        const double sampleRate = 48000.0;
        const double rampTime = 0.05; // 50 ms

        smoother.reset(sampleRate, rampTime);
        smoother.setCurrentAndTargetValue(0.0f);
        smoother.setTargetValue(1.0f);

        // Calculate expected number of samples for ramp
        const int expectedSamples = static_cast<int>(sampleRate * rampTime);

        // Track when smoothing finishes
        int sampleCount = 0;
        while (smoother.isSmoothing() && sampleCount < expectedSamples * 2)
        {
            smoother.getNextValue();
            ++sampleCount;
        }

        // Should finish within expected time (with some tolerance)
        expect(sampleCount >= expectedSamples * 0.9,
             "Ramp finished too early: " + juce::String(sampleCount) + " vs " + juce::String(expectedSamples));
        expect(sampleCount <= expectedSamples * 1.1,
             "Ramp took too long: " + juce::String(sampleCount) + " vs " + juce::String(expectedSamples));
    }

    void testTargetConvergence()
    {
        beginTest("Target value convergence");

        juce::SmoothedValue<float> smoother;
        const double sampleRate = 48000.0;
        const double rampTime = 0.05;

        smoother.reset(sampleRate, rampTime);
        smoother.setCurrentAndTargetValue(0.0f);
        smoother.setTargetValue(1.0f);

        // Process until no longer smoothing
        float lastValue = 0.0f;
        while (smoother.isSmoothing())
        {
            lastValue = smoother.getNextValue();
        }

        // Final value should be very close to target
        expectWithinAbsoluteError(lastValue, 1.0f, 0.001f,
                                "Should converge to target value");
        expectWithinAbsoluteError(smoother.getCurrentValue(), 1.0f, 0.001f,
                                "Current value should equal target");
    }

    void testNoDiscontinuities()
    {
        beginTest("No discontinuities in smoothed values");

        juce::SmoothedValue<float> smoother;
        const double sampleRate = 48000.0;
        const double rampTime = 0.05;

        smoother.reset(sampleRate, rampTime);
        smoother.setCurrentAndTargetValue(0.0f);
        smoother.setTargetValue(1.0f);

        // Check that consecutive values don't jump
        float previousValue = smoother.getCurrentValue();
        const float maxStep = 0.05f; // Maximum allowed step between samples

        while (smoother.isSmoothing())
        {
            float currentValue = smoother.getNextValue();
            float step = std::abs(currentValue - previousValue);

            expect(step <= maxStep,
                 "Discontinuity detected: step = " + juce::String(step));

            previousValue = currentValue;
        }
    }

    void testMultipleParameterChanges()
    {
        beginTest("Multiple parameter changes");

        juce::SmoothedValue<float> smoother;
        const double sampleRate = 48000.0;
        const double rampTime = 0.05;

        smoother.reset(sampleRate, rampTime);
        smoother.setCurrentAndTargetValue(0.0f);

        // Change target multiple times during smoothing
        smoother.setTargetValue(1.0f);

        // Process partway through ramp
        for (int i = 0; i < 500; ++i)
            smoother.getNextValue();

        // Change target again before reaching previous target
        float midValue = smoother.getCurrentValue();
        smoother.setTargetValue(0.5f);

        expect(smoother.isSmoothing(), "Should still be smoothing after target change");

        // Continue smoothing to new target
        while (smoother.isSmoothing())
            smoother.getNextValue();

        // Should reach new target
        expectWithinAbsoluteError(smoother.getCurrentValue(), 0.5f, 0.001f,
                                "Should reach updated target");
    }
};

static ParameterSmoothingTests parameterSmoothingTests;
