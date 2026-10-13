/*
  PluginProcessor.h - MelodyMaker Audio Processor
*/

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "MelodyEngine.h"

class MelodyMakerProcessor : public juce::AudioProcessor
{
public:
    MelodyMakerProcessor();
    ~MelodyMakerProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "MelodyMaker"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    bool isSynth() const { return true; }

    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getValueTreeState() { return apvts_; }

    // MelodyMaker specific
    MelodyEngine& getMelodyEngine() { return melodyEngine_; }
    // For the message thread; the audio thread reads the melody under melodyLock_
    const std::vector<MelodyEngine::Note>& getCurrentMelody() const { return currentMelody_; }

    void generateNewMelody(const std::string& style, int numBars = 4);
    const std::string& getStyle() const { return style_; }
    void setMelodyEnabled(bool enabled) { melodyEnabled_ = enabled; }
    bool isMelodyEnabled() const { return melodyEnabled_; }

    void setKey(int rootNote, bool isMajor) { keyRoot_ = rootNote; keyIsMajor_ = isMajor; }
    int getKeyRoot() const { return keyRoot_; }
    bool getKeyIsMajor() const { return keyIsMajor_; }

    // MIDI export
    void exportToMidiFile(const juce::File& file);

    // Synth parameters, read by the voices
    int getWaveform() const { return static_cast<int>(waveform_->load()); }
    float getAttack() const { return attack_->load(); }
    float getDecay() const { return decay_->load(); }
    float getSustain() const { return sustain_->load(); }
    float getRelease() const { return release_->load(); }

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void setMelody(std::vector<MelodyEngine::Note> melody);

    juce::AudioProcessorValueTreeState apvts_;

    MelodyEngine melodyEngine_;
    std::vector<MelodyEngine::Note> currentMelody_;
    juce::SpinLock melodyLock_;
    // Held by everything outside the audio thread that replaces the melody or reads it (saving,
    // export): a host can save from a background thread while the melody is being replaced
    juce::CriticalSection melodyEditLock_;

    double sampleRate_ = 44100.0;
    bool melodyEnabled_ = true;
    int melodyLengthInBeats_ = 16; // 4 bars at 4/4

    std::string style_ = "Pop";
    int keyRoot_ = 60;       // C4
    bool keyIsMajor_ = true; // Major scale

    // Parameters
    static constexpr float maxDelaySeconds = 1.0f;
    std::atomic<float>* waveform_;  // 0=Sine, 1=Saw, 2=Square, 3=Triangle
    std::atomic<float>* attack_;
    std::atomic<float>* decay_;
    std::atomic<float>* sustain_;
    std::atomic<float>* release_;
    std::atomic<float>* reverbMix_;
    std::atomic<float>* delayTime_;
    std::atomic<float>* delayFeedback_;
    std::atomic<float>* delayMix_;
    std::atomic<float>* filterCutoff_;
    std::atomic<float>* filterResonance_;

    // Synthesizer
    juce::Synthesiser synth_;
    juce::MidiBuffer internalMidiBuffer_;

    // Effects processors
    juce::dsp::Reverb reverb_;
    juce::dsp::DelayLine<float> delayLine_{48000};
    juce::dsp::StateVariableTPTFilter<float> filter_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MelodyMakerProcessor)
};
