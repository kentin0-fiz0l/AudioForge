/*
  PluginProcessor.h - BassLine Audio Processor
*/

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "BassEngine.h"

class BassLineProcessor : public juce::AudioProcessor
{
public:
    BassLineProcessor();
    ~BassLineProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "BassLine"; }
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

    // BassLine specific
    BassEngine& getBassEngine() { return bassEngine_; }
    std::vector<BassEngine::Note>& getCurrentBassLine() { return currentBassLine_; }

    void generateNewBassLine(const std::string& style, int numBars = 4);
    void setBassEnabled(bool enabled) { bassEnabled_ = enabled; }
    bool isBassEnabled() const { return bassEnabled_; }

    void setKey(int rootNote, bool isMajor) { keyRoot_ = rootNote; keyIsMajor_ = isMajor; }
    int getKeyRoot() const { return keyRoot_; }
    bool getKeyIsMajor() const { return keyIsMajor_; }

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

    void setFilterCutoff(float cutoff) { filterCutoff_ = cutoff; }
    float getFilterCutoff() const { return filterCutoff_; }

    void setOctave(int octave) { octave_ = octave; }
    int getOctave() const { return octave_; }

    // MIDI export
    void exportToMidiFile(const juce::File& file);

    // Effects parameters
    void setReverbMix(float mix) { reverbMix_ = mix; }
    float getReverbMix() const { return reverbMix_; }
    void setDelayMix(float mix) { delayMix_ = mix; }
    float getDelayMix() const { return delayMix_; }

private:
    BassEngine bassEngine_;
    std::vector<BassEngine::Note> currentBassLine_;

    double sampleRate_ = 44100.0;
    bool bassEnabled_ = true;
    int bassLengthInBeats_ = 16; // 4 bars at 4/4

    int keyRoot_ = 60;       // C4
    bool keyIsMajor_ = true; // Major scale
    int octave_ = 2;         // C2 (bass register)

    // Synth parameters
    int waveform_ = 0;        // 0=Sine, 1=Saw, 2=Square, 3=Triangle
    float attack_ = 0.005f;   // Fast attack for bass
    float decay_ = 0.1f;
    float sustain_ = 0.8f;
    float release_ = 0.2f;
    float filterCutoff_ = 800.0f;  // Low-pass filter cutoff

    // Effects parameters
    float reverbMix_ = 0.0f;
    float delayMix_ = 0.0f;

    // Synthesizer for playing notes
    juce::Synthesiser synth_;
    juce::MidiBuffer internalMidiBuffer_;

    // Effects processors
    juce::dsp::Reverb reverb_;
    juce::dsp::DelayLine<float> delayLine_{48000};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BassLineProcessor)
};
