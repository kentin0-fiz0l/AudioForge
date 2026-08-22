/**
 * Saturation Plugin Tests
 *
 * Integration tests for the Saturation audio plugin:
 * - Plugin initialization
 * - Parameter management (Drive, Mode, Tone, Mix, Oversampling)
 * - Saturation processing with different modes
 * - Tone shaping (low/high shelves)
 * - Oversampling functionality
 * - Mix control (dry/wet)
 * - State save/restore
 * - Level metering
 */

#include <juce_audio_processors/juce_audio_processors.h>

// Include Saturation plugin headers
#include "../../plugins/Saturation/Source/PluginProcessor.h"

class SaturationPluginTests : public juce::UnitTest
{
public:
    SaturationPluginTests()
        : juce::UnitTest("Saturation Plugin", "Plugins")
    {
    }

    void runTest() override
    {
        testPluginInitialization();
        testParameterRanges();
        testSaturationModes();
        testDriveControl();
        testToneControls();
        testMixControl();
        testOversamplingModes();
        testStateManagement();
        testLevelMetering();
    }

private:
    void testPluginInitialization()
    {
        beginTest("Plugin initialization");

        SaturationAudioProcessor processor;

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

    void testParameterRanges()
    {
        beginTest("Parameter range validation");

        SaturationAudioProcessor processor;

        // Check Drive parameter (0-100%)
        auto& driveParam = *processor.driveParam;
        auto driveRange = driveParam.getNormalisableRange();
        expectEquals(driveRange.start, 0.0f, "Min drive should be 0%");
        expectEquals(driveRange.end, 100.0f, "Max drive should be 100%");
        expectEquals(driveParam.get(), 0.0f, "Default drive should be 0%");

        // Check Mode parameter (5 modes)
        auto& modeParam = *processor.modeParam;
        expect(modeParam.choices.size() == 5, "Should have 5 saturation modes");
        expectEquals(modeParam.getIndex(), 0, "Default mode should be Tape");

        // Check Tone Low parameter (-12 to +12 dB)
        auto& toneLowParam = *processor.toneLowParam;
        auto toneLowRange = toneLowParam.getNormalisableRange();
        expectEquals(toneLowRange.start, -12.0f, "Min tone low should be -12 dB");
        expectEquals(toneLowRange.end, 12.0f, "Max tone low should be +12 dB");

        // Check Tone High parameter (-12 to +12 dB)
        auto& toneHighParam = *processor.toneHighParam;
        auto toneHighRange = toneHighParam.getNormalisableRange();
        expectEquals(toneHighRange.start, -12.0f, "Min tone high should be -12 dB");
        expectEquals(toneHighRange.end, 12.0f, "Max tone high should be +12 dB");

        // Check Mix parameter (0-100%)
        auto& mixParam = *processor.mixParam;
        auto mixRange = mixParam.getNormalisableRange();
        expectEquals(mixRange.start, 0.0f, "Min mix should be 0%");
        expectEquals(mixRange.end, 100.0f, "Max mix should be 100%");
        expectEquals(mixParam.get(), 100.0f, "Default mix should be 100%");

        // Check Oversampling parameter (3 modes)
        auto& oversamplingParam = *processor.oversamplingParam;
        expect(oversamplingParam.choices.size() == 3, "Should have 3 oversampling modes");
        expectEquals(oversamplingParam.getIndex(), 0, "Default should be Off");
    }

    void testSaturationModes()
    {
        beginTest("Saturation mode switching");

        SaturationAudioProcessor processor;

        // Prepare processor
        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        processor.driveParam->setValueNotifyingHost(0.5f); // 50% drive

        // Test each mode produces different output
        std::vector<float> modeOutputs;

        for (int mode = 0; mode < 5; ++mode)
        {
            processor.modeParam->setValueNotifyingHost(
                processor.modeParam->convertTo0to1(mode));

            buffer.clear();
            for (int ch = 0; ch < 2; ++ch)
            {
                auto* data = buffer.getWritePointer(ch);
                for (int i = 0; i < blockSize; ++i)
                    data[i] = 0.7f * std::sin(2.0f * juce::MathConstants<float>::pi * i / 100.0f);
            }

            processor.processBlock(buffer, midiBuffer);

            float peak = buffer.getMagnitude(0, blockSize);
            modeOutputs.push_back(peak);
        }

        // Verify modes produce distinct outputs
        expect(modeOutputs.size() == 5, "Should test all 5 modes");

        bool foundDifference = false;
        for (size_t i = 0; i < modeOutputs.size() - 1; ++i)
        {
            for (size_t j = i + 1; j < modeOutputs.size(); ++j)
            {
                if (std::abs(modeOutputs[i] - modeOutputs[j]) > 0.05f)
                {
                    foundDifference = true;
                    break;
                }
            }
            if (foundDifference) break;
        }

        expect(foundDifference, "Different modes should produce distinct outputs");
    }

    void testDriveControl()
    {
        beginTest("Drive control (0-10x multiplier)");

        SaturationAudioProcessor processor;

        // Prepare processor
        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        // Test zero drive
        processor.driveParam->setValueNotifyingHost(0.0f);

        buffer.clear();
        for (int ch = 0; ch < 2; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < blockSize; ++i)
                data[i] = 0.3f;
        }

        float inputPeak = buffer.getMagnitude(0, blockSize);
        processor.processBlock(buffer, midiBuffer);
        float zeroDrivePeak = buffer.getMagnitude(0, blockSize);

        // Zero drive should preserve signal mostly
        expectWithinAbsoluteError(zeroDrivePeak, inputPeak, 0.15f,
                                "Zero drive should preserve signal");

        // Test high drive
        processor.driveParam->setValueNotifyingHost(1.0f); // 100%

        buffer.clear();
        for (int ch = 0; ch < 2; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < blockSize; ++i)
                data[i] = 0.3f;
        }

        processor.processBlock(buffer, midiBuffer);
        float highDrivePeak = buffer.getMagnitude(0, blockSize);

        // High drive should increase saturation
        expect(highDrivePeak != zeroDrivePeak,
             "High drive should alter the signal");
    }

    void testToneControls()
    {
        beginTest("Tone shaping (low/high shelves)");

        SaturationAudioProcessor processor;

        // Prepare processor
        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::AudioBuffer<float> neutralBuffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        // Create signal with low frequency content
        buffer.clear();
        neutralBuffer.clear();
        for (int ch = 0; ch < 2; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            auto* neutralData = neutralBuffer.getWritePointer(ch);
            for (int i = 0; i < blockSize; ++i)
            {
                float sample = 0.5f * std::sin(2.0f * juce::MathConstants<float>::pi * i / 100.0f);
                data[i] = sample;
                neutralData[i] = sample;
            }
        }

        // Process with neutral tone (0 dB)
        processor.processBlock(neutralBuffer, midiBuffer);
        float neutralPeak = neutralBuffer.getMagnitude(0, blockSize);

        // Process with tone low boost (+6 dB)
        processor.toneLowParam->setValueNotifyingHost(
            processor.toneLowParam->convertTo0to1(6.0f));

        processor.processBlock(buffer, midiBuffer);
        float boostPeak = buffer.getMagnitude(0, blockSize);

        // Boosted signal should be higher than neutral
        expect(boostPeak > neutralPeak * 1.1f,
             "Tone boost should increase low frequency signal level");
    }

    void testMixControl()
    {
        beginTest("Mix control (dry/wet blend)");

        SaturationAudioProcessor processor;

        // Prepare processor
        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        processor.driveParam->setValueNotifyingHost(1.0f); // Full drive

        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        // Test 0% mix (fully dry)
        processor.mixParam->setValueNotifyingHost(0.0f);

        buffer.clear();
        for (int ch = 0; ch < 2; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < blockSize; ++i)
                data[i] = 0.5f * std::sin(2.0f * juce::MathConstants<float>::pi * i / 100.0f);
        }

        float inputPeak = buffer.getMagnitude(0, blockSize);
        processor.processBlock(buffer, midiBuffer);
        float dryPeak = buffer.getMagnitude(0, blockSize);

        // 0% mix should be close to original
        expectWithinAbsoluteError(dryPeak, inputPeak, 0.15f,
                                "0% mix should be mostly dry");
    }

    void testOversamplingModes()
    {
        beginTest("Oversampling mode switching");

        SaturationAudioProcessor processor;

        // Test each oversampling mode initializes
        for (int mode = 0; mode < 3; ++mode)
        {
            processor.oversamplingParam->setValueNotifyingHost(
                processor.oversamplingParam->convertTo0to1(mode));

            processor.prepareToPlay(48000.0, 512);

            juce::AudioBuffer<float> buffer(2, 512);
            juce::MidiBuffer midiBuffer;

            buffer.clear();
            for (int ch = 0; ch < 2; ++ch)
            {
                auto* data = buffer.getWritePointer(ch);
                for (int i = 0; i < 512; ++i)
                    data[i] = 0.5f;
            }

            // Should not crash
            processor.processBlock(buffer, midiBuffer);

            expect(std::isfinite(buffer.getMagnitude(0, 512)),
                 "Output should be finite with oversampling mode " + juce::String(mode));
        }
    }

    void testStateManagement()
    {
        beginTest("State save and restore");

        SaturationAudioProcessor processor1;

        // Set specific parameter values
        processor1.driveParam->setValueNotifyingHost(0.75f);
        processor1.modeParam->setValueNotifyingHost(
            processor1.modeParam->convertTo0to1(2)); // Transistor
        processor1.toneLowParam->setValueNotifyingHost(
            processor1.toneLowParam->convertTo0to1(3.0f));
        processor1.toneHighParam->setValueNotifyingHost(
            processor1.toneHighParam->convertTo0to1(-2.0f));
        processor1.mixParam->setValueNotifyingHost(0.5f);
        processor1.oversamplingParam->setValueNotifyingHost(
            processor1.oversamplingParam->convertTo0to1(1)); // 2x

        // Save state
        juce::MemoryBlock stateData;
        processor1.getStateInformation(stateData);

        expect(stateData.getSize() > 0, "State data should not be empty");

        // Create new processor and restore state
        SaturationAudioProcessor processor2;
        processor2.setStateInformation(stateData.getData(),
                                      static_cast<int>(stateData.getSize()));

        // Check that parameters were restored
        expectWithinAbsoluteError(processor2.driveParam->get(),
                                processor1.driveParam->get(), 0.01f,
                                "Drive should be restored");

        expectEquals(processor2.modeParam->getIndex(),
                    processor1.modeParam->getIndex(),
                    "Mode should be restored");

        expectWithinAbsoluteError(processor2.toneLowParam->get(),
                                processor1.toneLowParam->get(), 0.01f,
                                "Tone Low should be restored");

        expectWithinAbsoluteError(processor2.toneHighParam->get(),
                                processor1.toneHighParam->get(), 0.01f,
                                "Tone High should be restored");

        expectWithinAbsoluteError(processor2.mixParam->get(),
                                processor1.mixParam->get(), 0.01f,
                                "Mix should be restored");

        expectEquals(processor2.oversamplingParam->getIndex(),
                    processor1.oversamplingParam->getIndex(),
                    "Oversampling should be restored");
    }

    void testLevelMetering()
    {
        beginTest("Level metering");

        SaturationAudioProcessor processor;

        // Prepare processor
        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        // Create buffer with known peak
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        buffer.clear();
        buffer.setSample(0, 0, 0.6f);
        buffer.setSample(1, 100, -0.6f);

        // Process
        processor.processBlock(buffer, midiBuffer);

        // Check meters
        float inputLevel = processor.inputMeter.getLevelDb();
        float outputLevel = processor.outputMeter.getLevelDb();

        expect(inputLevel > -60.0f, "Input meter should register signal");
        expect(outputLevel > -60.0f, "Output meter should register signal");
    }
};

static SaturationPluginTests saturationPluginTests;
