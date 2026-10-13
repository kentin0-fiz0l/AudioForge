#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "MIDIGenerator.h"
#include "Styles.h"

class AIProducerProcessor : public juce::AudioProcessor,
                            private juce::Timer {
public:
    AIProducerProcessor();
    ~AIProducerProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Track Sketch"; }
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

    juce::AudioProcessorValueTreeState& getValueTreeState() { return apvts_; }

    // Rebuilds the track if the controls have changed since it was last
    // built. Called from a timer on the message thread, and after loading.
    void regenerateIfChanged();

    // Moves the Variation control on, so the same settings give a new track
    void nextVariation();

    // What is playing, e.g. "House in A minor, 72 bars at 124 BPM"
    juce::String getDescription() const;

private:
    void timerCallback() override { regenerateIfChanged(); }
    Styles::Settings currentSettings() const;

    // Hands a newly generated sequence to the audio thread (message thread only)
    void publishSequence(std::unique_ptr<const MIDIGenerator::Sequence> next);

    juce::AudioProcessorValueTreeState apvts_;

    // Guards building a track, which the timer and setStateInformation() can
    // both start; never taken on the audio thread
    mutable juce::CriticalSection buildLock_;
    Styles::Settings builtSettings_;
    bool hasBuilt_ = false;
    juce::String description_;

    double lastSampleRate_ = 44100.0;

    // The sequence being played. Written on the message thread by
    // publishSequence() and read on the audio thread in processBlock().
    juce::SpinLock sequenceLock_;
    std::unique_ptr<const MIDIGenerator::Sequence> sequence_;

    // Playback state, touched only on the audio thread
    MIDIGenerator::HeldNotes heldNotes_;
    std::uint64_t playingSequenceId_ = 0;   // 0 = nothing played yet
    bool wasPlaying_ = false;
    double nextBlockStartPpq_ = 0.0;        // Where the next block starts if the transport runs on
    double renderedUpToPpq_ = 0.0;          // Events before this have already been sent

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AIProducerProcessor)
};
