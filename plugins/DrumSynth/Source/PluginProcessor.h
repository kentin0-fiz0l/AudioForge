#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <presets/PresetManager.h>
#include "KickModule.h"
#include "SnareModule.h"
#include "HiHatModule.h"

/**
 * DrumSynth Processor
 *
 * 3-module drum synthesizer:
 * - Kick (C1 / MIDI 36)
 * - Snare (D1 / MIDI 38)
 * - Hi-Hat (F#1 / MIDI 42)
 *
 * Each drum is monophonic - retriggering restarts the sound.
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

    // MIDI note numbers for drum triggers
    static constexpr int KICK_NOTE = 36;   // C1
    static constexpr int SNARE_NOTE = 38;  // D1
    static constexpr int HIHAT_NOTE = 42;  // F#1

private:
    juce::AudioProcessorValueTreeState apvts;
    AudioForge::PresetManager presetManager;

    KickModule kickModule;
    SnareModule snareModule;
    HiHatModule hihatModule;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
};
