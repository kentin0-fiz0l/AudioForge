#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "MonosynthVoice.h"

class ClassicMonosynthProcessor : public juce::AudioProcessor
{
public:
    ClassicMonosynthProcessor();
    ~ClassicMonosynthProcessor() override;

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

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void updateVoiceParameters();

    juce::Synthesiser synth_;
    juce::AudioProcessorValueTreeState apvts_;

    // Oscillators
    static constexpr const char* PARAM_OSC1_LEVEL = "osc1Level";
    static constexpr const char* PARAM_OSC2_LEVEL = "osc2Level";
    static constexpr const char* PARAM_OSC3_LEVEL = "osc3Level";
    static constexpr const char* PARAM_OSC2_DETUNE = "osc2Detune";
    static constexpr const char* PARAM_OSC3_DETUNE = "osc3Detune";
    static constexpr const char* PARAM_PULSE_WIDTH = "pulseWidth";
    static constexpr const char* PARAM_OSC_SYNC = "oscSync";

    // Filter
    static constexpr const char* PARAM_FILTER_CUTOFF = "filterCutoff";
    static constexpr const char* PARAM_FILTER_RESONANCE = "filterResonance";
    static constexpr const char* PARAM_FILTER_ENV_AMOUNT = "filterEnvAmount";

    // Filter Envelope
    static constexpr const char* PARAM_FILTER_ATTACK = "filterAttack";
    static constexpr const char* PARAM_FILTER_DECAY = "filterDecay";
    static constexpr const char* PARAM_FILTER_SUSTAIN = "filterSustain";
    static constexpr const char* PARAM_FILTER_RELEASE = "filterRelease";

    // Amp Envelope
    static constexpr const char* PARAM_AMP_ATTACK = "ampAttack";
    static constexpr const char* PARAM_AMP_DECAY = "ampDecay";
    static constexpr const char* PARAM_AMP_SUSTAIN = "ampSustain";
    static constexpr const char* PARAM_AMP_RELEASE = "ampRelease";

    // LFOs
    static constexpr const char* PARAM_LFO1_RATE = "lfo1Rate";
    static constexpr const char* PARAM_LFO1_AMOUNT = "lfo1Amount";
    static constexpr const char* PARAM_LFO2_RATE = "lfo2Rate";
    static constexpr const char* PARAM_LFO2_AMOUNT = "lfo2Amount";

    // Glide
    static constexpr const char* PARAM_GLIDE_TIME = "glideTime";

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClassicMonosynthProcessor)
};
