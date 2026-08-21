/**
 * CleanDelay Plugin Tests
 *
 * Comprehensive test suite for the CleanDelay audio effect plugin.
 * Tests delay line, feedback, wet/dry mixing, and ping-pong mode.
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include "../../plugins/CleanDelay/Source/PluginProcessor.h"

//==============================================================================
class CleanDelayTests : public juce::UnitTest
{
public:
    CleanDelayTests()
        : juce::UnitTest("CleanDelay", "Plugins") {}

    void runTest() override
    {
        beginTest("Plugin instantiation");
        testInstantiation();

        beginTest("Plugin metadata");
        testPluginInfo();

        beginTest("Delay line basic operation");
        testDelayLine();

        beginTest("Delay time parameter");
        testDelayTime();

        beginTest("Feedback parameter");
        testFeedback();

        beginTest("Wet/dry mixing");
        testMixing();

        beginTest("Ping-pong mode");
        testPingPong();

        beginTest("State save and restore");
        testStatePreservation();

        beginTest("Zero delay handling");
        testZeroDelay();

        beginTest("Maximum delay handling");
        testMaxDelay();
    }

private:
    void testInstantiation()
    {
        CleanDelayProcessor processor;
        expect(processor.getName().isNotEmpty(), "Plugin should have a name");
        expect(processor.hasEditor(), "Plugin should have an editor");
        expect(!processor.acceptsMidi(), "Plugin should not accept MIDI");
        expect(!processor.producesMidi(), "Plugin should not produce MIDI");
    }

    void testPluginInfo()
    {
        CleanDelayProcessor processor;

        expect(!processor.isMidiEffect(), "Plugin should not be a MIDI effect");
        expect(processor.getTotalNumInputChannels() == 2, "Should have 2 input channels");
        expect(processor.getTotalNumOutputChannels() == 2, "Should have 2 output channels");

        // Tail length should be based on delay time
        double tailLength = processor.getTailLengthSeconds();
        expect(tailLength > 0.0, "Tail length should be positive");
    }

    void testDelayLine()
    {
        CleanDelayProcessor processor;

        // Prepare processor
        processor.prepareToPlay(44100.0, 512);

        // Create test buffer with impulse
        juce::AudioBuffer<float> buffer(2, 512);
        buffer.clear();
        buffer.setSample(0, 0, 1.0f);  // Left channel impulse
        buffer.setSample(1, 0, 1.0f);  // Right channel impulse

        // Set delay time to 100ms (4410 samples at 44.1kHz)
        const auto& params = processor.getParameters();
        auto* delayTimeParam = dynamic_cast<juce::AudioParameterFloat*>(params[0]);
        delayTimeParam->setValueNotifyingHost(delayTimeParam->convertTo0to1(0.1f));

        // Set 100% wet for easier testing
        auto* mixParam = dynamic_cast<juce::AudioParameterFloat*>(params[2]);
        mixParam->setValueNotifyingHost(1.0f);

        // Set zero feedback for pure delay test
        auto* feedbackParam = dynamic_cast<juce::AudioParameterFloat*>(params[1]);
        feedbackParam->setValueNotifyingHost(0.0f);

        juce::MidiBuffer midiBuffer;

        // Process first block (input impulse)
        processor.processBlock(buffer, midiBuffer);

        // First block should be silent (impulse is in delay line)
        float firstBlockMax = buffer.getMagnitude(0, 512);
        expect(firstBlockMax < 0.01f, "First block should be mostly silent");

        // Process blocks until we reach the delay time
        // 100ms at 44.1kHz = 4410 samples = 8.6 blocks of 512 samples
        float maxOutput = 0.0f;
        for (int i = 0; i < 10; ++i)
        {
            buffer.clear();
            processor.processBlock(buffer, midiBuffer);
            float blockMax = buffer.getMagnitude(0, 512);
            maxOutput = juce::jmax(maxOutput, blockMax);
        }

        // We should have seen the delayed impulse in one of these blocks
        expect(maxOutput > 0.5f, "Delayed impulse should appear after delay time");

        processor.releaseResources();
    }

    void testDelayTime()
    {
        CleanDelayProcessor processor;
        processor.prepareToPlay(44100.0, 512);

        const auto& params = processor.getParameters();
        auto* delayTimeParam = dynamic_cast<juce::AudioParameterFloat*>(params[0]);

        // Test minimum delay time (1ms)
        delayTimeParam->setValueNotifyingHost(delayTimeParam->convertTo0to1(0.001f));
        expectWithinAbsoluteError(delayTimeParam->get(), 0.001f, 0.0001f, "Minimum delay time should be 1ms");

        // Test maximum delay time (2s)
        delayTimeParam->setValueNotifyingHost(delayTimeParam->convertTo0to1(2.0f));
        expectWithinAbsoluteError(delayTimeParam->get(), 2.0f, 0.01f, "Maximum delay time should be 2s");

        // Test middle value (250ms default)
        delayTimeParam->setValueNotifyingHost(delayTimeParam->convertTo0to1(0.25f));
        expectWithinAbsoluteError(delayTimeParam->get(), 0.25f, 0.01f, "Default delay time should be 250ms");

        processor.releaseResources();
    }

    void testFeedback()
    {
        CleanDelayProcessor processor;
        processor.prepareToPlay(44100.0, 512);

        // Create test buffer with impulse
        juce::AudioBuffer<float> buffer(2, 512);
        buffer.clear();
        buffer.setSample(0, 0, 1.0f);

        // Set short delay (10ms = 441 samples)
        const auto& params = processor.getParameters();
        auto* delayTimeParam = dynamic_cast<juce::AudioParameterFloat*>(params[0]);
        delayTimeParam->setValueNotifyingHost(delayTimeParam->convertTo0to1(0.01f));

        // Set 100% wet
        auto* mixParam = dynamic_cast<juce::AudioParameterFloat*>(params[2]);
        mixParam->setValueNotifyingHost(1.0f);

        // Set 50% feedback
        auto* feedbackParam = dynamic_cast<juce::AudioParameterFloat*>(params[1]);
        feedbackParam->setValueNotifyingHost(feedbackParam->convertTo0to1(0.5f));

        juce::MidiBuffer midiBuffer;

        // Process first block
        processor.processBlock(buffer, midiBuffer);

        // Process second block (should have first echo at 50% amplitude)
        buffer.clear();
        processor.processBlock(buffer, midiBuffer);
        float firstEcho = buffer.getMagnitude(0, 512);

        // Process third block (should have second echo at 25% amplitude)
        buffer.clear();
        processor.processBlock(buffer, midiBuffer);
        float secondEcho = buffer.getMagnitude(0, 512);

        // Second echo should be approximately half the amplitude of first echo
        expect(secondEcho < firstEcho, "Feedback should decay with each repetition");
        expect(secondEcho > 0.1f, "Second echo should still be audible with 50% feedback");

        processor.releaseResources();
    }

    void testMixing()
    {
        CleanDelayProcessor processor;
        processor.prepareToPlay(44100.0, 512);

        // Create test buffer with constant signal
        juce::AudioBuffer<float> buffer(2, 512);
        buffer.clear();
        for (int i = 0; i < 512; ++i)
            buffer.setSample(0, i, 0.5f);

        // Set delay time
        const auto& params = processor.getParameters();
        auto* delayTimeParam = dynamic_cast<juce::AudioParameterFloat*>(params[0]);
        delayTimeParam->setValueNotifyingHost(delayTimeParam->convertTo0to1(0.1f));

        // Set zero feedback
        auto* feedbackParam = dynamic_cast<juce::AudioParameterFloat*>(params[1]);
        feedbackParam->setValueNotifyingHost(0.0f);

        // Test 0% wet (100% dry)
        auto* mixParam = dynamic_cast<juce::AudioParameterFloat*>(params[2]);
        mixParam->setValueNotifyingHost(0.0f);

        juce::MidiBuffer midiBuffer;
        processor.processBlock(buffer, midiBuffer);

        float dryMagnitude = buffer.getMagnitude(0, 512);
        expectWithinAbsoluteError(dryMagnitude, 0.5f, 0.01f, "0% wet should preserve dry signal");

        // Test 100% wet (0% dry)
        buffer.clear();
        for (int i = 0; i < 512; ++i)
            buffer.setSample(0, i, 0.5f);

        mixParam->setValueNotifyingHost(1.0f);

        // Process several blocks to fill delay line
        for (int i = 0; i < 10; ++i)
        {
            buffer.clear();
            for (int j = 0; j < 512; ++j)
                buffer.setSample(0, j, 0.5f);
            processor.processBlock(buffer, midiBuffer);
        }

        float wetMagnitude = buffer.getMagnitude(0, 512);
        expect(wetMagnitude > 0.0f, "100% wet should output delayed signal");

        processor.releaseResources();
    }

    void testPingPong()
    {
        CleanDelayProcessor processor;
        processor.prepareToPlay(44100.0, 512);

        // Create test buffer with left channel only impulse
        juce::AudioBuffer<float> buffer(2, 512);
        buffer.clear();
        buffer.setSample(0, 0, 1.0f);  // Left channel
        buffer.setSample(1, 0, 0.0f);  // Right channel silent

        // Set short delay (10ms)
        const auto& params = processor.getParameters();
        auto* delayTimeParam = dynamic_cast<juce::AudioParameterFloat*>(params[0]);
        delayTimeParam->setValueNotifyingHost(delayTimeParam->convertTo0to1(0.01f));

        // Set 100% wet
        auto* mixParam = dynamic_cast<juce::AudioParameterFloat*>(params[2]);
        mixParam->setValueNotifyingHost(1.0f);

        // Set 80% feedback for multiple echoes
        auto* feedbackParam = dynamic_cast<juce::AudioParameterFloat*>(params[1]);
        feedbackParam->setValueNotifyingHost(feedbackParam->convertTo0to1(0.8f));

        // Enable ping-pong mode
        auto* pingPongParam = dynamic_cast<juce::AudioParameterBool*>(params[3]);
        pingPongParam->setValueNotifyingHost(1.0f);

        juce::MidiBuffer midiBuffer;

        // Process first block (input)
        processor.processBlock(buffer, midiBuffer);

        // Process second block (first echo - should be in left channel)
        buffer.clear();
        processor.processBlock(buffer, midiBuffer);
        float leftFirstEcho = buffer.getMagnitude(0, 512);
        float rightFirstEcho = buffer.getMagnitude(1, 512);

        // Process third block (second echo - should ping-pong to right channel)
        buffer.clear();
        processor.processBlock(buffer, midiBuffer);
        float leftSecondEcho = buffer.getMagnitude(0, 512);
        float rightSecondEcho = buffer.getMagnitude(1, 512);

        // In ping-pong mode, echoes should alternate between channels
        expect(leftFirstEcho > 0.5f, "First echo should appear in left channel");
        expect(rightSecondEcho > 0.3f, "Second echo should ping-pong to right channel");

        processor.releaseResources();
    }

    void testStatePreservation()
    {
        CleanDelayProcessor processor1;

        // Set parameters to non-default values
        const auto& params = processor1.getParameters();
        auto* delayTimeParam = dynamic_cast<juce::AudioParameterFloat*>(params[0]);
        auto* feedbackParam = dynamic_cast<juce::AudioParameterFloat*>(params[1]);
        auto* mixParam = dynamic_cast<juce::AudioParameterFloat*>(params[2]);
        auto* pingPongParam = dynamic_cast<juce::AudioParameterBool*>(params[3]);

        delayTimeParam->setValueNotifyingHost(delayTimeParam->convertTo0to1(0.5f));
        feedbackParam->setValueNotifyingHost(feedbackParam->convertTo0to1(0.7f));
        mixParam->setValueNotifyingHost(0.8f);
        pingPongParam->setValueNotifyingHost(1.0f);

        // Save state
        juce::MemoryBlock stateData;
        processor1.getStateInformation(stateData);

        // Create new processor and restore state
        CleanDelayProcessor processor2;
        processor2.setStateInformation(stateData.getData(), static_cast<int>(stateData.getSize()));

        const auto& params2 = processor2.getParameters();
        auto* delayTimeParam2 = dynamic_cast<juce::AudioParameterFloat*>(params2[0]);
        auto* feedbackParam2 = dynamic_cast<juce::AudioParameterFloat*>(params2[1]);
        auto* mixParam2 = dynamic_cast<juce::AudioParameterFloat*>(params2[2]);
        auto* pingPongParam2 = dynamic_cast<juce::AudioParameterBool*>(params2[3]);

        expectWithinAbsoluteError(delayTimeParam2->get(), 0.5f, 0.01f, "Delay time should be preserved");
        expectWithinAbsoluteError(feedbackParam2->get(), 0.7f, 0.01f, "Feedback should be preserved");
        expectWithinAbsoluteError(mixParam2->get(), 0.8f, 0.01f, "Mix should be preserved");
        expect(pingPongParam2->get(), "Ping-pong mode should be preserved");
    }

    void testZeroDelay()
    {
        CleanDelayProcessor processor;
        processor.prepareToPlay(44100.0, 512);

        // Create test buffer
        juce::AudioBuffer<float> buffer(2, 512);
        buffer.clear();
        for (int i = 0; i < 512; ++i)
            buffer.setSample(0, i, 0.5f);

        // Set minimum delay (1ms)
        const auto& params = processor.getParameters();
        auto* delayTimeParam = dynamic_cast<juce::AudioParameterFloat*>(params[0]);
        delayTimeParam->setValueNotifyingHost(delayTimeParam->convertTo0to1(0.001f));

        // Set 50% wet for audible effect
        auto* mixParam = dynamic_cast<juce::AudioParameterFloat*>(params[2]);
        mixParam->setValueNotifyingHost(0.5f);

        juce::MidiBuffer midiBuffer;

        // Process should not crash or produce NaN/inf
        processor.processBlock(buffer, midiBuffer);

        float magnitude = buffer.getMagnitude(0, 512);
        expect(!std::isnan(magnitude), "Output should not be NaN");
        expect(!std::isinf(magnitude), "Output should not be infinite");
        expect(magnitude > 0.0f, "Output should have signal");

        processor.releaseResources();
    }

    void testMaxDelay()
    {
        CleanDelayProcessor processor;
        processor.prepareToPlay(44100.0, 512);

        // Create test buffer
        juce::AudioBuffer<float> buffer(2, 512);
        buffer.clear();
        for (int i = 0; i < 512; ++i)
            buffer.setSample(0, i, 0.5f);

        // Set maximum delay (2s)
        const auto& params = processor.getParameters();
        auto* delayTimeParam = dynamic_cast<juce::AudioParameterFloat*>(params[0]);
        delayTimeParam->setValueNotifyingHost(delayTimeParam->convertTo0to1(2.0f));

        // Set 50% wet
        auto* mixParam = dynamic_cast<juce::AudioParameterFloat*>(params[2]);
        mixParam->setValueNotifyingHost(0.5f);

        juce::MidiBuffer midiBuffer;

        // Process should not crash or produce NaN/inf
        processor.processBlock(buffer, midiBuffer);

        float magnitude = buffer.getMagnitude(0, 512);
        expect(!std::isnan(magnitude), "Output should not be NaN");
        expect(!std::isinf(magnitude), "Output should not be infinite");
        expect(magnitude > 0.0f, "Output should have signal");

        processor.releaseResources();
    }
};

static CleanDelayTests cleanDelayTests;
