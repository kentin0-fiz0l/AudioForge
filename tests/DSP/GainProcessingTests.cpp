/**
 * Gain Processing DSP Tests
 *
 * Tests for gain/volume calculations and processing:
 * - Decibel to linear conversion accuracy
 * - Unity gain (0 dB)
 * - Extreme values (-60 dB, +12 dB)
 * - Signal multiplication correctness
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class GainProcessingTests : public juce::UnitTest
{
public:
    GainProcessingTests()
        : juce::UnitTest("Gain Processing", "DSP")
    {
    }

    void runTest() override
    {
        testDecibelConversion();
        testUnityGain();
        testExtremeGainValues();
        testGainApplication();
        testMultiChannelGain();
    }

private:
    void testDecibelConversion()
    {
        beginTest("Decibel to linear conversion");

        // Test unity gain (0 dB = 1.0 linear)
        float unityGain = juce::Decibels::decibelsToGain(0.0f);
        expectWithinAbsoluteError(unityGain, 1.0f, 0.0001f, "0 dB should equal 1.0 linear");

        // Test -6 dB ≈ 0.5 linear (half amplitude)
        float halfGain = juce::Decibels::decibelsToGain(-6.0f);
        expectWithinAbsoluteError(halfGain, 0.501187f, 0.0001f, "-6 dB should be ~0.5");

        // Test +6 dB ≈ 2.0 linear (double amplitude)
        float doubleGain = juce::Decibels::decibelsToGain(6.0f);
        expectWithinAbsoluteError(doubleGain, 1.995262f, 0.0001f, "+6 dB should be ~2.0");

        // Test -60 dB (very quiet, near silence)
        float quietGain = juce::Decibels::decibelsToGain(-60.0f);
        expect(quietGain < 0.002f, "-60 dB should be very quiet");

        // Test +12 dB (maximum boost in SimpleGain)
        float maxGain = juce::Decibels::decibelsToGain(12.0f);
        expectWithinAbsoluteError(maxGain, 3.981f, 0.01f, "+12 dB should be ~4x amplitude");
    }

    void testUnityGain()
    {
        beginTest("Unity gain processing");

        // Create test buffer with known values
        juce::AudioBuffer<float> buffer(2, 512);

        for (int ch = 0; ch < 2; ++ch)
        {
            auto* channelData = buffer.getWritePointer(ch);
            for (int i = 0; i < 512; ++i)
                channelData[i] = std::sin(2.0f * juce::MathConstants<float>::pi * i / 512.0f);
        }

        // Store original values
        juce::AudioBuffer<float> original(2, 512);
        for (int ch = 0; ch < 2; ++ch)
            original.copyFrom(ch, 0, buffer, ch, 0, 512);

        // Apply unity gain (1.0)
        float gain = 1.0f;
        for (int ch = 0; ch < 2; ++ch)
        {
            auto* channelData = buffer.getWritePointer(ch);
            for (int i = 0; i < 512; ++i)
                channelData[i] *= gain;
        }

        // Verify samples are unchanged
        for (int ch = 0; ch < 2; ++ch)
        {
            for (int i = 0; i < 512; ++i)
            {
                expectEquals(buffer.getSample(ch, i), original.getSample(ch, i),
                           "Unity gain should not change samples");
            }
        }
    }

    void testExtremeGainValues()
    {
        beginTest("Extreme gain values");

        // Test very low gain (should approach zero)
        float veryLowGain = juce::Decibels::decibelsToGain(-60.0f);
        float input = 1.0f;
        float output = input * veryLowGain;
        expect(output < 0.01f, "Output should be very quiet at -60 dB");

        // Test high gain (should amplify significantly)
        float highGain = juce::Decibels::decibelsToGain(12.0f);
        output = input * highGain;
        expect(output > 3.5f && output < 4.5f, "Output should be ~4x at +12 dB");

        // Test zero gain (silence)
        float zeroGain = juce::Decibels::decibelsToGain(-std::numeric_limits<float>::infinity());
        output = input * zeroGain;
        expectEquals(output, 0.0f, "Infinite negative dB should be zero");
    }

    void testGainApplication()
    {
        beginTest("Gain application accuracy");

        // Create sine wave test signal
        juce::AudioBuffer<float> buffer(1, 1024);
        auto* data = buffer.getWritePointer(0);

        // Generate 1 kHz sine at 0.5 amplitude
        const float amplitude = 0.5f;
        const float frequency = 1000.0f;
        const float sampleRate = 48000.0f;

        for (int i = 0; i < 1024; ++i)
        {
            data[i] = amplitude * std::sin(2.0f * juce::MathConstants<float>::pi *
                                          frequency * i / sampleRate);
        }

        // Apply +6 dB gain (should double amplitude)
        float gain = juce::Decibels::decibelsToGain(6.0f);
        for (int i = 0; i < 1024; ++i)
            data[i] *= gain;

        // Find peak value
        float peak = buffer.getMagnitude(0, 1024);

        // Peak should be approximately double the original amplitude
        expectWithinAbsoluteError(peak, amplitude * 2.0f, 0.05f,
                                "Peak should double with +6 dB gain");
    }

    void testMultiChannelGain()
    {
        beginTest("Multi-channel gain processing");

        // Create 4-channel test buffer
        juce::AudioBuffer<float> buffer(4, 256);

        // Fill with different patterns per channel
        for (int ch = 0; ch < 4; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < 256; ++i)
                data[i] = (ch + 1) * 0.1f * std::sin(i * 0.1f);
        }

        // Apply gain to all channels
        float gain = 2.0f;
        for (int ch = 0; ch < 4; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < 256; ++i)
                data[i] *= gain;
        }

        // Verify each channel's peak is scaled correctly
        for (int ch = 0; ch < 4; ++ch)
        {
            float expectedPeak = (ch + 1) * 0.1f * gain;
            float actualPeak = buffer.getMagnitude(ch, 0, 256);
            expectWithinAbsoluteError(actualPeak, expectedPeak, 0.05f,
                                    "Channel " + juce::String(ch) + " gain incorrect");
        }
    }
};

static GainProcessingTests gainProcessingTests;
