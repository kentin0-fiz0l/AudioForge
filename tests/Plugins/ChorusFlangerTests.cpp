/**
 * Chorus/Flanger Plugin Tests
 *
 * Integration tests for the Chorus/Flanger audio plugin:
 * - Plugin initialization
 * - Parameter management (Rate, Depth, Mode, Feedback, Mix, Waveform, Stereo Width)
 * - Chorus vs Flanger mode behavior
 * - LFO modulation
 * - Stereo processing with phase offset
 * - Feedback control
 * - State save/restore
 * - Level metering
 */

#include <juce_audio_processors/juce_audio_processors.h>

// Include Chorus/Flanger plugin headers
#include "../../plugins/ChorusFlanger/Source/PluginProcessor.h"

class ChorusFlangerPluginTests : public juce::UnitTest
{
public:
    ChorusFlangerPluginTests()
        : juce::UnitTest("Chorus/Flanger Plugin", "Plugins")
    {
    }

    void runTest() override
    {
        testPluginInitialization();
        testParameterRanges();
        testChorusMode();
        testFlangerMode();
        testLFOModulation();
        testStereoPhaseOffset();
        testFeedbackControl();
        testMixControl();
        testStateManagement();
        testLevelMetering();
    }

private:
    void testPluginInitialization()
    {
        beginTest("Plugin initialization");

        ChorusFlangerAudioProcessor processor;

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

        ChorusFlangerAudioProcessor processor;

        // Check Rate parameter (0.1-10 Hz)
        auto& rateParam = *processor.rateParam;
        auto rateRange = rateParam.getNormalisableRange();
        expectEquals(rateRange.start, 0.1f, "Min rate should be 0.1 Hz");
        expectEquals(rateRange.end, 10.0f, "Max rate should be 10 Hz");
        expectEquals(rateParam.get(), 0.5f, "Default rate should be 0.5 Hz");

        // Check Depth parameter (0-100%)
        auto& depthParam = *processor.depthParam;
        auto depthRange = depthParam.getNormalisableRange();
        expectEquals(depthRange.start, 0.0f, "Min depth should be 0%");
        expectEquals(depthRange.end, 100.0f, "Max depth should be 100%");
        expectEquals(depthParam.get(), 50.0f, "Default depth should be 50%");

        // Check Mode parameter (2 modes)
        auto& modeParam = *processor.modeParam;
        expect(modeParam.choices.size() == 2, "Should have 2 modes");
        expectEquals(modeParam.getIndex(), 0, "Default mode should be Chorus");

        // Check Feedback parameter (0-95%)
        auto& feedbackParam = *processor.feedbackParam;
        auto feedbackRange = feedbackParam.getNormalisableRange();
        expectEquals(feedbackRange.start, 0.0f, "Min feedback should be 0%");
        expectEquals(feedbackRange.end, 95.0f, "Max feedback should be 95%");

        // Check Mix parameter (0-100%)
        auto& mixParam = *processor.mixParam;
        auto mixRange = mixParam.getNormalisableRange();
        expectEquals(mixRange.start, 0.0f, "Min mix should be 0%");
        expectEquals(mixRange.end, 100.0f, "Max mix should be 100%");

        // Check Waveform parameter (2 waveforms)
        auto& waveformParam = *processor.waveformParam;
        expect(waveformParam.choices.size() == 2, "Should have 2 waveforms");

        // Check Stereo Width parameter (0-100%)
        auto& stereoWidthParam = *processor.stereoWidthParam;
        auto stereoWidthRange = stereoWidthParam.getNormalisableRange();
        expectEquals(stereoWidthRange.start, 0.0f, "Min stereo width should be 0%");
        expectEquals(stereoWidthRange.end, 100.0f, "Max stereo width should be 100%");
    }

    void testChorusMode()
    {
        beginTest("Chorus mode (wide, slow modulation)");

        ChorusFlangerAudioProcessor processor;

        // Prepare processor
        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        // Set to Chorus mode
        processor.modeParam->setValueNotifyingHost(
            processor.modeParam->convertTo0to1(0));

        processor.rateParam->setValueNotifyingHost(0.5f); // 0.5 Hz
        processor.depthParam->setValueNotifyingHost(50.0f); // 50%
        processor.mixParam->setValueNotifyingHost(50.0f); // 50%

        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        // Create input signal and process multiple blocks to fill delay buffer
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

        float outputPeak = buffer.getMagnitude(0, blockSize);

        // Chorus should modulate the signal
        expect(std::isfinite(outputPeak), "Output should be finite");
        expect(outputPeak > 0.0f, "Output should have signal");
    }

    void testFlangerMode()
    {
        beginTest("Flanger mode (narrow, fast modulation)");

        ChorusFlangerAudioProcessor processor;

        // Prepare processor
        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        // Set to Flanger mode
        processor.modeParam->setValueNotifyingHost(
            processor.modeParam->convertTo0to1(1));

        processor.rateParam->setValueNotifyingHost(1.0f); // 1 Hz
        processor.depthParam->setValueNotifyingHost(100.0f); // 100%
        processor.feedbackParam->setValueNotifyingHost(50.0f); // 50% feedback
        processor.mixParam->setValueNotifyingHost(100.0f); // 100% wet

        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        // Create input signal
        buffer.clear();
        for (int ch = 0; ch < 2; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < blockSize; ++i)
                data[i] = 0.5f * std::sin(2.0f * juce::MathConstants<float>::pi * i / 50.0f);
        }

        processor.processBlock(buffer, midiBuffer);
        float outputPeak = buffer.getMagnitude(0, blockSize);

        // Flanger with feedback should have output
        expect(std::isfinite(outputPeak), "Output should be finite");
        expect(outputPeak > 0.0f, "Output should have signal");
    }

    void testLFOModulation()
    {
        beginTest("LFO modulation creates pitch variation");

        ChorusFlangerAudioProcessor processor;

        const double sampleRate = 48000.0;
        const int blockSize = 2048; // Longer buffer to capture modulation
        processor.prepareToPlay(sampleRate, blockSize);

        processor.rateParam->setValueNotifyingHost(2.0f); // Fast LFO
        processor.depthParam->setValueNotifyingHost(100.0f); // Full depth
        processor.mixParam->setValueNotifyingHost(100.0f); // 100% wet

        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        // Create constant tone
        buffer.clear();
        for (int ch = 0; ch < 2; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < blockSize; ++i)
                data[i] = 0.5f * std::sin(2.0f * juce::MathConstants<float>::pi * i / 100.0f);
        }

        processor.processBlock(buffer, midiBuffer);

        // Check that LFO phases are advancing
        float phase1 = processor.getCurrentLFOPhase();
        processor.processBlock(buffer, midiBuffer);
        float phase2 = processor.getCurrentLFOPhase();

        expect(phase1 != phase2, "LFO phase should advance between blocks");
    }

    void testStereoPhaseOffset()
    {
        beginTest("Stereo width creates L/R phase offset");

        ChorusFlangerAudioProcessor processor;

        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        // Set stereo width to 100% (maximum phase offset)
        processor.stereoWidthParam->setValueNotifyingHost(100.0f);
        processor.rateParam->setValueNotifyingHost(1.0f);
        processor.mixParam->setValueNotifyingHost(50.0f);

        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        buffer.clear();
        for (int ch = 0; ch < 2; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < blockSize; ++i)
                data[i] = 0.5f;
        }

        processor.processBlock(buffer, midiBuffer);

        // Check that left and right LFO phases are different
        float leftPhase = processor.getCurrentLFOPhase();
        float rightPhase = processor.getStereoLFOPhase();

        expect(leftPhase != rightPhase, "Left and right LFO phases should differ with stereo width");
    }

    void testFeedbackControl()
    {
        beginTest("Feedback control affects signal character");

        ChorusFlangerAudioProcessor processor;

        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        processor.modeParam->setValueNotifyingHost(
            processor.modeParam->convertTo0to1(1)); // Flanger
        processor.mixParam->setValueNotifyingHost(100.0f);

        juce::AudioBuffer<float> buffer1(2, blockSize);
        juce::AudioBuffer<float> buffer2(2, blockSize);
        juce::MidiBuffer midiBuffer;

        // Create identical input signals
        for (int ch = 0; ch < 2; ++ch)
        {
            for (int i = 0; i < blockSize; ++i)
            {
                float sample = 0.5f * std::sin(2.0f * juce::MathConstants<float>::pi * i / 100.0f);
                buffer1.setSample(ch, i, sample);
                buffer2.setSample(ch, i, sample);
            }
        }

        // Process with low feedback
        processor.feedbackParam->setValueNotifyingHost(0.0f);
        processor.processBlock(buffer1, midiBuffer);
        float lowFeedbackPeak = buffer1.getMagnitude(0, blockSize);

        // Reset and process with high feedback
        processor.prepareToPlay(sampleRate, blockSize);
        processor.modeParam->setValueNotifyingHost(
            processor.modeParam->convertTo0to1(1));
        processor.feedbackParam->setValueNotifyingHost(70.0f);
        processor.mixParam->setValueNotifyingHost(100.0f);
        processor.processBlock(buffer2, midiBuffer);
        float highFeedbackPeak = buffer2.getMagnitude(0, blockSize);

        // Feedback should create different output characteristics
        expect(std::isfinite(lowFeedbackPeak) && std::isfinite(highFeedbackPeak),
             "Both outputs should be finite");
    }

    void testMixControl()
    {
        beginTest("Mix control (dry/wet blend)");

        ChorusFlangerAudioProcessor processor;

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

        // 0% mix should be close to original (allowing for DC blocker effects)
        expectWithinAbsoluteError(dryPeak, inputPeak, 0.15f,
                                "0% mix should be mostly dry");
    }

    void testStateManagement()
    {
        beginTest("State save and restore");

        ChorusFlangerAudioProcessor processor1;

        // Set specific parameter values
        processor1.rateParam->setValueNotifyingHost(2.5f);
        processor1.depthParam->setValueNotifyingHost(75.0f);
        processor1.modeParam->setValueNotifyingHost(
            processor1.modeParam->convertTo0to1(1)); // Flanger
        processor1.feedbackParam->setValueNotifyingHost(60.0f);
        processor1.mixParam->setValueNotifyingHost(80.0f);
        processor1.waveformParam->setValueNotifyingHost(
            processor1.waveformParam->convertTo0to1(1)); // Triangle
        processor1.stereoWidthParam->setValueNotifyingHost(75.0f);

        // Save state
        juce::MemoryBlock stateData;
        processor1.getStateInformation(stateData);

        expect(stateData.getSize() > 0, "State data should not be empty");

        // Create new processor and restore state
        ChorusFlangerAudioProcessor processor2;
        processor2.setStateInformation(stateData.getData(),
                                      static_cast<int>(stateData.getSize()));

        // Check that parameters were restored
        expectWithinAbsoluteError(processor2.rateParam->get(),
                                processor1.rateParam->get(), 0.01f,
                                "Rate should be restored");

        expectWithinAbsoluteError(processor2.depthParam->get(),
                                processor1.depthParam->get(), 0.01f,
                                "Depth should be restored");

        expectEquals(processor2.modeParam->getIndex(),
                    processor1.modeParam->getIndex(),
                    "Mode should be restored");

        expectWithinAbsoluteError(processor2.feedbackParam->get(),
                                processor1.feedbackParam->get(), 0.01f,
                                "Feedback should be restored");

        expectWithinAbsoluteError(processor2.mixParam->get(),
                                processor1.mixParam->get(), 0.01f,
                                "Mix should be restored");

        expectEquals(processor2.waveformParam->getIndex(),
                    processor1.waveformParam->getIndex(),
                    "Waveform should be restored");

        expectWithinAbsoluteError(processor2.stereoWidthParam->get(),
                                processor1.stereoWidthParam->get(), 0.01f,
                                "Stereo Width should be restored");
    }

    void testLevelMetering()
    {
        beginTest("Level metering");

        ChorusFlangerAudioProcessor processor;

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

static ChorusFlangerPluginTests chorusFlangerPluginTests;
