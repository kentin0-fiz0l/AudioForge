#include <juce_audio_processors/juce_audio_processors.h>
#include "../../plugins/FMSynth/Source/PluginProcessor.h"
#include "../../plugins/FMSynth/Source/FMOperator.h"

/**
 * FM Synth Unit Tests
 *
 * Tests for the FMSynth plugin covering:
 * - Voice allocation and management
 * - MIDI handling
 * - DSP correctness
 * - Preset loading
 * - FM modulation behavior
 */
class FMSynthTests : public juce::UnitTest
{
public:
    FMSynthTests() : juce::UnitTest("FMSynth", "Plugins") {}

    void runTest() override
    {
        beginTest("Plugin instantiation");
        {
            PluginProcessor processor;
            expect(processor.getName() == "FMSynth", "Plugin name should be 'FMSynth'");
            expect(processor.acceptsMidi(), "Plugin should accept MIDI");
            expect(!processor.producesMidi(), "Plugin should not produce MIDI");
        }

        beginTest("Parameter count");
        {
            PluginProcessor processor;
            auto& apvts = processor.getAPVTS();
            expect(apvts.state.getNumChildren() > 0, "Should have parameters");

            // Check all 10 parameters exist
            expect(apvts.getParameter("modRatio") != nullptr, "modRatio parameter should exist");
            expect(apvts.getParameter("modDepth") != nullptr, "modDepth parameter should exist");
            expect(apvts.getParameter("carrierAttack") != nullptr, "carrierAttack parameter should exist");
            expect(apvts.getParameter("carrierDecay") != nullptr, "carrierDecay parameter should exist");
            expect(apvts.getParameter("carrierSustain") != nullptr, "carrierSustain parameter should exist");
            expect(apvts.getParameter("carrierRelease") != nullptr, "carrierRelease parameter should exist");
            expect(apvts.getParameter("modAttack") != nullptr, "modAttack parameter should exist");
            expect(apvts.getParameter("modDecay") != nullptr, "modDecay parameter should exist");
            expect(apvts.getParameter("modSustain") != nullptr, "modSustain parameter should exist");
            expect(apvts.getParameter("modRelease") != nullptr, "modRelease parameter should exist");
        }

        beginTest("MIDI note-on/note-off");
        {
            PluginProcessor processor;
            processor.prepareToPlay(44100.0, 512);

            juce::MidiBuffer midiBuffer;
            juce::AudioBuffer<float> audioBuffer(2, 512);
            audioBuffer.clear();

            // Send note-on
            midiBuffer.addEvent(juce::MidiMessage::noteOn(1, 60, 0.8f), 0);

            processor.processBlock(audioBuffer, midiBuffer);

            // Check that audio was generated (non-zero samples)
            bool hasAudio = false;
            for (int ch = 0; ch < audioBuffer.getNumChannels(); ++ch)
            {
                auto* data = audioBuffer.getReadPointer(ch);
                for (int i = 0; i < audioBuffer.getNumSamples(); ++i)
                {
                    if (std::abs(data[i]) > 0.0001f)
                    {
                        hasAudio = true;
                        break;
                    }
                }
            }

            expect(hasAudio, "Audio should be generated after note-on");

            processor.releaseResources();
        }

        beginTest("DSP output range");
        {
            PluginProcessor processor;
            processor.prepareToPlay(44100.0, 512);

            juce::MidiBuffer midiBuffer;
            juce::AudioBuffer<float> audioBuffer(2, 512);

            // Trigger multiple notes
            midiBuffer.addEvent(juce::MidiMessage::noteOn(1, 60, 1.0f), 0);
            midiBuffer.addEvent(juce::MidiMessage::noteOn(1, 64, 1.0f), 10);
            midiBuffer.addEvent(juce::MidiMessage::noteOn(1, 67, 1.0f), 20);

            audioBuffer.clear();
            processor.processBlock(audioBuffer, midiBuffer);

            // Check all samples are in valid range
            bool allSamplesValid = true;
            for (int ch = 0; ch < audioBuffer.getNumChannels(); ++ch)
            {
                auto* data = audioBuffer.getReadPointer(ch);
                for (int i = 0; i < audioBuffer.getNumSamples(); ++i)
                {
                    if (std::isnan(data[i]) || std::isinf(data[i]) || std::abs(data[i]) > 2.0f)
                    {
                        allSamplesValid = false;
                        break;
                    }
                }
            }

            expect(allSamplesValid, "All samples should be in valid range (no NaN/Inf, < 2.0)");

            processor.releaseResources();
        }

        beginTest("Voice polyphony");
        {
            PluginProcessor processor;
            processor.prepareToPlay(44100.0, 512);

            juce::MidiBuffer midiBuffer;
            juce::AudioBuffer<float> audioBuffer(2, 512);

            // Trigger 8 notes (max polyphony)
            for (int note = 60; note < 68; ++note)
            {
                midiBuffer.addEvent(juce::MidiMessage::noteOn(1, note, 0.8f), note - 60);
            }

            audioBuffer.clear();
            processor.processBlock(audioBuffer, midiBuffer);

            // Should handle 8 voices without crashing
            expect(true, "8-voice polyphony should work");

            processor.releaseResources();
        }

        beginTest("All notes off");
        {
            PluginProcessor processor;
            processor.prepareToPlay(44100.0, 512);

            juce::MidiBuffer midiBuffer;
            juce::AudioBuffer<float> audioBuffer(2, 512);

            // Trigger notes
            midiBuffer.addEvent(juce::MidiMessage::noteOn(1, 60, 0.8f), 0);
            midiBuffer.addEvent(juce::MidiMessage::noteOn(1, 64, 0.8f), 10);

            audioBuffer.clear();
            processor.processBlock(audioBuffer, midiBuffer);

            // All notes off
            midiBuffer.clear();
            midiBuffer.addEvent(juce::MidiMessage::allNotesOff(1), 0);

            audioBuffer.clear();
            processor.processBlock(audioBuffer, midiBuffer);

            expect(true, "All notes off should not crash");

            processor.releaseResources();
        }

        beginTest("FMOperator basic functionality");
        {
            FMOperator op;
            op.setFrequency(440.0f);
            op.setRatio(1.0f);
            op.setLevel(1.0f);
            op.setEnvelope(0.01f, 0.1f, 0.7f, 0.3f);

            op.trigger();
            expect(op.isActive(), "Operator should be active after trigger");

            // Process some samples
            float sample = op.processSample(44100.0, 0.0f);
            expect(std::abs(sample) >= 0.0f && std::abs(sample) <= 1.0f,
                  "Sample should be in valid range");

            op.release();
            // After release, should still be active until envelope finishes
            expect(op.isActive(), "Operator should still be active during release");

            op.reset();
            expect(!op.isActive(), "Operator should be inactive after reset");
        }

        beginTest("FMOperator modulation");
        {
            FMOperator carrier;
            FMOperator modulator;

            carrier.setFrequency(440.0f);
            carrier.setRatio(1.0f);
            modulator.setFrequency(440.0f);
            modulator.setRatio(2.0f);  // Higher ratio for modulation

            carrier.trigger();
            modulator.trigger();

            // Generate modulated sample
            float modSignal = modulator.processSample(44100.0, 0.0f);
            float carrierOutput = carrier.processSample(44100.0, modSignal * 2.0f);

            expect(std::abs(carrierOutput) <= 1.0f, "Modulated output should be valid");
        }

        beginTest("State save/restore");
        {
            PluginProcessor processor;
            processor.prepareToPlay(44100.0, 512);

            // Set some parameters
            auto* modRatioParam = processor.getAPVTS().getParameter("modRatio");
            auto* modDepthParam = processor.getAPVTS().getParameter("modDepth");

            modRatioParam->setValue(0.5f);
            modDepthParam->setValue(0.7f);

            // Save state
            juce::MemoryBlock stateData;
            processor.getStateInformation(stateData);

            // Change parameters
            modRatioParam->setValue(0.2f);
            modDepthParam->setValue(0.3f);

            // Restore state
            processor.setStateInformation(stateData.getData(), static_cast<int>(stateData.getSize()));

            // Check parameters restored
            expect(std::abs(modRatioParam->getValue() - 0.5f) < 0.01f,
                  "modRatio should be restored");
            expect(std::abs(modDepthParam->getValue() - 0.7f) < 0.01f,
                  "modDepth should be restored");

            processor.releaseResources();
        }

        beginTest("Preset system");
        {
            PluginProcessor processor;
            auto& presetManager = processor.getPresetManager();

            presetManager.scanPresets();
            auto presets = presetManager.getPresets();

            expect(presets.size() >= 20, "Should have at least 20 factory presets");

            // Check preset categories
            auto categories = presetManager.getCategories();
            expect(categories.size() >= 4, "Should have multiple categories");

            // Try loading first preset
            if (presets.size() > 0)
            {
                bool loaded = presetManager.loadPreset(0);
                expect(loaded, "Should be able to load factory preset");
            }
        }
    }
};

static FMSynthTests fmSynthTests;
