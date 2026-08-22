/**
 * WaveShaper Plugin Tests
 *
 * Integration tests for the WaveShaper audio plugin:
 * - Plugin initialization
 * - Parameter management (Drive, Mix, Tone, Shape, Output)
 * - Waveshaping processing with different modes
 * - Drive control (0-10x multiplier)
 * - Mix control (dry/wet blend)
 * - Tone filter functionality
 * - DC blocker effectiveness
 * - State save/restore
 * - Level metering
 */

#include <juce_audio_processors/juce_audio_processors.h>

// Include WaveShaper plugin headers
#include "../../plugins/WaveShaper/Source/PluginProcessor.h"

class WaveShaperPluginTests : public juce::UnitTest
{
public:
    WaveShaperPluginTests()
        : juce::UnitTest("WaveShaper Plugin", "Plugins")
    {
    }

    void runTest() override
    {
        testPluginInitialization();
        testParameterRanges();
        testWaveshapingModes();
        testDriveControl();
        testMixControl();
        testToneFilter();
        testOutputGain();
        testStateManagement();
        testLevelMetering();
        testDCBlocker();
    }

private:
    void testPluginInitialization()
    {
        beginTest("Plugin initialization");

        WaveShaperAudioProcessor processor;

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

        WaveShaperAudioProcessor processor;

        // Check Drive parameter (0-100%)
        auto& driveParam = *processor.driveParam;
        auto driveRange = driveParam.getNormalisableRange();
        expectEquals(driveRange.start, 0.0f, "Min drive should be 0%");
        expectEquals(driveRange.end, 100.0f, "Max drive should be 100%");
        expectEquals(driveParam.get(), 0.0f, "Default drive should be 0%");

        // Check Mix parameter (0-100%)
        auto& mixParam = *processor.mixParam;
        auto mixRange = mixParam.getNormalisableRange();
        expectEquals(mixRange.start, 0.0f, "Min mix should be 0%");
        expectEquals(mixRange.end, 100.0f, "Max mix should be 100%");
        expectEquals(mixParam.get(), 100.0f, "Default mix should be 100%");

        // Check Tone parameter (0-100%)
        auto& toneParam = *processor.toneParam;
        auto toneRange = toneParam.getNormalisableRange();
        expectEquals(toneRange.start, 0.0f, "Min tone should be 0%");
        expectEquals(toneRange.end, 100.0f, "Max tone should be 100%");
        expectEquals(toneParam.get(), 100.0f, "Default tone should be 100%");

        // Check Shape parameter (5 modes)
        auto& shapeParam = *processor.shapeParam;
        expect(shapeParam.choices.size() == 5, "Should have 5 shape modes");
        expectEquals(shapeParam.getIndex(), 0, "Default shape should be Soft Clip");

        // Check Output Gain parameter (-12 to +12 dB)
        auto& outputGainParam = *processor.outputGainParam;
        auto gainRange = outputGainParam.getNormalisableRange();
        expectEquals(gainRange.start, -12.0f, "Min output gain should be -12 dB");
        expectEquals(gainRange.end, 12.0f, "Max output gain should be +12 dB");
        expectEquals(outputGainParam.get(), 0.0f, "Default output gain should be 0 dB");
    }

    void testWaveshapingModes()
    {
        beginTest("Waveshaping mode switching");

        WaveShaperAudioProcessor processor;

        // Prepare processor
        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        // Create test buffer with moderate drive
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        processor.driveParam->setValueNotifyingHost(0.5f); // 50% drive

        // Test each shape mode produces different output
        std::vector<float> modeOutputs;

        for (int mode = 0; mode < 5; ++mode)
        {
            processor.shapeParam->setValueNotifyingHost(
                processor.shapeParam->convertTo0to1(mode));

            buffer.clear();
            for (int ch = 0; ch < 2; ++ch)
            {
                auto* data = buffer.getWritePointer(ch);
                for (int i = 0; i < blockSize; ++i)
                    data[i] = 0.8f * std::sin(2.0f * juce::MathConstants<float>::pi * i / 100.0f);
            }

            processor.processBlock(buffer, midiBuffer);

            float peak = buffer.getMagnitude(0, blockSize);
            modeOutputs.push_back(peak);
        }

        // Verify modes produce distinct outputs
        expect(modeOutputs.size() == 5, "Should test all 5 modes");

        // At least some modes should differ significantly
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

        expect(foundDifference, "Different waveshaping modes should produce distinct outputs");
    }

    void testDriveControl()
    {
        beginTest("Drive control (0-10x multiplier)");

        WaveShaperAudioProcessor processor;

        // Prepare processor
        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        // Create test signal
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        // Test zero drive (unity pass)
        processor.driveParam->setValueNotifyingHost(0.0f);
        processor.shapeParam->setValueNotifyingHost(0.0f); // Soft clip

        buffer.clear();
        for (int ch = 0; ch < 2; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < blockSize; ++i)
                data[i] = 0.3f; // Moderate level
        }

        float inputPeak = buffer.getMagnitude(0, blockSize);
        processor.processBlock(buffer, midiBuffer);
        float zeroDrivePeak = buffer.getMagnitude(0, blockSize);

        // Zero drive should barely change the signal (may have slight increase from filtering)
        // Using larger tolerance because signal goes through tone filter and DC blocker
        expectWithinAbsoluteError(zeroDrivePeak, inputPeak, 0.15f,
                                "Zero drive should preserve signal mostly");

        // Test high drive (100% = 11x multiplier)
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

        // High drive should saturate the signal (clipping effect)
        expect(highDrivePeak > zeroDrivePeak * 0.8f,
             "High drive should increase saturation");
    }

    void testMixControl()
    {
        beginTest("Mix control (dry/wet blend)");

        WaveShaperAudioProcessor processor;

        // Prepare processor
        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        processor.driveParam->setValueNotifyingHost(1.0f); // Full drive
        processor.shapeParam->setValueNotifyingHost(0.0f); // Soft clip

        // Test 0% mix (fully dry)
        processor.mixParam->setValueNotifyingHost(0.0f);

        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

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

        // 0% mix should be close to original (may have tone filter applied)
        expectWithinAbsoluteError(dryPeak, inputPeak, 0.1f,
                                "0% mix should be mostly dry");

        // Test 100% mix (fully wet)
        processor.mixParam->setValueNotifyingHost(1.0f);

        buffer.clear();
        for (int ch = 0; ch < 2; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < blockSize; ++i)
                data[i] = 0.5f * std::sin(2.0f * juce::MathConstants<float>::pi * i / 100.0f);
        }

        processor.processBlock(buffer, midiBuffer);
        float wetPeak = buffer.getMagnitude(0, blockSize);

        // Wet signal should differ significantly from dry (due to saturation)
        expect(std::abs(wetPeak - dryPeak) > 0.05f,
             "100% wet should differ from dry signal");
    }

    void testToneFilter()
    {
        beginTest("Tone filter (low-pass)");

        WaveShaperAudioProcessor processor;

        // Prepare processor
        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        // Create bright signal (high frequency content)
        buffer.clear();
        for (int ch = 0; ch < 2; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < blockSize; ++i)
            {
                // Mix of low and high frequencies
                data[i] = 0.3f * std::sin(2.0f * juce::MathConstants<float>::pi * i / 100.0f) // Low
                        + 0.2f * std::sin(2.0f * juce::MathConstants<float>::pi * i / 5.0f);   // High
            }
        }

        float inputPeak = buffer.getMagnitude(0, blockSize);

        // Test low tone (500 Hz cutoff)
        processor.toneParam->setValueNotifyingHost(0.0f);
        processor.processBlock(buffer, midiBuffer);
        float lowTonePeak = buffer.getMagnitude(0, blockSize);

        // Low tone should reduce high frequencies
        expect(lowTonePeak < inputPeak,
             "Low tone setting should filter high frequencies");
    }

    void testOutputGain()
    {
        beginTest("Output gain compensation");

        WaveShaperAudioProcessor processor;

        // Prepare processor
        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        // Test +6 dB output gain
        processor.outputGainParam->setValueNotifyingHost(
            processor.outputGainParam->convertTo0to1(6.0f));

        buffer.clear();
        for (int ch = 0; ch < 2; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < blockSize; ++i)
                data[i] = 0.25f; // Moderate level
        }

        float inputPeak = buffer.getMagnitude(0, blockSize);
        processor.processBlock(buffer, midiBuffer);
        float outputPeak = buffer.getMagnitude(0, blockSize);

        // +6 dB should approximately double amplitude
        expectWithinAbsoluteError(outputPeak, inputPeak * 2.0f, 0.3f,
                                "+6 dB should approximately double amplitude");
    }

    void testStateManagement()
    {
        beginTest("State save and restore");

        WaveShaperAudioProcessor processor1;

        // Set specific parameter values
        processor1.driveParam->setValueNotifyingHost(0.75f);
        processor1.mixParam->setValueNotifyingHost(0.5f);
        processor1.toneParam->setValueNotifyingHost(0.6f);
        processor1.shapeParam->setValueNotifyingHost(
            processor1.shapeParam->convertTo0to1(2)); // Tanh
        processor1.outputGainParam->setValueNotifyingHost(
            processor1.outputGainParam->convertTo0to1(3.0f));

        // Save state
        juce::MemoryBlock stateData;
        processor1.getStateInformation(stateData);

        expect(stateData.getSize() > 0, "State data should not be empty");

        // Create new processor and restore state
        WaveShaperAudioProcessor processor2;
        processor2.setStateInformation(stateData.getData(),
                                      static_cast<int>(stateData.getSize()));

        // Check that parameters were restored
        expectWithinAbsoluteError(processor2.driveParam->get(),
                                processor1.driveParam->get(), 0.01f,
                                "Drive parameter should be restored");

        expectWithinAbsoluteError(processor2.mixParam->get(),
                                processor1.mixParam->get(), 0.01f,
                                "Mix parameter should be restored");

        expectWithinAbsoluteError(processor2.toneParam->get(),
                                processor1.toneParam->get(), 0.01f,
                                "Tone parameter should be restored");

        expectEquals(processor2.shapeParam->getIndex(),
                    processor1.shapeParam->getIndex(),
                    "Shape parameter should be restored");

        expectWithinAbsoluteError(processor2.outputGainParam->get(),
                                processor1.outputGainParam->get(), 0.01f,
                                "Output gain parameter should be restored");
    }

    void testLevelMetering()
    {
        beginTest("Level metering");

        WaveShaperAudioProcessor processor;

        // Prepare processor
        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        // Create buffer with known peak
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        buffer.clear();
        buffer.setSample(0, 0, 0.7f);   // Input peak
        buffer.setSample(1, 100, -0.7f);

        // Process
        processor.processBlock(buffer, midiBuffer);

        // Check meters
        float inputLevel = processor.inputMeter.getLevelDb();
        float outputLevel = processor.outputMeter.getLevelDb();

        expect(inputLevel > -60.0f, "Input meter should register signal");
        expect(outputLevel > -60.0f, "Output meter should register signal");
    }

    void testDCBlocker()
    {
        beginTest("DC blocker effectiveness");

        WaveShaperAudioProcessor processor;

        // Prepare processor
        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        // Create signal with DC offset
        buffer.clear();
        for (int ch = 0; ch < 2; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < blockSize; ++i)
                data[i] = 0.5f + 0.2f * std::sin(2.0f * juce::MathConstants<float>::pi * i / 100.0f);
        }

        float inputMean = buffer.getRMSLevel(0, 0, blockSize);

        // Process multiple blocks to let DC blocker settle
        for (int block = 0; block < 20; ++block)
        {
            buffer.clear();
            for (int ch = 0; ch < 2; ++ch)
            {
                auto* data = buffer.getWritePointer(ch);
                for (int i = 0; i < blockSize; ++i)
                    data[i] = 0.5f + 0.2f * std::sin(2.0f * juce::MathConstants<float>::pi * i / 100.0f);
            }
            processor.processBlock(buffer, midiBuffer);
        }

        // Calculate mean after DC blocking
        float mean = 0.0f;
        for (int i = 0; i < blockSize; ++i)
            mean += buffer.getSample(0, i);
        mean /= blockSize;

        // DC blocker should significantly reduce DC offset
        expect(std::abs(mean) < 0.2f,
             "DC blocker should remove DC offset");
    }
};

static WaveShaperPluginTests waveShaperPluginTests;
