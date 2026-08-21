/**
 * SimpleGain Plugin Tests
 *
 * Integration tests for the SimpleGain audio plugin:
 * - Plugin initialization
 * - Parameter management
 * - Audio processing correctness
 * - State save/restore
 * - Level metering
 */

#include <juce_audio_processors/juce_audio_processors.h>

// Include SimpleGain plugin headers
#include "../../plugins/SimpleGain/Source/PluginProcessor.h"

class SimpleGainPluginTests : public juce::UnitTest
{
public:
    SimpleGainPluginTests()
        : juce::UnitTest("SimpleGain Plugin", "Plugins")
    {
    }

    void runTest() override
    {
        testPluginInitialization();
        testParameterRange();
        testGainProcessing();
        testStateManagement();
        testLevelMetering();
        testSmoothParameterChanges();
    }

private:
    void testPluginInitialization()
    {
        beginTest("Plugin initialization");

        SimpleGainProcessor processor;

        // Check basic plugin properties
        expect(processor.getName().isNotEmpty(), "Plugin should have a name");
        expect(!processor.acceptsMidi(), "Should not accept MIDI");
        expect(!processor.producesMidi(), "Should not produce MIDI");
        expect(!processor.isMidiEffect(), "Should not be MIDI effect");
        expect(processor.getTailLengthSeconds() == 0.0, "Should have no tail");

        // Check bus configuration
        expect(processor.getBusCount(true) == 1, "Should have one input bus");
        expect(processor.getBusCount(false) == 1, "Should have one output bus");
    }

    void testParameterRange()
    {
        beginTest("Parameter range validation");

        SimpleGainProcessor processor;

        // Get the gain parameter
        const auto& params = processor.getParameters();
        expect(params.size() > 0, "Should have parameters");

        auto* gainParam = dynamic_cast<juce::AudioParameterFloat*>(params[0]);
        expect(gainParam != nullptr, "First parameter should be float");

        // Check parameter range
        auto range = gainParam->getNormalisableRange();
        expectEquals(range.start, -60.0f, "Min gain should be -60 dB");
        expectEquals(range.end, 12.0f, "Max gain should be +12 dB");
        expectEquals(gainParam->get(), 0.0f, "Default gain should be 0 dB");
    }

    void testGainProcessing()
    {
        beginTest("Gain processing accuracy");

        SimpleGainProcessor processor;

        // Prepare processor
        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        // Create test buffer with known signal
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        // Fill with sine wave at 0.5 amplitude
        for (int ch = 0; ch < 2; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < blockSize; ++i)
                data[i] = 0.5f * std::sin(2.0f * juce::MathConstants<float>::pi * i / blockSize);
        }

        // Test unity gain (0 dB)
        const auto& params = processor.getParameters();
        auto* gainParam = dynamic_cast<juce::AudioParameterFloat*>(params[0]);
        gainParam->setValueNotifyingHost(gainParam->convertTo0to1(0.0f));

        // Store original peak
        float originalPeak = buffer.getMagnitude(0, blockSize);

        // Process
        processor.processBlock(buffer, midiBuffer);

        // Check output is approximately the same (unity gain)
        float processedPeak = buffer.getMagnitude(0, blockSize);
        expectWithinAbsoluteError(processedPeak, originalPeak, 0.05f,
                                "Unity gain should preserve amplitude");

        // Test +6 dB gain (should approximately double)
        gainParam->setValueNotifyingHost(gainParam->convertTo0to1(6.0f));

        // Process multiple blocks to let smoothing settle
        for (int block = 0; block < 10; ++block)
        {
            buffer.clear();
            for (int ch = 0; ch < 2; ++ch)
            {
                auto* data = buffer.getWritePointer(ch);
                for (int i = 0; i < blockSize; ++i)
                    data[i] = 0.5f * std::sin(2.0f * juce::MathConstants<float>::pi * i / blockSize);
            }
            processor.processBlock(buffer, midiBuffer);
        }

        processedPeak = buffer.getMagnitude(0, blockSize);
        expectWithinAbsoluteError(processedPeak, originalPeak * 2.0f, 0.2f,
                                "+6 dB should approximately double amplitude");
    }

    void testStateManagement()
    {
        beginTest("State save and restore");

        SimpleGainProcessor processor1;

        // Set a specific gain value
        const auto& params = processor1.getParameters();
        auto* gainParam = dynamic_cast<juce::AudioParameterFloat*>(params[0]);
        gainParam->setValueNotifyingHost(gainParam->convertTo0to1(-12.0f));

        // Save state
        juce::MemoryBlock stateData;
        processor1.getStateInformation(stateData);

        expect(stateData.getSize() > 0, "State data should not be empty");

        // Create new processor and restore state
        SimpleGainProcessor processor2;
        processor2.setStateInformation(stateData.getData(),
                                      static_cast<int>(stateData.getSize()));

        // Check that parameter was restored
        const auto& params2 = processor2.getParameters();
        auto* gainParam2 = dynamic_cast<juce::AudioParameterFloat*>(params2[0]);
        expectWithinAbsoluteError(gainParam2->get(), -12.0f, 0.01f,
                                "State should be restored correctly");
    }

    void testLevelMetering()
    {
        beginTest("Level metering");

        SimpleGainProcessor processor;

        // Prepare processor
        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        // Create buffer with known peak
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        buffer.clear();
        buffer.setSample(0, 0, 0.8f); // Peak at 0.8
        buffer.setSample(1, 100, -0.8f);

        // Process
        processor.processBlock(buffer, midiBuffer);

        // Check level meter
        float level = processor.getCurrentLevel();
        expect(level > 0.0f, "Level meter should register signal");
        expect(level <= 1.0f, "Level meter should not exceed 1.0");
    }

    void testSmoothParameterChanges()
    {
        beginTest("Smooth parameter changes");

        SimpleGainProcessor processor;

        // Prepare processor
        const double sampleRate = 48000.0;
        const int blockSize = 64;
        processor.prepareToPlay(sampleRate, blockSize);

        // Create test buffer
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        // Process with initial gain
        const auto& params = processor.getParameters();
        auto* gainParam = dynamic_cast<juce::AudioParameterFloat*>(params[0]);
        gainParam->setValueNotifyingHost(gainParam->convertTo0to1(0.0f));

        buffer.clear();
        for (int ch = 0; ch < 2; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < blockSize; ++i)
                data[i] = 1.0f; // Constant signal
        }

        processor.processBlock(buffer, midiBuffer);

        // Change gain abruptly and process
        gainParam->setValueNotifyingHost(gainParam->convertTo0to1(12.0f));

        juce::AudioBuffer<float> buffer2(2, blockSize);
        for (int ch = 0; ch < 2; ++ch)
        {
            auto* data = buffer2.getWritePointer(ch);
            for (int i = 0; i < blockSize; ++i)
                data[i] = 1.0f;
        }

        processor.processBlock(buffer2, midiBuffer);

        // Check that output changes gradually (smoothing working)
        auto* data = buffer2.getReadPointer(0);
        float firstSample = std::abs(data[0]);
        float lastSample = std::abs(data[blockSize - 1]);

        expect(firstSample < lastSample,
             "Gain should increase gradually within block (smoothing)");
    }
};

static SimpleGainPluginTests simpleGainPluginTests;
