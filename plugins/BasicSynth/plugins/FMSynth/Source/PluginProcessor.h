#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <synth/VoiceManager.h>
#include <presets/PresetManager.h>

/**
 * Voice class for this synthesizer.
 * Implements the per-voice DSP and state.
 */
class Voice
{
public:
    Voice();

    void noteOn(int midiNote, float velocity, double sampleRate);
    void noteOff();
    bool isActive() const { return active; }
    int getMidiNote() const { return currentNote; }
    float getLevel() const { return level; }
    void reset();

    float processSample(double sampleRate);

private:
    bool active = false;
    int currentNote = -1;
    float velocity = 0.0f;
    float level = 0.0f;

    // TODO: Add synthesis components (oscillators, envelopes, filters, etc.)
};

/**
 * Plugin Processor
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

    // Parameters
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

private:
    juce::AudioProcessorValueTreeState apvts;
    AudioForge::VoiceManager<Voice, 8> voiceManager;
    AudioForge::PresetManager presetManager;

    void createParameters();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
};
