#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>
#include "../../../midi/MIDILearnManager.h"
#include "../../../shared/preset/PresetManager.h"

/**
 * Convolution Reverb
 *
 * Plays the input through a recorded space: an impulse response, mono or
 * stereo, from a WAV, AIFF or FLAC file. The wet signal goes through a
 * pre-delay and a low and high cut before it's mixed with the dry.
 *
 * The impulse response is part of the saved state, as the root's "ir"
 * attribute: <PARAMETERS ir="/path/hall.wav"/> as a state loads one.
 */
class ConvolutionReverbProcessor : public juce::AudioProcessor {
public:
    ConvolutionReverbProcessor();
    ~ConvolutionReverbProcessor() override;
    void prepareToPlay(double, int) override;
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "ConvolutionReverb"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override;
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;
    juce::AudioProcessorParameter* getBypassParameter() const override;
    juce::AudioProcessorValueTreeState& getValueTreeState() { return apvts_; }
    AudioForge::MIDILearnManager& getMidiLearnManager() { return midiLearnManager_; }
    AudioForge::PresetManager& getPresetManager() { return presetManager_; }

    /**
     * Load an impulse response. The convolution takes it up at the next
     * prepareToPlay, or crossfades to it while playing.
     */
    bool loadImpulseResponse(const juce::File& file, juce::String& error);
    const juce::File& getImpulseResponseFile() const { return irFile_; }
    double getImpulseResponseSeconds() const { return irSeconds_.load(); }

    static constexpr double maxImpulseSeconds = 20.0;
    static constexpr float maxPreDelayMs = 200.0f;

private:
    juce::AudioProcessorValueTreeState apvts_;
    juce::dsp::Convolution convolution_;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::None> preDelay_;
    juce::dsp::StateVariableTPTFilter<float> lowCut_, highCut_;
    juce::AudioBuffer<float> dry_;
    juce::SmoothedValue<float> mix_, outputGain_;
    double sampleRate_ = 44100.0;

    juce::File irFile_;
    std::atomic<double> irSeconds_ { 0.0 };  // 0 until one is loaded

    std::atomic<float>* mixParam_ = nullptr;
    std::atomic<float>* preDelayParam_ = nullptr;
    std::atomic<float>* lowCutParam_ = nullptr;
    std::atomic<float>* highCutParam_ = nullptr;
    std::atomic<float>* outputParam_ = nullptr;
    std::atomic<float>* bypassParam_ = nullptr;

    AudioForge::MIDILearnManager midiLearnManager_;
    AudioForge::PresetManager presetManager_;

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConvolutionReverbProcessor)
};
