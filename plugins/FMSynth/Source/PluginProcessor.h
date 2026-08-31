#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <synth/VoiceManager.h>
#include <presets/PresetManager.h>
#include <dsp/WaveformGenerators.h>
#include "FMOperator.h"

/**
 * FM Synthesis Voice
 * 2-operator FM: Modulator → Carrier → Output
 */
class Voice
{
public:
    Voice();

    void noteOn(int midiNote, float velocity, double sampleRate);
    void noteOff();
    bool isActive() const;
    int getMidiNote() const { return currentNote; }
    float getLevel() const;
    void reset();

    float processSample(double sampleRate, float modulatorRatio, float modulatorDepth);

    void updateEnvelopes(float carrierAttack, float carrierDecay, float carrierSustain, float carrierRelease,
                        float modAttack, float modDecay, float modSustain, float modRelease);

private:
    bool active = false;
    int currentNote = -1;
    float velocity = 0.0f;

    FMOperator modulator;
    FMOperator carrier;
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

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
};
