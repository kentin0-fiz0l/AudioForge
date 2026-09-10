#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "../../../midi/MIDILearnManager.h"

class ChorusProcessor : public juce::AudioProcessor {
public:
    ChorusProcessor(); ~ChorusProcessor() override;
    void prepareToPlay(double, int) override;
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "StereoChorus"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.1; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;
    juce::AudioProcessorValueTreeState& getValueTreeState() { return apvts_; }
    AudioForge::MIDILearnManager& getMidiLearnManager() { return midiLearnManager_; }

private:
    juce::AudioProcessorValueTreeState apvts_;
    juce::dsp::Chorus<float> chorus_;
    void updateChorusParameters();
    AudioForge::MIDILearnManager midiLearnManager_;

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChorusProcessor)
};
