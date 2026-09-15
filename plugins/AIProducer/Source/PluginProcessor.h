#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "MIDIGenerator.h"
#include "AIClient.h"

class AIProducerProcessor : public juce::AudioProcessor {
public:
    AIProducerProcessor();
    ~AIProducerProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "AI Producer"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return true; }  // This plugin generates MIDI!
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Public API for UI
    void generateTrack(const juce::String& prompt);
    bool isGenerating() const { return isGenerating_; }
    juce::String getStatus() const { return statusMessage_; }
    float getProgress() const { return progress_; }

private:
    void onTrackGenerated(TrackStructure structure);
    void onGenerationError(std::string error);

    std::unique_ptr<AIClient> aiClient_;
    std::unique_ptr<MIDIGenerator> midiGenerator_;

    bool isGenerating_ = false;
    juce::String statusMessage_ = "Ready";
    float progress_ = 0.0f;

    double lastSampleRate_ = 44100.0;
    double currentBPM_ = 128.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AIProducerProcessor)
};
