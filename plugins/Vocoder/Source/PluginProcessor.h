#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "VocoderEngine.h"
#include "../../../midi/MIDILearnManager.h"

class VocoderProcessor : public juce::AudioProcessor {
public:
    VocoderProcessor();
    ~VocoderProcessor() override;
    void prepareToPlay(double, int) override;
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "Vocoder"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;
    juce::AudioProcessorValueTreeState& getValueTreeState() { return apvts_; }
    VocoderEngine& getEngine() { return engine_; }
    AudioForge::MIDILearnManager& getMidiLearnManager() { return midiLearnManager_; }
private:
    juce::AudioProcessorValueTreeState apvts_;
    VocoderEngine engine_;
    AudioForge::MIDILearnManager midiLearnManager_;
    void updateEngineParameters();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VocoderProcessor)
};
