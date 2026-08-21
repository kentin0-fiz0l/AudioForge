/**
 * PanUtil Plugin Tests
 *
 * Integration tests for the PanUtil stereo panning plugin:
 * - Plugin initialization
 * - Pan, width, and mode parameters
 * - Panning algorithm correctness
 * - Balance mode
 * - Stereo width processing
 * - State save/restore
 */

#include <juce_audio_processors/juce_audio_processors.h>

// Include PanUtil plugin headers
#include "../../plugins/PanUtil/Source/PluginProcessor.h"

class PanUtilPluginTests : public juce::UnitTest
{
public:
    PanUtilPluginTests()
        : juce::UnitTest("PanUtil Plugin", "Plugins")
    {
    }

    void runTest() override
    {
        testPluginInitialization();
        testParameterRanges();
        testPanModeProcessing();
        testBalanceModeProcessing();
        testWidthControl();
        testStateManagement();
        testLevelMetering();
    }

private:
    void testPluginInitialization()
    {
        beginTest("Plugin initialization");

        PanUtilProcessor processor;

        // Check basic plugin properties
        expect(processor.getName().isNotEmpty(), "Plugin should have a name");
        expect(!processor.acceptsMidi(), "Should not accept MIDI");
        expect(!processor.producesMidi(), "Should not produce MIDI");
        expect(processor.getTailLengthSeconds() == 0.0, "Should have no tail");

        // Check bus configuration (stereo)
        expect(processor.getBusCount(true) == 1, "Should have one input bus");
        expect(processor.getBusCount(false) == 1, "Should have one output bus");
    }

    void testParameterRanges()
    {
        beginTest("Parameter range validation");

        PanUtilProcessor processor;
        const auto& params = processor.getParameters();

        expect(params.size() == 3, "Should have 3 parameters");

        // Check pan parameter
        auto* panParam = dynamic_cast<juce::AudioParameterFloat*>(params[0]);
        expect(panParam != nullptr, "Pan should be float parameter");

        auto panRange = panParam->getNormalisableRange();
        expectEquals(panRange.start, -1.0f, "Pan min should be -1.0");
        expectEquals(panRange.end, 1.0f, "Pan max should be +1.0");
        expectEquals(panParam->get(), 0.0f, "Default pan should be center");

        // Check width parameter
        auto* widthParam = dynamic_cast<juce::AudioParameterFloat*>(params[1]);
        expect(widthParam != nullptr, "Width should be float parameter");

        auto widthRange = widthParam->getNormalisableRange();
        expectEquals(widthRange.start, 0.0f, "Width min should be 0.0");
        expectEquals(widthRange.end, 2.0f, "Width max should be 2.0");
        expectEquals(widthParam->get(), 1.0f, "Default width should be 1.0");

        // Check mode parameter
        auto* modeParam = dynamic_cast<juce::AudioParameterChoice*>(params[2]);
        expect(modeParam != nullptr, "Mode should be choice parameter");
        expectEquals(modeParam->getIndex(), 0, "Default mode should be Pan");
    }

    void testPanModeProcessing()
    {
        beginTest("Pan mode processing");

        PanUtilProcessor processor;

        // Prepare processor
        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        const auto& params = processor.getParameters();
        auto* panParam = dynamic_cast<juce::AudioParameterFloat*>(params[0]);
        auto* modeParam = dynamic_cast<juce::AudioParameterChoice*>(params[2]);

        // Set to pan mode
        modeParam->setValueNotifyingHost(0.0f);

        // Create mono test signal
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        for (int i = 0; i < blockSize; ++i)
        {
            float value = 0.5f * std::sin(2.0f * juce::MathConstants<float>::pi * i / blockSize);
            buffer.setSample(0, i, value);
            buffer.setSample(1, i, value);
        }

        // Test hard left pan
        panParam->setValueNotifyingHost(panParam->convertTo0to1(-1.0f));

        // Process multiple blocks to let smoothing settle
        for (int i = 0; i < 10; ++i)
        {
            buffer.clear();
            for (int j = 0; j < blockSize; ++j)
            {
                float value = 0.5f * std::sin(2.0f * juce::MathConstants<float>::pi * j / blockSize);
                buffer.setSample(0, j, value);
                buffer.setSample(1, j, value);
            }
            processor.processBlock(buffer, midiBuffer);
        }

        float leftPeak = buffer.getMagnitude(0, 0, blockSize);
        float rightPeak = buffer.getMagnitude(1, 0, blockSize);

        expect(leftPeak > 0.4f, "Left channel should be loud when panned left");
        expect(rightPeak < 0.1f, "Right channel should be quiet when panned left");

        // Test center pan
        panParam->setValueNotifyingHost(panParam->convertTo0to1(0.0f));

        for (int i = 0; i < 10; ++i)
        {
            buffer.clear();
            for (int j = 0; j < blockSize; ++j)
            {
                float value = 0.5f * std::sin(2.0f * juce::MathConstants<float>::pi * j / blockSize);
                buffer.setSample(0, j, value);
                buffer.setSample(1, j, value);
            }
            processor.processBlock(buffer, midiBuffer);
        }

        leftPeak = buffer.getMagnitude(0, 0, blockSize);
        rightPeak = buffer.getMagnitude(1, 0, blockSize);

        // At center, both channels should be similar (~-3 dB each)
        // Using slightly larger tolerance to account for parameter smoothing iterations
        expectWithinAbsoluteError(leftPeak, rightPeak, 0.1f,
                                "Center pan should have balanced L/R");
    }

    void testBalanceModeProcessing()
    {
        beginTest("Balance mode processing");

        PanUtilProcessor processor;

        // Prepare processor
        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        const auto& params = processor.getParameters();
        auto* panParam = dynamic_cast<juce::AudioParameterFloat*>(params[0]);
        auto* modeParam = dynamic_cast<juce::AudioParameterChoice*>(params[2]);

        // Set to balance mode
        modeParam->setValueNotifyingHost(1.0f);

        // Create stereo test signal
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        buffer.clear();
        for (int i = 0; i < blockSize; ++i)
        {
            buffer.setSample(0, i, 0.8f); // Left
            buffer.setSample(1, i, 0.6f); // Right
        }

        // Test balance left (should attenuate right)
        panParam->setValueNotifyingHost(panParam->convertTo0to1(-0.5f));

        // Process multiple blocks to let smoothing settle
        for (int i = 0; i < 10; ++i)
            processor.processBlock(buffer, midiBuffer);

        float leftValue = std::abs(buffer.getSample(0, blockSize - 1));
        float rightValue = std::abs(buffer.getSample(1, blockSize - 1));

        expect(leftValue > 0.7f, "Left should be mostly unchanged in balance left");
        expect(rightValue < 0.4f, "Right should be attenuated in balance left");
    }

    void testWidthControl()
    {
        beginTest("Stereo width control");

        PanUtilProcessor processor;

        // Prepare processor
        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        const auto& params = processor.getParameters();
        auto* widthParam = dynamic_cast<juce::AudioParameterFloat*>(params[1]);

        // Create stereo signal with phase difference
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        for (int i = 0; i < blockSize; ++i)
        {
            buffer.setSample(0, i, 1.0f);  // Left positive
            buffer.setSample(1, i, -1.0f); // Right negative (max stereo)
        }

        // Test width = 0 (mono)
        widthParam->setValueNotifyingHost(widthParam->convertTo0to1(0.0f));

        // Process multiple blocks to let smoothing settle
        for (int i = 0; i < 10; ++i)
        {
            buffer.clear();
            for (int j = 0; j < blockSize; ++j)
            {
                buffer.setSample(0, j, 1.0f);  // Left positive
                buffer.setSample(1, j, -1.0f); // Right negative (max stereo)
            }
            processor.processBlock(buffer, midiBuffer);
        }

        float leftFinal = buffer.getSample(0, blockSize - 1);
        float rightFinal = buffer.getSample(1, blockSize - 1);

        // Using larger tolerance to account for parameter smoothing iterations
        expectWithinAbsoluteError(leftFinal, rightFinal, 0.2f,
                                "Width 0 should produce mono (L ≈ R)");

        // Test width = 2.0 (wide)
        widthParam->setValueNotifyingHost(widthParam->convertTo0to1(2.0f));

        for (int i = 0; i < 10; ++i)
        {
            buffer.clear();
            for (int j = 0; j < blockSize; ++j)
            {
                buffer.setSample(0, j, 1.0f);
                buffer.setSample(1, j, -1.0f);
            }
            processor.processBlock(buffer, midiBuffer);
        }

        leftFinal = buffer.getSample(0, blockSize - 1);
        rightFinal = buffer.getSample(1, blockSize - 1);

        float stereoSeparation = std::abs(leftFinal - rightFinal);
        expect(stereoSeparation > 2.0f,
             "Width 2.0 should enhance stereo separation");
    }

    void testStateManagement()
    {
        beginTest("State save and restore");

        PanUtilProcessor processor1;

        // Set specific parameter values
        const auto& params = processor1.getParameters();
        auto* panParam = dynamic_cast<juce::AudioParameterFloat*>(params[0]);
        auto* widthParam = dynamic_cast<juce::AudioParameterFloat*>(params[1]);
        auto* modeParam = dynamic_cast<juce::AudioParameterChoice*>(params[2]);

        panParam->setValueNotifyingHost(panParam->convertTo0to1(0.75f));
        widthParam->setValueNotifyingHost(widthParam->convertTo0to1(1.5f));
        modeParam->setValueNotifyingHost(1.0f); // Balance mode

        // Save state
        juce::MemoryBlock stateData;
        processor1.getStateInformation(stateData);

        expect(stateData.getSize() > 0, "State data should not be empty");

        // Create new processor and restore state
        PanUtilProcessor processor2;
        processor2.setStateInformation(stateData.getData(),
                                      static_cast<int>(stateData.getSize()));

        // Check that parameters were restored
        const auto& params2 = processor2.getParameters();
        auto* panParam2 = dynamic_cast<juce::AudioParameterFloat*>(params2[0]);
        auto* widthParam2 = dynamic_cast<juce::AudioParameterFloat*>(params2[1]);
        auto* modeParam2 = dynamic_cast<juce::AudioParameterChoice*>(params2[2]);

        expectWithinAbsoluteError(panParam2->get(), 0.75f, 0.01f, "Pan should be restored");
        expectWithinAbsoluteError(widthParam2->get(), 1.5f, 0.01f, "Width should be restored");
        expectEquals(modeParam2->getIndex(), 1, "Mode should be restored");
    }

    void testLevelMetering()
    {
        beginTest("Level metering (L/R independent)");

        PanUtilProcessor processor;

        // Prepare processor
        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        // Create buffer with different L/R levels
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        buffer.clear();
        buffer.setSample(0, 0, 0.9f);  // Left peak
        buffer.setSample(1, 100, 0.5f); // Right peak

        // Process
        processor.processBlock(buffer, midiBuffer);

        // Check level meters
        float leftLevel = processor.getLeftLevel();
        float rightLevel = processor.getRightLevel();

        expect(leftLevel > 0.0f, "Left level meter should register signal");
        expect(rightLevel > 0.0f, "Right level meter should register signal");
        expect(leftLevel != rightLevel, "L/R meters should be independent");
    }
};

static PanUtilPluginTests panUtilPluginTests;
