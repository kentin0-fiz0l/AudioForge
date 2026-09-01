#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <presets/PresetManager.h>
#include <synth/VoiceManager.h>
#include "Voice.h"

/**
 * OrganEmulator Processor
 *
 * Hammond-style organ with:
 * - 9 drawbars for harmonic control
 * - Leslie rotating speaker simulator
 * - Percussion click envelope
 * - 8-voice polyphony
 */
class PluginProcessor : public juce::AudioProcessor
{
public:
    PluginProcessor();
    ~PluginProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    AudioForge::PresetManager& getPresetManager() { return presetManager; }

private:
    juce::AudioProcessorValueTreeState apvts;
    AudioForge::PresetManager presetManager;

    AudioForge::VoiceManager<Voice, 8> voiceManager;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
};
