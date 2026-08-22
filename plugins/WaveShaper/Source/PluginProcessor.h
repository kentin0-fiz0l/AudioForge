#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <dsp/AudioForgeDSP.h>

class WaveShaperAudioProcessor : public juce::AudioProcessor
{
public:
    WaveShaperAudioProcessor();
    ~WaveShaperAudioProcessor() override;

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
    juce::AudioParameterFloat* mixParam;
    juce::AudioParameterFloat* toneParam;
    juce::AudioParameterChoice* shapeParam;
    juce::AudioParameterFloat* outputGainParam;

    // Metering
    AudioForge::DSP::ThreadSafeMeter inputMeter;
    AudioForge::DSP::ThreadSafeMeter outputMeter;

    enum class ShapeMode
    {
        SoftClip = 0,
        HardClip,
        Tanh,
        Cubic,
        Asymmetric,
        NumModes
    };

private:
    // Tone filter (low-pass)
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, 
                                    juce::dsp::IIR::Coefficients<float>> toneFilter;
    
    // DC blocker
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                    juce::dsp::IIR::Coefficients<float>> dcBlocker;

    double currentSampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveShaperAudioProcessor)
};
