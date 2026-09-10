#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "ShimmerEngine.h"
class ShimmerProcessor : public juce::AudioProcessor {
public:
    ShimmerProcessor(); ~ShimmerProcessor() override;
    void prepareToPlay(double, int) override;
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "ShimmerReverb"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 5.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;
    juce::AudioProcessorValueTreeState& getValueTreeState() { return apvts_; }
private:
    juce::AudioProcessorValueTreeState apvts_;
    ShimmerEngine engine_;
    void updateEngineParameters();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ShimmerProcessor)
};
