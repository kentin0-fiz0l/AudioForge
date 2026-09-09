#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "BrassVoice.h"
#include "BrassEngine.h"

class BrassSectionProcessor : public juce::AudioProcessor
{
public:
    BrassSectionProcessor();
    ~BrassSectionProcessor() override;

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

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void updateVoiceParameters();

    juce::Synthesiser synth_;
    juce::AudioProcessorValueTreeState apvts_;

    static constexpr const char* PARAM_INSTRUMENT = "instrument";
    static constexpr const char* PARAM_ARTICULATION = "articulation";
    static constexpr const char* PARAM_BRIGHTNESS = "brightness";
    static constexpr const char* PARAM_BREATH_NOISE = "breathNoise";
    static constexpr const char* PARAM_VIBRATO_DEPTH = "vibratoDepth";
    static constexpr const char* PARAM_VIBRATO_RATE = "vibratoRate";
    static constexpr const char* PARAM_SECTION_SIZE = "sectionSize";

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BrassSectionProcessor)
};
