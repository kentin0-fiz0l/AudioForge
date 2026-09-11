#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "../../../midi/MIDILearnManager.h"
#include "../../../shared/preset/PresetManager.h"

class XYPadControllerProcessor : public juce::AudioProcessor {
public:
    XYPadControllerProcessor();
    ~XYPadControllerProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "XY Pad Controller"; }
    bool acceptsMidi() const override { return false; }  // Generator, not processor
    bool producesMidi() const override { return true; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Default"; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts_; }
    AudioForge::MIDILearnManager& getMidiLearnManager() { return midiLearnManager_; }
    AudioForge::PresetManager& getPresetManager() { return presetManager_; }

    // Called from UI when pad position changes
    void queueCCMessage(int cc, float value);

private:
    juce::AudioProcessorValueTreeState apvts_;
    AudioForge::MIDILearnManager midiLearnManager_;
    AudioForge::PresetManager presetManager_;

    // Pending CC messages from UI
    struct PendingCC {
        int cc;
        int value;  // 0-127
    };
    std::vector<PendingCC> pendingCCs_;
    juce::CriticalSection ccLock_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(XYPadControllerProcessor)
};
