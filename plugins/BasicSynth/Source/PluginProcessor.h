#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <dsp/AudioForgeDSP.h>
#include "Voice.h"

/**
 * BasicSynth Audio Processor
 *
 * A simple subtractive synthesizer for learning synthesis fundamentals.
 *
 * Features:
 * - 3 oscillator waveforms (sine, sawtooth, square)
 * - ADSR envelope (attack, decay, sustain, release)
 * - 4-8 voice polyphony with voice stealing
 * - Low-pass filter with resonance
 * - Master volume with output metering
 */
class BasicSynthProcessor : public juce::AudioProcessor
{
public:
    BasicSynthProcessor();
    ~BasicSynthProcessor() override;

    //==============================================================================
    // Audio processing
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    // Editor
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    //==============================================================================
    // Plugin info
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    //==============================================================================
    // Programs
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    //==============================================================================
    // State
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    // Parameter access for editor
    float getCurrentLevel() const { return levelMeter.getLevel(); }

    // Parameter IDs
    static constexpr const char* PARAM_WAVEFORM = "waveform";
    static constexpr const char* PARAM_VOLUME = "volume";
    static constexpr const char* PARAM_ATTACK = "attack";
    static constexpr const char* PARAM_DECAY = "decay";
    static constexpr const char* PARAM_SUSTAIN = "sustain";
    static constexpr const char* PARAM_RELEASE = "release";
    static constexpr const char* PARAM_FILTER_CUTOFF = "filterCutoff";
    static constexpr const char* PARAM_FILTER_RESONANCE = "filterResonance";
    static constexpr const char* PARAM_FILTER_TYPE = "filterType";

private:
    //==============================================================================
    // Parameters
    juce::AudioParameterChoice* waveformParam;    // Sine, Saw, Square
    juce::AudioParameterFloat* volumeParam;       // Master volume (0.0 to 1.0)
    juce::AudioParameterFloat* attackParam;       // ADSR Attack (ms)
    juce::AudioParameterFloat* decayParam;        // ADSR Decay (ms)
    juce::AudioParameterFloat* sustainParam;      // ADSR Sustain (0.0 to 1.0)
    juce::AudioParameterFloat* releaseParam;      // ADSR Release (ms)
    juce::AudioParameterFloat* filterCutoffParam; // Filter cutoff (Hz)
    juce::AudioParameterFloat* filterResonanceParam; // Filter resonance (Q)
    juce::AudioParameterChoice* filterTypeParam;  // LP, HP, BP, Notch

    // Voice management
    static constexpr int MAX_VOICES = 8;
    std::array<SynthVoice, MAX_VOICES> voices;
    int nextVoiceIndex = 0;

    // Smoothed master volume
    AudioForge::DSP::ParameterSmoothing<float> smoothedVolume;

    // Level metering
    AudioForge::DSP::ThreadSafeMeter levelMeter;

    // Helper methods
    SynthVoice* findFreeVoice();
    SynthVoice* findVoiceForNote(int midiNoteNumber);
    SynthVoice* stealVoice();
    void handleNoteOn(int midiNoteNumber, float velocity);
    void handleNoteOff(int midiNoteNumber);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BasicSynthProcessor)
};
