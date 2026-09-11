#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "ElectricPianoVoice.h"
#include "ElectricPianoEngine.h"
#include "../../../midi/MIDILearnManager.h"
#include "../../../shared/preset/PresetManager.h"

/**
 * ElectricPianoProcessor - Main audio plugin processor
 *
 * Manages the polyphonic synthesizer with 16 voices of electric piano
 */
class ElectricPianoProcessor : public juce::AudioProcessor
{
public:
    ElectricPianoProcessor();
    ~ElectricPianoProcessor() override;

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

    // Parameter access
    juce::AudioProcessorValueTreeState& getValueTreeState() { return apvts_; }

    AudioForge::MIDILearnManager& getMidiLearnManager() { return midiLearnManager_; }
    AudioForge::PresetManager& getPresetManager() { return presetManager_; }

private:
    // Create parameter layout
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Update voice parameters from APVTS
    void updateVoiceParameters();

    // Synthesizer
    juce::Synthesiser synth_;

    // Parameters
    juce::AudioProcessorValueTreeState apvts_;

    // Parameter IDs
    static constexpr const char* PARAM_MODEL = "model";
    static constexpr const char* PARAM_TONE = "tone";
    static constexpr const char* PARAM_DECAY = "decay";
    static constexpr const char* PARAM_MECHANICAL_NOISE = "mechanicalNoise";
    static constexpr const char* PARAM_VELOCITY_CURVE = "velocityCurve";
    static constexpr const char* PARAM_TREMOLO_DEPTH = "tremoloDepth";
    static constexpr const char* PARAM_TREMOLO_RATE = "tremoloRate";
    static constexpr const char* PARAM_CHORUS_DEPTH = "chorusDepth";
    static constexpr const char* PARAM_REVERB_MIX = "reverbMix";

    AudioForge::MIDILearnManager midiLearnManager_;
    AudioForge::PresetManager presetManager_;

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ElectricPianoProcessor)
};
