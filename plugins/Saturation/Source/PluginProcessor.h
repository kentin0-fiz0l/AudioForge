#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <dsp/AudioForgeDSP.h>

class SaturationAudioProcessor : public juce::AudioProcessor
{
public:
    SaturationAudioProcessor();
    ~SaturationAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Parameters
    juce::AudioParameterFloat* driveParam;
    juce::AudioParameterChoice* modeParam;
    juce::AudioParameterFloat* toneLowParam;
    juce::AudioParameterFloat* toneHighParam;
    juce::AudioParameterFloat* mixParam;
    juce::AudioParameterChoice* oversamplingParam;

    // Metering
    AudioForge::DSP::ThreadSafeMeter inputMeter;
    AudioForge::DSP::ThreadSafeMeter outputMeter;

    enum class SaturationMode
    {
        Tape = 0,
        Tube,
        Transistor,
        Transformer,
        Diode,
        NumModes
    };

private:
    // Tone filters (low and high shelves)
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                    juce::dsp::IIR::Coefficients<float>> lowShelf;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                    juce::dsp::IIR::Coefficients<float>> highShelf;

    // DC blocker
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                    juce::dsp::IIR::Coefficients<float>> dcBlocker;

    // Oversampling
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;
    int currentOversamplingFactor = 1;

    double currentSampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SaturationAudioProcessor)
};
