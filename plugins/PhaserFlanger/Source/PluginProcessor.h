#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "ModulationEngine.h"
#include "../../../midi/MIDILearnManager.h"
#include "../../../shared/preset/PresetManager.h"

class PhaserFlangerProcessor : public juce::AudioProcessor {
public:
    PhaserFlangerProcessor();
    ~PhaserFlangerProcessor() override;
    void prepareToPlay(double, int) override;
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "PhaserFlanger"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 1.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;
    juce::AudioProcessorValueTreeState& getValueTreeState() { return apvts_; }
    AudioForge::MIDILearnManager& getMidiLearnManager() { return midiLearnManager_; }
    AudioForge::PresetManager& getPresetManager() { return presetManager_; }

private:
    juce::AudioProcessorValueTreeState apvts_;
    ModulationEngine engine_;
    void updateEngineParameters();
    AudioForge::MIDILearnManager midiLearnManager_;
    AudioForge::PresetManager presetManager_;

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PhaserFlangerProcessor)
};
