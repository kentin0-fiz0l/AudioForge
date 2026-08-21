/**
 * Plugin Integration Tests
 *
 * Tests for real-world plugin usage scenarios:
 * - Chaining multiple plugins
 * - Parameter automation
 * - Different sample rates
 * - Various buffer sizes
 * - Edge cases and stress tests
 */

#include <juce_audio_processors/juce_audio_processors.h>

// Include both plugin headers
#include "../../plugins/SimpleGain/Source/PluginProcessor.h"
#include "../../plugins/PanUtil/Source/PluginProcessor.h"

class PluginIntegrationTests : public juce::UnitTest
{
public:
    PluginIntegrationTests()
        : juce::UnitTest("Plugin Integration", "Integration")
    {
    }

    void runTest() override
    {
        testPluginChaining();
        testDifferentSampleRates();
        testVariableBufferSizes();
        testParameterAutomation();
        testZeroCrossing();
        testExtremeInputSignals();
    }

private:
    void testPluginChaining()
    {
        beginTest("Chaining plugins (SimpleGain -> PanUtil)");

        SimpleGainProcessor gainProcessor;
        PanUtilProcessor panProcessor;

        // Prepare both plugins
        const double sampleRate = 48000.0;
        const int blockSize = 512;

        gainProcessor.prepareToPlay(sampleRate, blockSize);

        panProcessor.prepareToPlay(sampleRate, blockSize);

        // Set gain to +6 dB
        const auto& gainParams = gainProcessor.getParameters();
        auto* gainParam = dynamic_cast<juce::AudioParameterFloat*>(gainParams[0]);
        gainParam->setValueNotifyingHost(gainParam->convertTo0to1(6.0f));

        // Set pan to hard left
        const auto& panParams = panProcessor.getParameters();
        auto* panParam = dynamic_cast<juce::AudioParameterFloat*>(panParams[0]);
        panParam->setValueNotifyingHost(panParam->convertTo0to1(-1.0f));

        // Create test signal
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        for (int i = 0; i < blockSize; ++i)
        {
            float value = 0.25f * std::sin(2.0f * juce::MathConstants<float>::pi * i / blockSize);
            buffer.setSample(0, i, value);
            buffer.setSample(1, i, value);
        }

        float originalPeak = buffer.getMagnitude(0, 0, blockSize);

        // Process through gain plugin
        for (int i = 0; i < 10; ++i)
            gainProcessor.processBlock(buffer, midiBuffer);

        // Process through pan plugin
        for (int i = 0; i < 10; ++i)
            panProcessor.processBlock(buffer, midiBuffer);

        // Check results
        float leftPeak = buffer.getMagnitude(0, 0, blockSize);
        float rightPeak = buffer.getMagnitude(1, 0, blockSize);

        // Should be amplified and panned left
        expect(leftPeak > originalPeak * 1.5f, "Left should be amplified and prominent");
        expect(rightPeak < originalPeak * 0.5f, "Right should be quiet after pan");
    }

    void testDifferentSampleRates()
    {
        beginTest("Different sample rates");

        std::vector<double> sampleRates = {44100.0, 48000.0, 96000.0, 192000.0};

        for (double sampleRate : sampleRates)
        {
            SimpleGainProcessor processor;

            const int blockSize = 512;
            processor.prepareToPlay(sampleRate, blockSize);

            // Create test buffer
            juce::AudioBuffer<float> buffer(2, blockSize);
            juce::MidiBuffer midiBuffer;

            for (int ch = 0; ch < 2; ++ch)
            {
                auto* data = buffer.getWritePointer(ch);
                for (int i = 0; i < blockSize; ++i)
                    data[i] = 0.5f;
            }

            // Process
            processor.processBlock(buffer, midiBuffer);

            // Should complete without errors at any sample rate
            expect(buffer.getMagnitude(0, 0, blockSize) > 0.0f,
                 "Processing should work at " + juce::String(sampleRate) + " Hz");
        }
    }

    void testVariableBufferSizes()
    {
        beginTest("Variable buffer sizes");

        std::vector<int> bufferSizes = {32, 64, 128, 256, 512, 1024, 2048};

        for (int blockSize : bufferSizes)
        {
            PanUtilProcessor processor;

            const double sampleRate = 48000.0;
            processor.prepareToPlay(sampleRate, blockSize);

            // Create test buffer
            juce::AudioBuffer<float> buffer(2, blockSize);
            juce::MidiBuffer midiBuffer;

            for (int i = 0; i < blockSize; ++i)
            {
                buffer.setSample(0, i, 0.5f);
                buffer.setSample(1, i, 0.5f);
            }

            // Process
            processor.processBlock(buffer, midiBuffer);

            // Should handle any buffer size
            expect(buffer.getMagnitude(0, 0, blockSize) > 0.0f,
                 "Processing should work with buffer size " + juce::String(blockSize));
        }
    }

    void testParameterAutomation()
    {
        beginTest("Parameter automation");

        SimpleGainProcessor processor;

        // Prepare processor
        const double sampleRate = 48000.0;
        const int blockSize = 64;
        processor.setRateAndBufferSizeDetails(sampleRate, blockSize);
        processor.prepareToPlay(sampleRate, blockSize);

        const auto& params = processor.getParameters();
        auto* gainParam = dynamic_cast<juce::AudioParameterFloat*>(params[0]);

        // Simulate parameter automation (rapid changes)
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        std::vector<float> automationValues = {0.0f, 6.0f, -6.0f, 0.0f, 12.0f, -12.0f};

        for (float automationValue : automationValues)
        {
            // Set parameter value
            gainParam->setValueNotifyingHost(gainParam->convertTo0to1(automationValue));

            // Fill buffer with constant signal
            buffer.clear();
            for (int ch = 0; ch < 2; ++ch)
            {
                auto* data = buffer.getWritePointer(ch);
                for (int i = 0; i < blockSize; ++i)
                    data[i] = 0.5f;
            }

            // Process
            processor.processBlock(buffer, midiBuffer);

            // Should not crash or produce NaN
            for (int ch = 0; ch < 2; ++ch)
            {
                for (int i = 0; i < blockSize; ++i)
                {
                    float sample = buffer.getSample(ch, i);
                    expect(!std::isnan(sample), "Automation should not produce NaN");
                    expect(!std::isinf(sample), "Automation should not produce Inf");
                }
            }
        }
    }

    void testZeroCrossing()
    {
        beginTest("Zero crossing detection (smoothing quality)");

        SimpleGainProcessor processor;

        const double sampleRate = 48000.0;
        const int blockSize = 1024;
        processor.setRateAndBufferSizeDetails(sampleRate, blockSize);
        processor.prepareToPlay(sampleRate, blockSize);

        const auto& params = processor.getParameters();
        auto* gainParam = dynamic_cast<juce::AudioParameterFloat*>(params[0]);

        // Start with 0 dB
        gainParam->setValueNotifyingHost(gainParam->convertTo0to1(0.0f));

        // Create sine wave
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        for (int ch = 0; ch < 2; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < blockSize; ++i)
                data[i] = std::sin(2.0f * juce::MathConstants<float>::pi * i / blockSize);
        }

        // Process
        processor.processBlock(buffer, midiBuffer);

        // Change gain dramatically mid-stream
        gainParam->setValueNotifyingHost(gainParam->convertTo0to1(-60.0f));

        juce::AudioBuffer<float> buffer2(2, blockSize);
        for (int ch = 0; ch < 2; ++ch)
        {
            auto* data = buffer2.getWritePointer(ch);
            for (int i = 0; i < blockSize; ++i)
                data[i] = std::sin(2.0f * juce::MathConstants<float>::pi * i / blockSize);
        }

        processor.processBlock(buffer2, midiBuffer);

        // Check for discontinuities (clicks) by looking at sample-to-sample differences
        auto* data = buffer2.getReadPointer(0);
        float maxDiff = 0.0f;

        for (int i = 1; i < blockSize; ++i)
        {
            float diff = std::abs(data[i] - data[i - 1]);
            maxDiff = juce::jmax(maxDiff, diff);
        }

        // Should not have huge jumps (clicks)
        expect(maxDiff < 0.5f, "Should not have discontinuities (max diff: " +
                             juce::String(maxDiff) + ")");
    }

    void testExtremeInputSignals()
    {
        beginTest("Extreme input signals");

        SimpleGainProcessor processor;

        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.setRateAndBufferSizeDetails(sampleRate, blockSize);
        processor.prepareToPlay(sampleRate, blockSize);

        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        // Test 1: DC offset
        buffer.clear();
        for (int ch = 0; ch < 2; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < blockSize; ++i)
                data[i] = 1.0f; // Maximum DC
        }

        processor.processBlock(buffer, midiBuffer);

        for (int ch = 0; ch < 2; ++ch)
        {
            for (int i = 0; i < blockSize; ++i)
            {
                float sample = buffer.getSample(ch, i);
                expect(!std::isnan(sample), "DC should not produce NaN");
            }
        }

        // Test 2: Zero signal
        buffer.clear();
        processor.processBlock(buffer, midiBuffer);

        for (int ch = 0; ch < 2; ++ch)
        {
            for (int i = 0; i < blockSize; ++i)
            {
                float sample = buffer.getSample(ch, i);
                expect(!std::isnan(sample), "Silence should not produce NaN");
            }
        }

        // Test 3: Very high frequency
        buffer.clear();
        for (int ch = 0; ch < 2; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < blockSize; ++i)
                data[i] = (i % 2 == 0) ? 1.0f : -1.0f; // Nyquist frequency
        }

        processor.processBlock(buffer, midiBuffer);

        for (int ch = 0; ch < 2; ++ch)
        {
            for (int i = 0; i < blockSize; ++i)
            {
                float sample = buffer.getSample(ch, i);
                expect(!std::isnan(sample), "High frequency should not produce NaN");
            }
        }
    }
};

static PluginIntegrationTests pluginIntegrationTests;
