/*
  ==============================================================================

    PluginProcessor.h
    Created: 1 Sep 2026
    Author:  AudioForge Team

    Main plugin processor for Limiter plugin.

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "LimiterEngine.h"

//==============================================================================
/**
 * @brief AudioForge Limiter - Professional True Peak Limiter
 *
 * Features:
 * - True peak limiting (ITU-R BS.1770-4 compliant)
 * - Lookahead buffer (0-10ms)
 * - Auto makeup gain
 * - Professional metering
 */
class LimiterAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    LimiterAudioProcessor();
    ~LimiterAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    // Parameter access (for UI)

    juce::AudioProcessorValueTreeState& getParameters() { return parameters; }
    audioforge::LimiterEngine& getLimiterEngine() { return limiterEngine; }

private:
    //==============================================================================
    // Parameters
    juce::AudioProcessorValueTreeState parameters;

    std::atomic<float>* ceilingParam = nullptr;
    std::atomic<float>* thresholdParam = nullptr;
    std::atomic<float>* releaseParam = nullptr;
    std::atomic<float>* lookaheadParam = nullptr;
    std::atomic<float>* autoMakeupParam = nullptr;
    std::atomic<float>* outputTrimParam = nullptr;

    // DSP Engine
    audioforge::LimiterEngine limiterEngine;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LimiterAudioProcessor)
};
