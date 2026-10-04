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

    // MelodyMaker specific
    MelodyEngine& getMelodyEngine() { return melodyEngine_; }
    std::vector<MelodyEngine::Note>& getCurrentMelody() { return currentMelody_; }

    void generateNewMelody(const std::string& style, int numBars = 4);
    void setMelodyEnabled(bool enabled) { melodyEnabled_ = enabled; }
    bool isMelodyEnabled() const { return melodyEnabled_; }

    void setKey(int rootNote, bool isMajor) { keyRoot_ = rootNote; keyIsMajor_ = isMajor; }
    int getKeyRoot() const { return keyRoot_; }
    bool getKeyIsMajor() const { return keyIsMajor_; }

    // MIDI export
    void exportToMidiFile(const juce::File& file);

    // Synth parameters
    void setWaveform(int waveform);
    int getWaveform() const { return waveform_; }

    void setAttack(float attack) { attack_ = attack; }
    void setDecay(float decay) { decay_ = decay; }
    void setSustain(float sustain) { sustain_ = sustain; }
    void setRelease(float release) { release_ = release; }

    float getAttack() const { return attack_; }
    float getDecay() const { return decay_; }
    float getSustain() const { return sustain_; }
    float getRelease() const { return release_; }

    // Effects parameters
    void setReverbMix(float mix) { reverbMix_ = mix; }
    float getReverbMix() const { return reverbMix_; }

    void setDelayTime(float time) { delayTime_ = time; }
    float getDelayTime() const { return delayTime_; }

    void setDelayFeedback(float feedback) { delayFeedback_ = feedback; }
    float getDelayFeedback() const { return delayFeedback_; }

    void setDelayMix(float mix) { delayMix_ = mix; }
    float getDelayMix() const { return delayMix_; }

    void setFilterCutoff(float cutoff) { filterCutoff_ = cutoff; }
    float getFilterCutoff() const { return filterCutoff_; }

    void setFilterResonance(float res) { filterResonance_ = res; }
    float getFilterResonance() const { return filterResonance_; }

private:
    MelodyEngine melodyEngine_;
    std::vector<MelodyEngine::Note> currentMelody_;

    double sampleRate_ = 44100.0;
    bool melodyEnabled_ = true;
    int melodyLengthInBeats_ = 16; // 4 bars at 4/4

    int keyRoot_ = 60;       // C4
    bool keyIsMajor_ = true; // Major scale

    // Synth parameters
    int waveform_ = 0;       // 0=Sine, 1=Saw, 2=Square, 3=Triangle
    float attack_ = 0.01f;
    float decay_ = 0.1f;
    float sustain_ = 0.7f;
    float release_ = 0.3f;

    // Effects parameters
    float reverbMix_ = 0.0f;
    float delayTime_ = 0.25f;
    float delayFeedback_ = 0.3f;
    float delayMix_ = 0.0f;
    float filterCutoff_ = 20000.0f;
    float filterResonance_ = 0.7f;

    // Synthesizer
    juce::Synthesiser synth_;
    juce::MidiBuffer internalMidiBuffer_;

    // Effects processors
    juce::dsp::Reverb reverb_;
    juce::dsp::DelayLine<float> delayLine_{48000};
    juce::dsp::StateVariableTPTFilter<float> filter_;
    juce::AudioBuffer<float> delayBuffer_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MelodyMakerProcessor)
};
