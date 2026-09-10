#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "ShakuhachiVoice.h"
#include "../../../midi/MIDILearnManager.h"

class ShakuhachiProcessor : public juce::AudioProcessor
{
public:
    ShakuhachiProcessor();
    ~ShakuhachiProcessor() override;

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

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void updateVoiceParameters();

    juce::Synthesiser synth_;
    juce::AudioProcessorValueTreeState apvts_;

    static constexpr const char* PARAM_BREATH_PRESSURE = "breathPressure";
    static constexpr const char* PARAM_EMBOUCHURE = "embouchure";
    static constexpr const char* PARAM_AIR_NOISE = "airNoise";
    static constexpr const char* PARAM_ATTACK = "attack";
    static constexpr const char* PARAM_RELEASE = "release";
    static constexpr const char* PARAM_VIBRATO_RATE = "vibratoRate";
    static constexpr const char* PARAM_VIBRATO_DEPTH = "vibratoDepth";
    static constexpr const char* PARAM_BRIGHTNESS = "brightness";

    AudioForge::MIDILearnManager midiLearnManager_;

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ShakuhachiProcessor)
};
