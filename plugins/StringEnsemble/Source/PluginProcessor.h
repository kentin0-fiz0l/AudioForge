#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "StringVoice.h"
#include "StringEngine.h"
#include "../../../midi/MIDILearnManager.h"
#include "../../../shared/preset/PresetManager.h"

class StringEnsembleProcessor : public juce::AudioProcessor
{
public:
    StringEnsembleProcessor();
    ~StringEnsembleProcessor() override;

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

    juce::AudioProcessorValueTreeState& getValueTreeState() { return apvts_; }

    AudioForge::MIDILearnManager& getMidiLearnManager() { return midiLearnManager_; }
    AudioForge::PresetManager& getPresetManager() { return presetManager_; }

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void updateVoiceParameters();

    juce::Synthesiser synth_;
    juce::AudioProcessorValueTreeState apvts_;

    static constexpr const char* PARAM_STRING_TYPE = "stringType";
    static constexpr const char* PARAM_BRIGHTNESS = "brightness";
    static constexpr const char* PARAM_ATTACK = "attack";
    static constexpr const char* PARAM_DECAY = "decay";
    static constexpr const char* PARAM_VIBRATO_DEPTH = "vibratoDepth";
    static constexpr const char* PARAM_VIBRATO_RATE = "vibratoRate";
    static constexpr const char* PARAM_ENSEMBLE_SIZE = "ensembleSize";

    AudioForge::MIDILearnManager midiLearnManager_;

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StringEnsembleProcessor)
};
