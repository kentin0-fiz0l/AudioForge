#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PolysynthVoice.h"
#include "../../../midi/MIDILearnManager.h"
#include "../../../shared/preset/PresetManager.h"

class PolysynthProcessor : public juce::AudioProcessor
{
public:
    PolysynthProcessor();
    ~PolysynthProcessor() override;

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

    static constexpr const char* PARAM_OSC_MIX = "oscMix";
    static constexpr const char* PARAM_DETUNE = "detune";
    static constexpr const char* PARAM_UNISON_VOICES = "unisonVoices";
    static constexpr const char* PARAM_UNISON_DETUNE = "unisonDetune";
    static constexpr const char* PARAM_FILTER_CUTOFF = "filterCutoff";
    static constexpr const char* PARAM_FILTER_RESONANCE = "filterResonance";
    static constexpr const char* PARAM_FILTER_MIX = "filterMix";
    static constexpr const char* PARAM_ATTACK = "attack";
    static constexpr const char* PARAM_DECAY = "decay";
    static constexpr const char* PARAM_SUSTAIN = "sustain";
    static constexpr const char* PARAM_RELEASE = "release";
    static constexpr const char* PARAM_CHORUS_DEPTH = "chorusDepth";
    static constexpr const char* PARAM_CHORUS_RATE = "chorusRate";

    AudioForge::MIDILearnManager midiLearnManager_;
    AudioForge::PresetManager presetManager_;

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PolysynthProcessor)
};
