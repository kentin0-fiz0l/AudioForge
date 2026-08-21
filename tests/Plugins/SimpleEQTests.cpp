/**
 * SimpleEQ Plugin Tests
 *
 * Tests for the 3-band parametric EQ with optional HPF/LPF.
 */

#include "../../plugins/SimpleEQ/Source/PluginProcessor.h"
#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================
class SimpleEQTests : public juce::UnitTest
{
public:
    SimpleEQTests() : UnitTest("SimpleEQ Tests", "Plugins") {}

    void runTest() override
    {
        testPluginInfo();
        testParameters();
        testInitialization();
        testLowShelfBoost();
        testLowShelfCut();
        testMidBandBoost();
        testMidBandCut();
        testHighShelfBoost();
        testHighShelfCut();
        testHighPassFilter();
        testLowPassFilter();
        testCascadedProcessing();
        testStatePreservation();
    }

private:
    void testPluginInfo()
    {
        beginTest("Plugin name");

        SimpleEQProcessor processor;
        expect(processor.getName().isNotEmpty(), "Plugin name should not be empty");
    }

    void testParameters()
    {
        beginTest("Parameter count and IDs");

        SimpleEQProcessor processor;
        const auto& params = processor.getParameters();

        // Should have 10 parameters
        expect(params.size() == 10, "Should have 10 parameters");

        // Check parameter IDs exist
        bool hasLowFreq = false, hasLowGain = false;
        bool hasMidFreq = false, hasMidGain = false;
        bool hasHighFreq = false, hasHighGain = false;
        bool hasHpfEnabled = false, hasHpfFreq = false;
        bool hasLpfEnabled = false, hasLpfFreq = false;

        for (auto* param : params)
        {
            juce::String id = param->getName(32);
            if (id.contains("Low Frequency")) hasLowFreq = true;
            if (id.contains("Low Gain")) hasLowGain = true;
            if (id.contains("Mid Frequency")) hasMidFreq = true;
            if (id.contains("Mid Gain")) hasMidGain = true;
            if (id.contains("High Frequency")) hasHighFreq = true;
            if (id.contains("High Gain")) hasHighGain = true;
            if (id.contains("HPF Enabled")) hasHpfEnabled = true;
            if (id.contains("HPF Frequency")) hasHpfFreq = true;
            if (id.contains("LPF Enabled")) hasLpfEnabled = true;
            if (id.contains("LPF Frequency")) hasLpfFreq = true;
        }

        expect(hasLowFreq && hasLowGain && hasMidFreq && hasMidGain &&
               hasHighFreq && hasHighGain && hasHpfEnabled && hasHpfFreq &&
               hasLpfEnabled && hasLpfFreq, "All parameters present");
    }

    void testInitialization()
    {
        beginTest("Plugin initialization");

        SimpleEQProcessor processor;
        processor.prepareToPlay(44100.0, 512);

        juce::AudioBuffer<float> buffer(2, 512);
        juce::MidiBuffer midiBuffer;

        // Fill with unity signal
        for (int sample = 0; sample < 512; ++sample)
        {
            buffer.setSample(0, sample, 0.5f);
            buffer.setSample(1, sample, 0.5f);
        }

        // Should process without crashing
        processor.processBlock(buffer, midiBuffer);

        float output = buffer.getSample(0, 0);
        expect(std::isfinite(output), "Output should be finite");
    }

    void testLowShelfBoost()
    {
        beginTest("Low shelf boost (+6dB at 80Hz)");

        SimpleEQProcessor processor;
        processor.prepareToPlay(44100.0, 512);

        // Set low shelf to +6dB at 80Hz
        const auto& params = processor.getParameters();
        for (auto* param : params)
        {
            if (juce::String(param->getName(32)).contains("Low Gain"))
            {
                auto* gainParam = dynamic_cast<juce::AudioParameterFloat*>(param);
                if (gainParam)
                    gainParam->setValueNotifyingHost(gainParam->convertTo0to1(6.0f));
            }
        }

        // Process one buffer to update filter coefficients
        juce::AudioBuffer<float> dummyBuffer(2, 512);
        juce::MidiBuffer dummyMidi;
        processor.processBlock(dummyBuffer, dummyMidi);

        // Generate 80Hz sine wave
        juce::AudioBuffer<float> buffer(2, 512);
        juce::MidiBuffer midiBuffer;
        const float frequency = 80.0f;
        const float sampleRate = 44100.0f;

        for (int sample = 0; sample < 512; ++sample)
        {
            float value = std::sin(2.0f * juce::MathConstants<float>::pi * frequency * sample / sampleRate);
            buffer.setSample(0, sample, value * 0.5f);
            buffer.setSample(1, sample, value * 0.5f);
        }

        float inputRMS = buffer.getRMSLevel(0, 0, 512);

        processor.processBlock(buffer, midiBuffer);

        float outputRMS = buffer.getRMSLevel(0, 0, 512);

        // With +6dB boost at shelf frequency, expect at least +3dB (1.4x) amplification
        // Full +6dB is reached at frequencies well below the shelf frequency
        expect(outputRMS > inputRMS * 1.3f,
               "Low shelf boost should amplify low frequencies");
    }

    void testLowShelfCut()
    {
        beginTest("Low shelf cut (-6dB at 80Hz)");

        SimpleEQProcessor processor;
        processor.prepareToPlay(44100.0, 512);

        // Set low shelf to -6dB
        const auto& params = processor.getParameters();
        for (auto* param : params)
        {
            if (juce::String(param->getName(32)).contains("Low Gain"))
            {
                auto* gainParam = dynamic_cast<juce::AudioParameterFloat*>(param);
                if (gainParam)
                    gainParam->setValueNotifyingHost(gainParam->convertTo0to1(-6.0f));
            }
        }

        // Process one buffer to update filter coefficients
        juce::AudioBuffer<float> dummyBuffer(2, 512);
        juce::MidiBuffer dummyMidi;
        processor.processBlock(dummyBuffer, dummyMidi);

        // Generate 80Hz sine wave
        juce::AudioBuffer<float> buffer(2, 512);
        juce::MidiBuffer midiBuffer;
        const float frequency = 80.0f;
        const float sampleRate = 44100.0f;

        for (int sample = 0; sample < 512; ++sample)
        {
            float value = std::sin(2.0f * juce::MathConstants<float>::pi * frequency * sample / sampleRate);
            buffer.setSample(0, sample, value * 0.5f);
            buffer.setSample(1, sample, value * 0.5f);
        }

        float inputRMS = buffer.getRMSLevel(0, 0, 512);

        processor.processBlock(buffer, midiBuffer);

        float outputRMS = buffer.getRMSLevel(0, 0, 512);

        // With -6dB cut at shelf frequency, expect at least -3dB (0.7x) attenuation
        // Full -6dB is reached at frequencies well below the shelf frequency
        expect(outputRMS < inputRMS * 0.75f,
               "Low shelf cut should attenuate low frequencies");
    }

    void testMidBandBoost()
    {
        beginTest("Mid band boost (+6dB at 1kHz)");

        SimpleEQProcessor processor;
        processor.prepareToPlay(44100.0, 512);

        // Set mid band to +6dB at 1kHz
        const auto& params = processor.getParameters();
        for (auto* param : params)
        {
            if (juce::String(param->getName(32)).contains("Mid Gain"))
            {
                auto* gainParam = dynamic_cast<juce::AudioParameterFloat*>(param);
                if (gainParam)
                    gainParam->setValueNotifyingHost(gainParam->convertTo0to1(6.0f));
            }
        }

        // Process one buffer to update filter coefficients
        juce::AudioBuffer<float> dummyBuffer(2, 512);
        juce::MidiBuffer dummyMidi;
        processor.processBlock(dummyBuffer, dummyMidi);

        // Generate 1kHz sine wave
        juce::AudioBuffer<float> buffer(2, 512);
        juce::MidiBuffer midiBuffer;
        const float frequency = 1000.0f;
        const float sampleRate = 44100.0f;

        for (int sample = 0; sample < 512; ++sample)
        {
            float value = std::sin(2.0f * juce::MathConstants<float>::pi * frequency * sample / sampleRate);
            buffer.setSample(0, sample, value * 0.5f);
            buffer.setSample(1, sample, value * 0.5f);
        }

        float inputRMS = buffer.getRMSLevel(0, 0, 512);

        processor.processBlock(buffer, midiBuffer);

        float outputRMS = buffer.getRMSLevel(0, 0, 512);

        expect(outputRMS > inputRMS * 1.8f && outputRMS < inputRMS * 2.2f,
               "Mid band boost should amplify mid frequencies");
    }

    void testMidBandCut()
    {
        beginTest("Mid band cut (-6dB at 1kHz)");

        SimpleEQProcessor processor;
        processor.prepareToPlay(44100.0, 512);

        // Set mid band to -6dB
        const auto& params = processor.getParameters();
        for (auto* param : params)
        {
            if (juce::String(param->getName(32)).contains("Mid Gain"))
            {
                auto* gainParam = dynamic_cast<juce::AudioParameterFloat*>(param);
                if (gainParam)
                    gainParam->setValueNotifyingHost(gainParam->convertTo0to1(-6.0f));
            }
        }

        // Process one buffer to update filter coefficients
        juce::AudioBuffer<float> dummyBuffer(2, 512);
        juce::MidiBuffer dummyMidi;
        processor.processBlock(dummyBuffer, dummyMidi);

        // Generate 1kHz sine wave
        juce::AudioBuffer<float> buffer(2, 512);
        juce::MidiBuffer midiBuffer;
        const float frequency = 1000.0f;
        const float sampleRate = 44100.0f;

        for (int sample = 0; sample < 512; ++sample)
        {
            float value = std::sin(2.0f * juce::MathConstants<float>::pi * frequency * sample / sampleRate);
            buffer.setSample(0, sample, value * 0.5f);
            buffer.setSample(1, sample, value * 0.5f);
        }

        float inputRMS = buffer.getRMSLevel(0, 0, 512);

        processor.processBlock(buffer, midiBuffer);

        float outputRMS = buffer.getRMSLevel(0, 0, 512);

        expect(outputRMS < inputRMS * 0.6f && outputRMS > inputRMS * 0.4f,
               "Mid band cut should attenuate mid frequencies");
    }

    void testHighShelfBoost()
    {
        beginTest("High shelf boost (+6dB at 8kHz)");

        SimpleEQProcessor processor;
        processor.prepareToPlay(44100.0, 512);

        // Set high shelf to +6dB at 8kHz
        const auto& params = processor.getParameters();
        for (auto* param : params)
        {
            if (juce::String(param->getName(32)).contains("High Gain"))
            {
                auto* gainParam = dynamic_cast<juce::AudioParameterFloat*>(param);
                if (gainParam)
                    gainParam->setValueNotifyingHost(gainParam->convertTo0to1(6.0f));
            }
        }

        // Process one buffer to update filter coefficients
        juce::AudioBuffer<float> dummyBuffer(2, 512);
        juce::MidiBuffer dummyMidi;
        processor.processBlock(dummyBuffer, dummyMidi);

        // Generate 8kHz sine wave
        juce::AudioBuffer<float> buffer(2, 512);
        juce::MidiBuffer midiBuffer;
        const float frequency = 8000.0f;
        const float sampleRate = 44100.0f;

        for (int sample = 0; sample < 512; ++sample)
        {
            float value = std::sin(2.0f * juce::MathConstants<float>::pi * frequency * sample / sampleRate);
            buffer.setSample(0, sample, value * 0.5f);
            buffer.setSample(1, sample, value * 0.5f);
        }

        float inputRMS = buffer.getRMSLevel(0, 0, 512);

        processor.processBlock(buffer, midiBuffer);

        float outputRMS = buffer.getRMSLevel(0, 0, 512);

        // With +6dB boost at shelf frequency, expect at least +3dB (1.4x) amplification
        // Full +6dB is reached at frequencies well above the shelf frequency
        expect(outputRMS > inputRMS * 1.3f,
               "High shelf boost should amplify high frequencies");
    }

    void testHighShelfCut()
    {
        beginTest("High shelf cut (-6dB at 8kHz)");

        SimpleEQProcessor processor;
        processor.prepareToPlay(44100.0, 512);

        // Set high shelf to -6dB
        const auto& params = processor.getParameters();
        for (auto* param : params)
        {
            if (juce::String(param->getName(32)).contains("High Gain"))
            {
                auto* gainParam = dynamic_cast<juce::AudioParameterFloat*>(param);
                if (gainParam)
                    gainParam->setValueNotifyingHost(gainParam->convertTo0to1(-6.0f));
            }
        }

        // Process one buffer to update filter coefficients
        juce::AudioBuffer<float> dummyBuffer(2, 512);
        juce::MidiBuffer dummyMidi;
        processor.processBlock(dummyBuffer, dummyMidi);

        // Generate 8kHz sine wave
        juce::AudioBuffer<float> buffer(2, 512);
        juce::MidiBuffer midiBuffer;
        const float frequency = 8000.0f;
        const float sampleRate = 44100.0f;

        for (int sample = 0; sample < 512; ++sample)
        {
            float value = std::sin(2.0f * juce::MathConstants<float>::pi * frequency * sample / sampleRate);
            buffer.setSample(0, sample, value * 0.5f);
            buffer.setSample(1, sample, value * 0.5f);
        }

        float inputRMS = buffer.getRMSLevel(0, 0, 512);

        processor.processBlock(buffer, midiBuffer);

        float outputRMS = buffer.getRMSLevel(0, 0, 512);

        // With -6dB cut at shelf frequency, expect at least -3dB (0.7x) attenuation
        // Full -6dB is reached at frequencies well above the shelf frequency
        expect(outputRMS < inputRMS * 0.75f,
               "High shelf cut should attenuate high frequencies");
    }

    void testHighPassFilter()
    {
        beginTest("High-pass filter (40Hz)");

        SimpleEQProcessor processor;
        processor.prepareToPlay(44100.0, 512);

        // Enable HPF
        const auto& params = processor.getParameters();
        for (auto* param : params)
        {
            if (juce::String(param->getName(32)).contains("HPF Enabled"))
            {
                auto* enabledParam = dynamic_cast<juce::AudioParameterBool*>(param);
                if (enabledParam)
                    enabledParam->setValueNotifyingHost(1.0f);
            }
        }

        // Process one buffer to update filter coefficients
        juce::AudioBuffer<float> dummyBuffer(2, 512);
        juce::MidiBuffer dummyMidi;
        processor.processBlock(dummyBuffer, dummyMidi);

        // Generate 30Hz sine wave (below cutoff)
        juce::AudioBuffer<float> buffer(2, 512);
        juce::MidiBuffer midiBuffer;
        const float frequency = 30.0f;
        const float sampleRate = 44100.0f;

        for (int sample = 0; sample < 512; ++sample)
        {
            float value = std::sin(2.0f * juce::MathConstants<float>::pi * frequency * sample / sampleRate);
            buffer.setSample(0, sample, value * 0.5f);
            buffer.setSample(1, sample, value * 0.5f);
        }

        float inputRMS = buffer.getRMSLevel(0, 0, 512);

        processor.processBlock(buffer, midiBuffer);

        float outputRMS = buffer.getRMSLevel(0, 0, 512);

        // 30Hz should be significantly attenuated by 40Hz HPF
        expect(outputRMS < inputRMS * 0.7f,
               "HPF should attenuate frequencies below cutoff");
    }

    void testLowPassFilter()
    {
        beginTest("Low-pass filter (12kHz)");

        SimpleEQProcessor processor;
        processor.prepareToPlay(44100.0, 512);

        // Enable LPF
        const auto& params = processor.getParameters();
        for (auto* param : params)
        {
            if (juce::String(param->getName(32)).contains("LPF Enabled"))
            {
                auto* enabledParam = dynamic_cast<juce::AudioParameterBool*>(param);
                if (enabledParam)
                    enabledParam->setValueNotifyingHost(1.0f);
            }
        }

        // Process one buffer to update filter coefficients
        juce::AudioBuffer<float> dummyBuffer(2, 512);
        juce::MidiBuffer dummyMidi;
        processor.processBlock(dummyBuffer, dummyMidi);

        // Generate 15kHz sine wave (above cutoff)
        juce::AudioBuffer<float> buffer(2, 512);
        juce::MidiBuffer midiBuffer;
        const float frequency = 15000.0f;
        const float sampleRate = 44100.0f;

        for (int sample = 0; sample < 512; ++sample)
        {
            float value = std::sin(2.0f * juce::MathConstants<float>::pi * frequency * sample / sampleRate);
            buffer.setSample(0, sample, value * 0.5f);
            buffer.setSample(1, sample, value * 0.5f);
        }

        float inputRMS = buffer.getRMSLevel(0, 0, 512);

        processor.processBlock(buffer, midiBuffer);

        float outputRMS = buffer.getRMSLevel(0, 0, 512);

        // 15kHz should be significantly attenuated by 12kHz LPF
        expect(outputRMS < inputRMS * 0.7f,
               "LPF should attenuate frequencies above cutoff");
    }

    void testCascadedProcessing()
    {
        beginTest("Cascaded filter processing");

        SimpleEQProcessor processor;
        processor.prepareToPlay(44100.0, 512);

        // Enable HPF, boost low, cut mid, boost high, enable LPF
        const auto& params = processor.getParameters();
        for (auto* param : params)
        {
            juce::String name = param->getName(32);

            if (name.contains("HPF Enabled"))
            {
                auto* boolParam = dynamic_cast<juce::AudioParameterBool*>(param);
                if (boolParam) boolParam->setValueNotifyingHost(1.0f);
            }
            else if (name.contains("Low Gain"))
            {
                auto* gainParam = dynamic_cast<juce::AudioParameterFloat*>(param);
                if (gainParam) gainParam->setValueNotifyingHost(gainParam->convertTo0to1(3.0f));
            }
            else if (name.contains("Mid Gain"))
            {
                auto* gainParam = dynamic_cast<juce::AudioParameterFloat*>(param);
                if (gainParam) gainParam->setValueNotifyingHost(gainParam->convertTo0to1(-3.0f));
            }
            else if (name.contains("High Gain"))
            {
                auto* gainParam = dynamic_cast<juce::AudioParameterFloat*>(param);
                if (gainParam) gainParam->setValueNotifyingHost(gainParam->convertTo0to1(3.0f));
            }
            else if (name.contains("LPF Enabled"))
            {
                auto* boolParam = dynamic_cast<juce::AudioParameterBool*>(param);
                if (boolParam) boolParam->setValueNotifyingHost(1.0f);
            }
        }

        // Process white noise
        juce::AudioBuffer<float> buffer(2, 512);
        juce::MidiBuffer midiBuffer;

        juce::Random random;
        for (int sample = 0; sample < 512; ++sample)
        {
            float noise = (random.nextFloat() * 2.0f - 1.0f) * 0.5f;
            buffer.setSample(0, sample, noise);
            buffer.setSample(1, sample, noise);
        }

        // Should process without issues
        processor.processBlock(buffer, midiBuffer);

        // Output should be finite and within reasonable bounds
        float maxOutput = buffer.getMagnitude(0, 512);
        expect(std::isfinite(maxOutput), "Cascaded output should be finite");
        expect(maxOutput < 2.0f, "Cascaded output should not clip severely");
    }

    void testStatePreservation()
    {
        beginTest("State preservation");

        SimpleEQProcessor processor;

        // Set some parameter values
        const auto& params = processor.getParameters();
        for (auto* param : params)
        {
            if (juce::String(param->getName(32)).contains("Low Gain"))
            {
                auto* gainParam = dynamic_cast<juce::AudioParameterFloat*>(param);
                if (gainParam)
                    gainParam->setValueNotifyingHost(gainParam->convertTo0to1(5.0f));
            }
        }

        // Save state
        juce::MemoryBlock stateData;
        processor.getStateInformation(stateData);

        // Create new processor and restore state
        SimpleEQProcessor newProcessor;
        newProcessor.setStateInformation(stateData.getData(), static_cast<int>(stateData.getSize()));

        // Check if state was preserved
        const auto& newParams = newProcessor.getParameters();
        float restoredGain = 0.0f;
        for (auto* param : newParams)
        {
            if (juce::String(param->getName(32)).contains("Low Gain"))
            {
                auto* gainParam = dynamic_cast<juce::AudioParameterFloat*>(param);
                if (gainParam)
                    restoredGain = gainParam->get();
            }
        }

        expectWithinAbsoluteError(restoredGain, 5.0f, 0.1f, "Low gain should be preserved");
    }
};

static SimpleEQTests simpleEQTests;
