/**
 * SimpleComp Plugin Tests
 *
 * Tests for the dynamic range compressor.
 */

#include "../../plugins/SimpleComp/Source/PluginProcessor.h"
#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================
class SimpleCompTests : public juce::UnitTest
{
public:
    SimpleCompTests() : UnitTest("SimpleComp Tests", "Plugins") {}

    void runTest() override
    {
        testPluginInfo();
        testParameters();
        testInitialization();
        testBelowThreshold();
        testAboveThreshold4to1();
        testAboveThreshold10to1();
        testHardKnee();
        testSoftKnee();
        testMakeupGain();
        testAttackResponse();
        testReleaseResponse();
        testStatePreservation();
    }

private:
    void testPluginInfo()
    {
        beginTest("Plugin name");

        SimpleCompProcessor processor;
        expect(processor.getName().isNotEmpty(), "Plugin name should not be empty");
    }

    void testParameters()
    {
        beginTest("Parameter count and IDs");

        SimpleCompProcessor processor;
        const auto& params = processor.getParameters();

        // Should have 5 parameters
        expect(params.size() == 5, "Should have 5 parameters");

        // Check parameter IDs exist
        bool hasThreshold = false, hasRatio = false;
        bool hasAttack = false, hasRelease = false, hasKnee = false;

        for (auto* param : params)
        {
            juce::String id = param->getName(32);
            if (id.contains("Threshold")) hasThreshold = true;
            if (id.contains("Ratio")) hasRatio = true;
            if (id.contains("Attack")) hasAttack = true;
            if (id.contains("Release")) hasRelease = true;
            if (id.contains("Knee")) hasKnee = true;
        }

        expect(hasThreshold && hasRatio && hasAttack && hasRelease && hasKnee,
               "All parameters present");
    }

    void testInitialization()
    {
        beginTest("Plugin initialization");

        SimpleCompProcessor processor;
        processor.prepareToPlay(44100.0, 512);

        juce::AudioBuffer<float> buffer(2, 512);
        juce::MidiBuffer midiBuffer;

        // Fill with moderate signal
        for (int sample = 0; sample < 512; ++sample)
        {
            buffer.setSample(0, sample, 0.1f);
            buffer.setSample(1, sample, 0.1f);
        }

        // Should process without crashing
        processor.processBlock(buffer, midiBuffer);

        float output = buffer.getSample(0, 0);
        expect(std::isfinite(output), "Output should be finite");
    }

    void testBelowThreshold()
    {
        beginTest("Signal below threshold (no compression)");

        SimpleCompProcessor processor;
        processor.prepareToPlay(44100.0, 512);

        // Set threshold to -20dB
        const auto& params = processor.getParameters();
        for (auto* param : params)
        {
            if (juce::String(param->getName(32)).contains("Threshold"))
            {
                auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
                floatParam->setValueNotifyingHost(floatParam->convertTo0to1(-20.0f));
            }
        }

        // Generate signal at -30dB (below threshold)
        juce::AudioBuffer<float> buffer(2, 512);
        juce::MidiBuffer midiBuffer;

        float inputLevel = 0.0316f;  // -30dB
        for (int sample = 0; sample < 512; ++sample)
        {
            buffer.setSample(0, sample, inputLevel);
            buffer.setSample(1, sample, inputLevel);
        }

        float inputRMS = buffer.getRMSLevel(0, 0, 512);

        processor.processBlock(buffer, midiBuffer);

        float outputRMS = buffer.getRMSLevel(0, 0, 512);

        // Output should be close to input (with makeup gain applied)
        // Allow some variation due to makeup gain
        expect(outputRMS > inputRMS * 0.8f && outputRMS < inputRMS * 2.0f,
               "Signal below threshold should pass through with minimal change");
    }

    void testAboveThreshold4to1()
    {
        beginTest("Compression above threshold (4:1 ratio)");

        SimpleCompProcessor processor;
        processor.prepareToPlay(44100.0, 512);

        // Set threshold to -20dB, ratio to 4:1
        const auto& params = processor.getParameters();
        for (auto* param : params)
        {
            juce::String name = param->getName(32);
            if (name.contains("Threshold"))
            {
                auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
                floatParam->setValueNotifyingHost(floatParam->convertTo0to1(-20.0f));
            }
            else if (name.contains("Ratio"))
            {
                auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
                floatParam->setValueNotifyingHost(floatParam->convertTo0to1(4.0f));
            }
        }

        // Generate signal at -10dB (10dB above threshold)
        juce::AudioBuffer<float> buffer(2, 512);
        juce::MidiBuffer midiBuffer;

        float inputLevel = 0.316f;  // -10dB
        for (int sample = 0; sample < 512; ++sample)
        {
            buffer.setSample(0, sample, inputLevel);
            buffer.setSample(1, sample, inputLevel);
        }

        // Process for several blocks to let envelope settle
        for (int block = 0; block < 20; ++block)
        {
            buffer.clear();
            for (int sample = 0; sample < 512; ++sample)
            {
                buffer.setSample(0, sample, inputLevel);
                buffer.setSample(1, sample, inputLevel);
            }
            processor.processBlock(buffer, midiBuffer);
        }

        // Check that gain reduction occurred
        float gainReduction = processor.getCurrentGainReduction();
        expect(gainReduction > 3.0f,  // Should have at least 3dB GR (10dB over / 4 = 2.5dB, plus envelope smoothing)
               "Should apply compression above threshold");
    }

    void testAboveThreshold10to1()
    {
        beginTest("Compression above threshold (10:1 ratio)");

        SimpleCompProcessor processor;
        processor.prepareToPlay(44100.0, 512);

        // Set threshold to -20dB, ratio to 10:1
        const auto& params = processor.getParameters();
        for (auto* param : params)
        {
            juce::String name = param->getName(32);
            if (name.contains("Threshold"))
            {
                auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
                floatParam->setValueNotifyingHost(floatParam->convertTo0to1(-20.0f));
            }
            else if (name.contains("Ratio"))
            {
                auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
                floatParam->setValueNotifyingHost(floatParam->convertTo0to1(10.0f));
            }
        }

        // Generate signal at -10dB (10dB above threshold)
        juce::AudioBuffer<float> buffer(2, 512);
        juce::MidiBuffer midiBuffer;

        float inputLevel = 0.316f;  // -10dB
        for (int sample = 0; sample < 512; ++sample)
        {
            buffer.setSample(0, sample, inputLevel);
            buffer.setSample(1, sample, inputLevel);
        }

        // Process for several blocks to let envelope settle
        for (int block = 0; block < 20; ++block)
        {
            buffer.clear();
            for (int sample = 0; sample < 512; ++sample)
            {
                buffer.setSample(0, sample, inputLevel);
                buffer.setSample(1, sample, inputLevel);
            }
            processor.processBlock(buffer, midiBuffer);
        }

        // Check that gain reduction occurred
        float gainReduction = processor.getCurrentGainReduction();
        expect(gainReduction > 5.0f,  // Higher ratio should give more GR
               "Higher ratio should produce more compression");
    }

    void testHardKnee()
    {
        beginTest("Hard knee mode");

        SimpleCompProcessor processor;
        processor.prepareToPlay(44100.0, 512);

        // Set to hard knee
        const auto& params = processor.getParameters();
        for (auto* param : params)
        {
            if (juce::String(param->getName(32)).contains("Knee"))
            {
                auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param);
                choiceParam->setValueNotifyingHost(0.0f);  // Hard knee
            }
        }

        // Process some audio
        juce::AudioBuffer<float> buffer(2, 512);
        juce::MidiBuffer midiBuffer;

        for (int sample = 0; sample < 512; ++sample)
        {
            buffer.setSample(0, sample, 0.5f);
            buffer.setSample(1, sample, 0.5f);
        }

        // Should process without crashing
        processor.processBlock(buffer, midiBuffer);
        expect(std::isfinite(buffer.getSample(0, 0)), "Hard knee should produce finite output");
    }

    void testSoftKnee()
    {
        beginTest("Soft knee mode");

        SimpleCompProcessor processor;
        processor.prepareToPlay(44100.0, 512);

        // Set to soft knee
        const auto& params = processor.getParameters();
        for (auto* param : params)
        {
            if (juce::String(param->getName(32)).contains("Knee"))
            {
                auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param);
                choiceParam->setValueNotifyingHost(1.0f);  // Soft knee
            }
        }

        // Process some audio
        juce::AudioBuffer<float> buffer(2, 512);
        juce::MidiBuffer midiBuffer;

        for (int sample = 0; sample < 512; ++sample)
        {
            buffer.setSample(0, sample, 0.5f);
            buffer.setSample(1, sample, 0.5f);
        }

        // Should process without crashing
        processor.processBlock(buffer, midiBuffer);
        expect(std::isfinite(buffer.getSample(0, 0)), "Soft knee should produce finite output");
    }

    void testMakeupGain()
    {
        beginTest("Automatic makeup gain");

        SimpleCompProcessor processor;
        processor.prepareToPlay(44100.0, 512);

        // Set aggressive compression (should trigger makeup gain)
        const auto& params = processor.getParameters();
        for (auto* param : params)
        {
            juce::String name = param->getName(32);
            if (name.contains("Threshold"))
            {
                auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
                floatParam->setValueNotifyingHost(floatParam->convertTo0to1(-30.0f));
            }
            else if (name.contains("Ratio"))
            {
                auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
                floatParam->setValueNotifyingHost(floatParam->convertTo0to1(10.0f));
            }
        }

        // Generate moderate signal
        juce::AudioBuffer<float> buffer(2, 512);
        juce::MidiBuffer midiBuffer;

        float inputLevel = 0.1f;
        for (int sample = 0; sample < 512; ++sample)
        {
            buffer.setSample(0, sample, inputLevel);
            buffer.setSample(1, sample, inputLevel);
        }

        processor.processBlock(buffer, midiBuffer);

        float outputLevel = std::abs(buffer.getSample(0, 256));

        // With makeup gain, output should not be drastically quieter than input
        expect(outputLevel > 0.05f,
               "Makeup gain should compensate for volume reduction");
    }

    void testAttackResponse()
    {
        beginTest("Attack time response");

        SimpleCompProcessor processor;
        processor.prepareToPlay(44100.0, 512);

        // Set fast attack
        const auto& params = processor.getParameters();
        for (auto* param : params)
        {
            if (juce::String(param->getName(32)).contains("Attack"))
            {
                auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
                floatParam->setValueNotifyingHost(floatParam->convertTo0to1(1.0f));  // 1ms
            }
        }

        // Process silence, then sudden loud signal
        juce::AudioBuffer<float> buffer(2, 512);
        juce::MidiBuffer midiBuffer;

        // Process silence first
        buffer.clear();
        processor.processBlock(buffer, midiBuffer);

        // Now process loud signal
        for (int sample = 0; sample < 512; ++sample)
        {
            buffer.setSample(0, sample, 0.8f);
            buffer.setSample(1, sample, 0.8f);
        }

        processor.processBlock(buffer, midiBuffer);

        // Fast attack should kick in quickly
        expect(processor.getCurrentGainReduction() > 0.0f,
               "Fast attack should respond to sudden level increase");
    }

    void testReleaseResponse()
    {
        beginTest("Release time response");

        SimpleCompProcessor processor;
        processor.prepareToPlay(44100.0, 512);

        // Set fast release
        const auto& params = processor.getParameters();
        for (auto* param : params)
        {
            if (juce::String(param->getName(32)).contains("Release"))
            {
                auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
                floatParam->setValueNotifyingHost(floatParam->convertTo0to1(20.0f));  // 20ms
            }
        }

        // Process loud signal to build up gain reduction
        juce::AudioBuffer<float> buffer(2, 512);
        juce::MidiBuffer midiBuffer;

        for (int block = 0; block < 10; ++block)
        {
            for (int sample = 0; sample < 512; ++sample)
            {
                buffer.setSample(0, sample, 0.8f);
                buffer.setSample(1, sample, 0.8f);
            }
            processor.processBlock(buffer, midiBuffer);
        }

        // Now process silence
        buffer.clear();
        for (int block = 0; block < 5; ++block)
            processor.processBlock(buffer, midiBuffer);

        // Fast release should reduce gain reduction
        expect(processor.getCurrentGainReduction() < 5.0f,
               "Release should reduce gain reduction when signal drops");
    }

    void testStatePreservation()
    {
        beginTest("State preservation");

        SimpleCompProcessor processor;

        // Set some parameter values
        const auto& params = processor.getParameters();
        for (auto* param : params)
        {
            juce::String name = param->getName(32);
            if (name.contains("Threshold"))
            {
                auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
                floatParam->setValueNotifyingHost(floatParam->convertTo0to1(-15.0f));
            }
            else if (name.contains("Ratio"))
            {
                auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
                floatParam->setValueNotifyingHost(floatParam->convertTo0to1(6.0f));
            }
        }

        // Save state
        juce::MemoryBlock stateData;
        processor.getStateInformation(stateData);

        // Create new processor and restore state
        SimpleCompProcessor newProcessor;
        newProcessor.setStateInformation(stateData.getData(), static_cast<int>(stateData.getSize()));

        // Check if state was preserved
        const auto& newParams = newProcessor.getParameters();
        float restoredThreshold = 0.0f;
        float restoredRatio = 0.0f;

        for (auto* param : newParams)
        {
            juce::String name = param->getName(32);
            if (name.contains("Threshold"))
            {
                auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
                restoredThreshold = floatParam->get();
            }
            else if (name.contains("Ratio"))
            {
                auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
                restoredRatio = floatParam->get();
            }
        }

        expectWithinAbsoluteError(restoredThreshold, -15.0f, 0.1f, "Threshold should be preserved");
        expectWithinAbsoluteError(restoredRatio, 6.0f, 0.1f, "Ratio should be preserved");
    }
};

static SimpleCompTests simpleCompTests;
