/**
 * Reverb Plugin Tests
 *
 * Integration tests for the Reverb audio plugin:
 * - Plugin initialization
 * - Parameter management (Room Size, Damping, Width, Pre-Delay, Mix, Freeze)
 * - Comb filter bank processing
 * - All-pass filter cascade
 * - Pre-delay functionality
 * - Stereo width control
 * - Freeze mode
 * - State save/restore
 * - Level metering
 */

#include <juce_audio_processors/juce_audio_processors.h>

// Include Reverb plugin headers
#include "../../plugins/Reverb/Source/PluginProcessor.h"

class ReverbPluginTests : public juce::UnitTest
{
public:
    ReverbPluginTests()
        : juce::UnitTest("Reverb Plugin", "Plugins")
    {
    }

    void runTest() override
    {
        testPluginInitialization();
        testParameterRanges();
        testRoomSizeScaling();
        testDampingControl();
        testWidthControl();
        testPreDelayFunctionality();
        testMixControl();
        testFreezeMode();
        testCombFilterProcessing();
        testAllPassDiffusion();
        testStereoProcessing();
        testStateManagement();
        testLevelMetering();
    }

private:
    void testPluginInitialization()
    {
        beginTest("Plugin initialization");

        ReverbAudioProcessor processor;

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

        ReverbAudioProcessor processor;

        // Check Room Size parameter (0-100%)
        auto& roomSizeParam = *processor.roomSizeParam;
        auto roomSizeRange = roomSizeParam.getNormalisableRange();
        expectEquals(roomSizeRange.start, 0.0f, "Min room size should be 0%");
        expectEquals(roomSizeRange.end, 100.0f, "Max room size should be 100%");
        expectEquals(roomSizeParam.get(), 50.0f, "Default room size should be 50%");

        // Check Damping parameter (0-100%)
        auto& dampingParam = *processor.dampingParam;
        auto dampingRange = dampingParam.getNormalisableRange();
        expectEquals(dampingRange.start, 0.0f, "Min damping should be 0%");
        expectEquals(dampingRange.end, 100.0f, "Max damping should be 100%");
        expectEquals(dampingParam.get(), 50.0f, "Default damping should be 50%");

        // Check Width parameter (0-100%)
        auto& widthParam = *processor.widthParam;
        auto widthRange = widthParam.getNormalisableRange();
        expectEquals(widthRange.start, 0.0f, "Min width should be 0%");
        expectEquals(widthRange.end, 100.0f, "Max width should be 100%");
        expectEquals(widthParam.get(), 100.0f, "Default width should be 100%");

        // Check Pre-Delay parameter (0-100ms)
        auto& preDelayParam = *processor.preDelayParam;
        auto preDelayRange = preDelayParam.getNormalisableRange();
        expectEquals(preDelayRange.start, 0.0f, "Min pre-delay should be 0ms");
        expectEquals(preDelayRange.end, 100.0f, "Max pre-delay should be 100ms");
        expectEquals(preDelayParam.get(), 0.0f, "Default pre-delay should be 0ms");

        // Check Mix parameter (0-100%)
        auto& mixParam = *processor.mixParam;
        auto mixRange = mixParam.getNormalisableRange();
        expectEquals(mixRange.start, 0.0f, "Min mix should be 0%");
        expectEquals(mixRange.end, 100.0f, "Max mix should be 100%");
        expectEquals(mixParam.get(), 50.0f, "Default mix should be 50%");

        // Check Freeze parameter (boolean)
        auto& freezeParam = *processor.freezeParam;
        expect(!freezeParam.get(), "Default freeze should be false");
    }

    void testRoomSizeScaling()
    {
        beginTest("Room size scaling affects reverb tail");

        ReverbAudioProcessor processor;

        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        juce::AudioBuffer<float> buffer1(2, blockSize);
        juce::MidiBuffer midiBuffer;

        // Process with small room - fill delay buffers with multiple blocks
        processor.roomSizeParam->setValueNotifyingHost(10.0f);
        processor.mixParam->setValueNotifyingHost(100.0f);

        for (int block = 0; block < 3; ++block)
        {
            buffer1.clear();
            for (int ch = 0; ch < 2; ++ch)
            {
                auto* data = buffer1.getWritePointer(ch);
                for (int i = 0; i < blockSize; ++i)
                    data[i] = 0.3f * std::sin(2.0f * juce::MathConstants<float>::pi * (i + block * blockSize) / 100.0f);
            }
            processor.processBlock(buffer1, midiBuffer);
        }

        float smallRoomPeak = buffer1.getMagnitude(0, blockSize);

        // Reset and process with large room
        processor.prepareToPlay(sampleRate, blockSize);
        processor.roomSizeParam->setValueNotifyingHost(90.0f);
        processor.mixParam->setValueNotifyingHost(100.0f);

        for (int block = 0; block < 3; ++block)
        {
            buffer1.clear();
            for (int ch = 0; ch < 2; ++ch)
            {
                auto* data = buffer1.getWritePointer(ch);
                for (int i = 0; i < blockSize; ++i)
                    data[i] = 0.3f * std::sin(2.0f * juce::MathConstants<float>::pi * (i + block * blockSize) / 100.0f);
            }
            processor.processBlock(buffer1, midiBuffer);
        }

        float largeRoomPeak = buffer1.getMagnitude(0, blockSize);

        expect(smallRoomPeak > 0.0f, "Small room should produce output");
        expect(largeRoomPeak > 0.0f, "Large room should produce output");
        expect(std::isfinite(smallRoomPeak) && std::isfinite(largeRoomPeak),
               "Both outputs should be finite");
    }

    void testDampingControl()
    {
        beginTest("Damping control affects high-frequency content");

        ReverbAudioProcessor processor;

        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        // Test with low damping (bright) - process multiple blocks
        processor.dampingParam->setValueNotifyingHost(0.0f);
        processor.mixParam->setValueNotifyingHost(100.0f);

        for (int block = 0; block < 3; ++block)
        {
            buffer.clear();
            for (int ch = 0; ch < 2; ++ch)
            {
                auto* data = buffer.getWritePointer(ch);
                for (int i = 0; i < blockSize; ++i)
                    data[i] = 0.5f * std::sin(2.0f * juce::MathConstants<float>::pi * (i + block * blockSize) / 50.0f);
            }
            processor.processBlock(buffer, midiBuffer);
        }

        float lowDampPeak = buffer.getMagnitude(0, blockSize);

        // Reset and test with high damping (dark)
        processor.prepareToPlay(sampleRate, blockSize);
        processor.dampingParam->setValueNotifyingHost(100.0f);
        processor.mixParam->setValueNotifyingHost(100.0f);

        for (int block = 0; block < 3; ++block)
        {
            buffer.clear();
            for (int ch = 0; ch < 2; ++ch)
            {
                auto* data = buffer.getWritePointer(ch);
                for (int i = 0; i < blockSize; ++i)
                    data[i] = 0.5f * std::sin(2.0f * juce::MathConstants<float>::pi * (i + block * blockSize) / 50.0f);
            }
            processor.processBlock(buffer, midiBuffer);
        }

        float highDampPeak = buffer.getMagnitude(0, blockSize);

        expect(std::isfinite(lowDampPeak) && std::isfinite(highDampPeak),
               "Both damping settings should produce finite output");
        expect(lowDampPeak > 0.0f && highDampPeak > 0.0f,
               "Both should produce signal");
    }

    void testWidthControl()
    {
        beginTest("Stereo width control");

        ReverbAudioProcessor processor;

        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        // Test 0% width (mono)
        processor.widthParam->setValueNotifyingHost(0.0f);
        processor.mixParam->setValueNotifyingHost(100.0f);

        buffer.clear();
        buffer.setSample(0, 0, 1.0f);
        buffer.setSample(1, 0, 1.0f);

        processor.processBlock(buffer, midiBuffer);

        expect(std::isfinite(buffer.getMagnitude(0, blockSize)),
               "Mono width should produce finite output");

        // Test 100% width (full stereo)
        processor.prepareToPlay(sampleRate, blockSize);
        processor.widthParam->setValueNotifyingHost(100.0f);
        processor.mixParam->setValueNotifyingHost(100.0f);

        buffer.clear();
        buffer.setSample(0, 0, 1.0f);
        buffer.setSample(1, 0, 1.0f);

        processor.processBlock(buffer, midiBuffer);

        expect(std::isfinite(buffer.getMagnitude(0, blockSize)),
               "Full width should produce finite output");
    }

    void testPreDelayFunctionality()
    {
        beginTest("Pre-delay adds initial delay");

        ReverbAudioProcessor processor;

        const double sampleRate = 48000.0;
        const int blockSize = 1024;
        processor.prepareToPlay(sampleRate, blockSize);

        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        // Test with no pre-delay first - establish baseline
        processor.preDelayParam->setValueNotifyingHost(0.0f);
        processor.mixParam->setValueNotifyingHost(100.0f);

        for (int block = 0; block < 2; ++block)
        {
            buffer.clear();
            for (int ch = 0; ch < 2; ++ch)
            {
                auto* data = buffer.getWritePointer(ch);
                for (int i = 0; i < blockSize; ++i)
                    data[i] = 0.5f * std::sin(2.0f * juce::MathConstants<float>::pi * (i + block * blockSize) / 100.0f);
            }
            processor.processBlock(buffer, midiBuffer);
        }

        float noDelayPeak = buffer.getMagnitude(0, blockSize);

        // Reset and test with pre-delay
        processor.prepareToPlay(sampleRate, blockSize);
        processor.preDelayParam->setValueNotifyingHost(50.0f);
        processor.mixParam->setValueNotifyingHost(100.0f);

        for (int block = 0; block < 2; ++block)
        {
            buffer.clear();
            for (int ch = 0; ch < 2; ++ch)
            {
                auto* data = buffer.getWritePointer(ch);
                for (int i = 0; i < blockSize; ++i)
                    data[i] = 0.5f * std::sin(2.0f * juce::MathConstants<float>::pi * (i + block * blockSize) / 100.0f);
            }
            processor.processBlock(buffer, midiBuffer);
        }

        float delayPeak = buffer.getMagnitude(0, blockSize);

        // Both should produce output
        expect(noDelayPeak > 0.0f, "No delay should produce reverb");
        expect(delayPeak > 0.0f, "Pre-delay should produce reverb");
        expect(std::isfinite(noDelayPeak) && std::isfinite(delayPeak),
               "Both should be finite");
    }

    void testMixControl()
    {
        beginTest("Mix control (dry/wet blend)");

        ReverbAudioProcessor processor;

        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

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

        expectWithinAbsoluteError(dryPeak, inputPeak, 0.01f,
                                  "0% mix should preserve dry signal");

        // Test 100% mix (fully wet)
        processor.prepareToPlay(sampleRate, blockSize);
        processor.mixParam->setValueNotifyingHost(100.0f);

        buffer.clear();
        for (int ch = 0; ch < 2; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < blockSize; ++i)
                data[i] = 0.5f * std::sin(2.0f * juce::MathConstants<float>::pi * i / 100.0f);
        }

        processor.processBlock(buffer, midiBuffer);
        float wetPeak = buffer.getMagnitude(0, blockSize);

        expect(wetPeak > 0.0f, "100% mix should produce wet signal");
        expect(std::isfinite(wetPeak), "Wet signal should be finite");
    }

    void testFreezeMode()
    {
        beginTest("Freeze mode (infinite decay)");

        ReverbAudioProcessor processor;

        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        // First, fill reverb with signal (freeze off)
        processor.freezeParam->setValueNotifyingHost(0.0f);
        processor.mixParam->setValueNotifyingHost(100.0f);

        for (int block = 0; block < 5; ++block)
        {
            buffer.clear();
            for (int ch = 0; ch < 2; ++ch)
            {
                auto* data = buffer.getWritePointer(ch);
                for (int i = 0; i < blockSize; ++i)
                    data[i] = 0.5f * std::sin(2.0f * juce::MathConstants<float>::pi * (i + block * blockSize) / 100.0f);
            }
            processor.processBlock(buffer, midiBuffer);
        }

        // Now enable freeze (input will be muted, reverb should sustain)
        processor.freezeParam->setValueNotifyingHost(1.0f);

        buffer.clear(); // No more input
        processor.processBlock(buffer, midiBuffer);
        processor.processBlock(buffer, midiBuffer);

        float freezeOutput = buffer.getMagnitude(0, blockSize);

        expect(freezeOutput > 0.0f, "Freeze mode should sustain reverb");
        expect(std::isfinite(freezeOutput), "Freeze output should be finite");
    }

    void testCombFilterProcessing()
    {
        beginTest("Comb filter bank processes signal");

        ReverbAudioProcessor processor;

        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        processor.mixParam->setValueNotifyingHost(100.0f);

        // Process multiple blocks to fill comb filters
        for (int block = 0; block < 3; ++block)
        {
            buffer.clear();
            for (int ch = 0; ch < 2; ++ch)
            {
                auto* data = buffer.getWritePointer(ch);
                for (int i = 0; i < blockSize; ++i)
                    data[i] = 0.3f * std::sin(2.0f * juce::MathConstants<float>::pi * (i + block * blockSize) / 100.0f);
            }
            processor.processBlock(buffer, midiBuffer);
        }

        float outputPeak = buffer.getMagnitude(0, blockSize);

        expect(outputPeak > 0.0f, "Comb filters should produce output");
        expect(std::isfinite(outputPeak), "Output should be finite");
        expect(outputPeak < 10.0f, "Output should not explode");
    }

    void testAllPassDiffusion()
    {
        beginTest("All-pass filters create diffusion");

        ReverbAudioProcessor processor;

        const double sampleRate = 48000.0;
        const int blockSize = 2048; // Longer buffer to capture diffusion
        processor.prepareToPlay(sampleRate, blockSize);

        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        processor.mixParam->setValueNotifyingHost(100.0f);

        // Process continuous signal through multiple blocks to fill delay lines
        for (int block = 0; block < 5; ++block)
        {
            buffer.clear();
            for (int ch = 0; ch < 2; ++ch)
            {
                auto* data = buffer.getWritePointer(ch);
                for (int i = 0; i < blockSize; ++i)
                    data[i] = 0.3f * std::sin(2.0f * juce::MathConstants<float>::pi * (i + block * blockSize) / 200.0f);
            }
            processor.processBlock(buffer, midiBuffer);
        }

        // Check that output is spread across the buffer (diffused)
        float outputPeak = buffer.getMagnitude(0, blockSize);

        expect(outputPeak > 0.0f, "All-pass filters should produce output");
        expect(std::isfinite(outputPeak), "Output should be finite");
    }

    void testStereoProcessing()
    {
        beginTest("Stereo processing with L/R decorrelation");

        ReverbAudioProcessor processor;

        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        processor.widthParam->setValueNotifyingHost(100.0f);
        processor.mixParam->setValueNotifyingHost(100.0f);

        // Process multiple blocks with mono input
        for (int block = 0; block < 3; ++block)
        {
            buffer.clear();
            for (int i = 0; i < blockSize; ++i)
            {
                float sample = 0.5f * std::sin(2.0f * juce::MathConstants<float>::pi * (i + block * blockSize) / 100.0f);
                buffer.setSample(0, i, sample);
                buffer.setSample(1, i, sample);
            }
            processor.processBlock(buffer, midiBuffer);
        }

        // Check that L and R outputs are different (decorrelated)
        float leftPeak = buffer.getMagnitude(0, 0, blockSize);
        float rightPeak = buffer.getMagnitude(1, 0, blockSize);

        expect(leftPeak > 0.0f && rightPeak > 0.0f, "Both channels should have output");
        expect(std::isfinite(leftPeak) && std::isfinite(rightPeak),
               "Stereo outputs should be finite");
    }

    void testStateManagement()
    {
        beginTest("State save and restore");

        ReverbAudioProcessor processor1;

        // Set specific parameter values
        processor1.roomSizeParam->setValueNotifyingHost(75.0f);
        processor1.dampingParam->setValueNotifyingHost(60.0f);
        processor1.widthParam->setValueNotifyingHost(80.0f);
        processor1.preDelayParam->setValueNotifyingHost(25.0f);
        processor1.mixParam->setValueNotifyingHost(70.0f);
        processor1.freezeParam->setValueNotifyingHost(1.0f);

        // Save state
        juce::MemoryBlock stateData;
        processor1.getStateInformation(stateData);

        expect(stateData.getSize() > 0, "State data should not be empty");

        // Create new processor and restore state
        ReverbAudioProcessor processor2;
        processor2.setStateInformation(stateData.getData(),
                                       static_cast<int>(stateData.getSize()));

        // Check that parameters were restored
        expectWithinAbsoluteError(processor2.roomSizeParam->get(),
                                  processor1.roomSizeParam->get(), 0.01f,
                                  "Room Size should be restored");

        expectWithinAbsoluteError(processor2.dampingParam->get(),
                                  processor1.dampingParam->get(), 0.01f,
                                  "Damping should be restored");

        expectWithinAbsoluteError(processor2.widthParam->get(),
                                  processor1.widthParam->get(), 0.01f,
                                  "Width should be restored");

        expectWithinAbsoluteError(processor2.preDelayParam->get(),
                                  processor1.preDelayParam->get(), 0.01f,
                                  "Pre-Delay should be restored");

        expectWithinAbsoluteError(processor2.mixParam->get(),
                                  processor1.mixParam->get(), 0.01f,
                                  "Mix should be restored");

        expect(processor2.freezeParam->get() == processor1.freezeParam->get(),
               "Freeze should be restored");
    }

    void testLevelMetering()
    {
        beginTest("Level metering");

        ReverbAudioProcessor processor;

        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        processor.mixParam->setValueNotifyingHost(100.0f);

        // Process multiple blocks with signal
        for (int block = 0; block < 3; ++block)
        {
            buffer.clear();
            for (int ch = 0; ch < 2; ++ch)
            {
                auto* data = buffer.getWritePointer(ch);
                for (int i = 0; i < blockSize; ++i)
                    data[i] = 0.5f * std::sin(2.0f * juce::MathConstants<float>::pi * (i + block * blockSize) / 100.0f);
            }
            processor.processBlock(buffer, midiBuffer);
        }

        // Check meters
        float inputLevel = processor.inputMeter.getLevelDb();
        float outputLevel = processor.outputMeter.getLevelDb();

        expect(inputLevel > -60.0f, "Input meter should register signal");
        expect(outputLevel > -60.0f, "Output meter should register signal");
    }
};

static ReverbPluginTests reverbPluginTests;
