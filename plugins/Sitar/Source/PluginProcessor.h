#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "SitarVoice.h"
#include "../../../midi/MIDILearnManager.h"
#include "../../../shared/preset/PresetManager.h"

class SitarProcessor : public juce::AudioProcessor
{
public:
    SitarProcessor();
    ~SitarProcessor() override;

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
    void setCurrentProgram(int) override;
    const juce::String getProgramName(int) override;
    void changeProgramName(int, const juce::String&) override;

    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    juce::AudioProcessorValueTreeState& getValueTreeState() { return apvts_; }

    AudioForge::MIDILearnManager& getMidiLearnManager() { return midiLearnManager_; }
    AudioForge::PresetManager& getPresetManager() { return presetManager_; }

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void updateVoiceParameters();

    juce::Synthesiser synth_;
    juce::AudioProcessorValueTreeState apvts_;

    static constexpr const char* PARAM_RAGA_SCALE = "ragaScale";
    static constexpr const char* PARAM_JAWARI = "jawari";
    static constexpr const char* PARAM_SYMPATHETIC = "sympathetic";
    static constexpr const char* PARAM_ATTACK = "attack";
    static constexpr const char* PARAM_DECAY = "decay";
    static constexpr const char* PARAM_PITCH_BEND = "pitchBend";
    static constexpr const char* PARAM_VIBRATO_RATE = "vibratoRate";
    static constexpr const char* PARAM_VIBRATO_DEPTH = "vibratoDepth";
    static constexpr const char* PARAM_DRONE_LEVEL = "droneLevel";

    AudioForge::MIDILearnManager midiLearnManager_;

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SitarProcessor)
};
