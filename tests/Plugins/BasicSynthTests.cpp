/**
 * BasicSynth Plugin Tests
 *
 * Integration tests for the BasicSynth synthesizer:
 * - Plugin initialization
 * - Oscillator waveform generation
 * - MIDI note handling
 * - Voice allocation and polyphony
 * - ADSR envelope behavior
 * - Parameter management
 * - State save/restore
 */

#include <juce_audio_processors/juce_audio_processors.h>

// Include BasicSynth plugin headers
#include "../../plugins/BasicSynth/Source/PluginProcessor.h"

class BasicSynthPluginTests : public juce::UnitTest
{
public:
    BasicSynthPluginTests()
        : juce::UnitTest("BasicSynth Plugin", "Plugins")
    {
    }

    void runTest() override
    {
        testPluginInitialization();
        testParameterRanges();
        testOscillatorOutput();
        testMIDINoteHandling();
        testPolyphony();
        testADSREnvelope();
        testStateManagement();
    }

private:
    void testPluginInitialization()
    {
        beginTest("Plugin initialization");

        BasicSynthProcessor processor;

        // Check basic plugin properties
        expect(processor.getName().isNotEmpty(), "Plugin should have a name");
        expect(processor.acceptsMidi(), "Should accept MIDI");
        expect(!processor.producesMidi(), "Should not produce MIDI");
        expect(!processor.isMidiEffect(), "Should not be MIDI effect");
        expect(processor.getTailLengthSeconds() == 0.0, "Should have no tail");

        // Check bus configuration (stereo output)
        expect(processor.getBusCount(true) == 0, "Should have no input bus");
        expect(processor.getBusCount(false) == 1, "Should have one output bus");

        // Check parameter count (8 parameters)
        const auto& params = processor.getParameters();
        expect(params.size() == 8, "Should have 8 parameters");
    }

    void testParameterRanges()
    {
        beginTest("Parameter range validation");

        BasicSynthProcessor processor;
        const auto& params = processor.getParameters();

        // Waveform parameter (choice)
        auto* waveformParam = dynamic_cast<juce::AudioParameterChoice*>(params[0]);
        expect(waveformParam != nullptr, "Waveform should be choice parameter");
        expect(waveformParam->choices.size() == 3, "Should have 3 waveforms");

        // Volume parameter
        auto* volumeParam = dynamic_cast<juce::AudioParameterFloat*>(params[1]);
        expect(volumeParam != nullptr, "Volume should be float parameter");
        auto volRange = volumeParam->getNormalisableRange();
        expectEquals(volRange.start, 0.0f, "Volume min should be 0.0");
        expectEquals(volRange.end, 1.0f, "Volume max should be 1.0");

        // Attack parameter
        auto* attackParam = dynamic_cast<juce::AudioParameterFloat*>(params[2]);
        expect(attackParam != nullptr, "Attack should be float parameter");
        auto attackRange = attackParam->getNormalisableRange();
        expectEquals(attackRange.start, 0.001f, "Attack min should be 0.001");
        expectEquals(attackRange.end, 2.0f, "Attack max should be 2.0");

        // Sustain parameter
        auto* sustainParam = dynamic_cast<juce::AudioParameterFloat*>(params[4]);
        expect(sustainParam != nullptr, "Sustain should be float parameter");
        auto sustainRange = sustainParam->getNormalisableRange();
        expectEquals(sustainRange.start, 0.0f, "Sustain min should be 0.0");
        expectEquals(sustainRange.end, 1.0f, "Sustain max should be 1.0");
    }

    void testOscillatorOutput()
    {
        beginTest("Oscillator output generation");

        BasicSynthProcessor processor;

        // Prepare processor
        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        // Create buffers
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        // Send a MIDI note on
        midiBuffer.addEvent(juce::MidiMessage::noteOn(1, 60, 0.8f), 0);

        // Process audio
        processor.processBlock(buffer, midiBuffer);

        // Check that output is not silent
        float peakLevel = buffer.getMagnitude(0, blockSize);
        expect(peakLevel > 0.0f, "Should generate non-zero output for note");
        expect(peakLevel <= 1.0f, "Output should not exceed 1.0");
    }

    void testMIDINoteHandling()
    {
        beginTest("MIDI note on/off handling");

        BasicSynthProcessor processor;
        processor.prepareToPlay(48000.0, 512);

        juce::AudioBuffer<float> buffer(2, 512);
        juce::MidiBuffer midiBuffer;

        // Note ON
        midiBuffer.addEvent(juce::MidiMessage::noteOn(1, 60, 0.8f), 0);
        buffer.clear();
        processor.processBlock(buffer, midiBuffer);

        float peakAfterNoteOn = buffer.getMagnitude(0, 512);
        expect(peakAfterNoteOn > 0.0f, "Should have output after note on");

        // Note OFF
        midiBuffer.clear();
        midiBuffer.addEvent(juce::MidiMessage::noteOff(1, 60), 0);

        // Process multiple blocks to let release phase complete
        for (int i = 0; i < 50; ++i)
        {
            buffer.clear();
            processor.processBlock(buffer, midiBuffer);
            midiBuffer.clear();
        }

        float peakAfterRelease = buffer.getMagnitude(0, 512);

        // After release phase, output should be silent or very quiet
        expect(peakAfterRelease < 0.01f, "Should be quiet after release phase");
    }

    void testPolyphony()
    {
        beginTest("Voice polyphony (8 voices)");

        BasicSynthProcessor processor;
        processor.prepareToPlay(48000.0, 512);

        juce::AudioBuffer<float> buffer(2, 512);
        juce::MidiBuffer midiBuffer;

        // Play 8 notes simultaneously (max polyphony)
        for (int note = 60; note < 68; ++note)
        {
            midiBuffer.addEvent(juce::MidiMessage::noteOn(1, note, 0.7f), 0);
        }

        buffer.clear();
        processor.processBlock(buffer, midiBuffer);

        float polyphonyPeak = buffer.getMagnitude(0, 512);
        expect(polyphonyPeak > 0.0f, "Should generate output with 8 voices");

        // Now add a 9th note (should trigger voice stealing)
        midiBuffer.clear();
        midiBuffer.addEvent(juce::MidiMessage::noteOn(1, 72, 0.7f), 0);

        buffer.clear();
        processor.processBlock(buffer, midiBuffer);

        float afterStealingPeak = buffer.getMagnitude(0, 512);
        expect(afterStealingPeak > 0.0f, "Should still generate output after voice stealing");
    }

    void testADSREnvelope()
    {
        beginTest("ADSR envelope behavior");

        BasicSynthProcessor processor;
        processor.prepareToPlay(48000.0, 64);

        const auto& params = processor.getParameters();
        auto* attackParam = dynamic_cast<juce::AudioParameterFloat*>(params[2]);
        auto* decayParam = dynamic_cast<juce::AudioParameterFloat*>(params[3]);
        auto* sustainParam = dynamic_cast<juce::AudioParameterFloat*>(params[4]);
        auto* releaseParam = dynamic_cast<juce::AudioParameterFloat*>(params[5]);

        // Set fast attack, medium decay, 50% sustain, fast release
        attackParam->setValueNotifyingHost(attackParam->convertTo0to1(0.005f)); // 5ms
        decayParam->setValueNotifyingHost(decayParam->convertTo0to1(0.05f));    // 50ms
        sustainParam->setValueNotifyingHost(sustainParam->convertTo0to1(0.5f));  // 50%
        releaseParam->setValueNotifyingHost(releaseParam->convertTo0to1(0.05f)); // 50ms

        juce::AudioBuffer<float> buffer(2, 64);
        juce::MidiBuffer midiBuffer;

        // Note on
        midiBuffer.addEvent(juce::MidiMessage::noteOn(1, 60, 1.0f), 0);

        // Process attack phase (should ramp up quickly)
        buffer.clear();
        processor.processBlock(buffer, midiBuffer);
        float attackPeak = buffer.getMagnitude(0, 64);

        expect(attackPeak > 0.0f, "Should have output during attack");

        // Process decay/sustain phase
        midiBuffer.clear();
        for (int i = 0; i < 10; ++i)
        {
            buffer.clear();
            processor.processBlock(buffer, midiBuffer);
        }

        float sustainPeak = buffer.getMagnitude(0, 64);
        expect(sustainPeak > 0.0f, "Should have output at sustain level");

        // Note off (trigger release)
        midiBuffer.clear();
        midiBuffer.addEvent(juce::MidiMessage::noteOff(1, 60), 0);

        buffer.clear();
        processor.processBlock(buffer, midiBuffer);

        float releasePeak = buffer.getMagnitude(0, 64);
        expect(releasePeak > 0.0f, "Should still have output at start of release");
    }

    void testStateManagement()
    {
        beginTest("State save and restore");

        BasicSynthProcessor processor1;

        // Set specific parameter values
        const auto& params = processor1.getParameters();
        auto* waveformParam = dynamic_cast<juce::AudioParameterChoice*>(params[0]);
        auto* volumeParam = dynamic_cast<juce::AudioParameterFloat*>(params[1]);
        auto* attackParam = dynamic_cast<juce::AudioParameterFloat*>(params[2]);

        waveformParam->setValueNotifyingHost(2.0f / (waveformParam->choices.size() - 1)); // Square
        volumeParam->setValueNotifyingHost(volumeParam->convertTo0to1(0.5f));
        attackParam->setValueNotifyingHost(attackParam->convertTo0to1(0.1f));

        // Save state
        juce::MemoryBlock stateData;
        processor1.getStateInformation(stateData);

        expect(stateData.getSize() > 0, "State data should not be empty");

        // Create new processor and restore state
        BasicSynthProcessor processor2;
        processor2.setStateInformation(stateData.getData(),
                                      static_cast<int>(stateData.getSize()));

        // Check that parameters were restored
        const auto& params2 = processor2.getParameters();
        auto* waveformParam2 = dynamic_cast<juce::AudioParameterChoice*>(params2[0]);
        auto* volumeParam2 = dynamic_cast<juce::AudioParameterFloat*>(params2[1]);
        auto* attackParam2 = dynamic_cast<juce::AudioParameterFloat*>(params2[2]);

        expectEquals(waveformParam2->getIndex(), 2, "Waveform should be restored");
        expectWithinAbsoluteError(volumeParam2->get(), 0.5f, 0.01f, "Volume should be restored");
        expectWithinAbsoluteError(attackParam2->get(), 0.1f, 0.01f, "Attack should be restored");
    }
};

static BasicSynthPluginTests basicSynthPluginTests;
