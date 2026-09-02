/*
  ==============================================================================

    PluginProcessor.h
    Created: 1 Sep 2026
    Author:  AudioForge Team

    Main plugin processor for Gate plugin.

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "GateEngine.h"

//==============================================================================
/**
 * @brief AudioForge Gate - Professional Noise Gate & Expander
 *
 * Features:
 * - Hard gate and soft expander modes
 * - Sidechain input with frequency-selective filtering
 * - Lookahead buffer (0-10ms)
 * - RMS/Peak detection modes
 * - Professional metering
 */
class GateAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    GateAudioProcessor();
    ~GateAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
   #endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    // Parameter access (for UI)

    juce::AudioProcessorValueTreeState& getParameters() { return parameters; }
    audioforge::GateEngine& getGateEngine() { return gateEngine; }

private:
    //==============================================================================
    // Parameters (14 total)
    juce::AudioProcessorValueTreeState parameters;

    std::atomic<float>* thresholdParam = nullptr;
    std::atomic<float>* ratioParam = nullptr;
    std::atomic<float>* rangeParam = nullptr;
    std::atomic<float>* attackParam = nullptr;
    std::atomic<float>* holdParam = nullptr;
    std::atomic<float>* releaseParam = nullptr;
    std::atomic<float>* lookaheadParam = nullptr;
    std::atomic<float>* modeParam = nullptr;

    // Sidechain parameters
    std::atomic<float>* sidechainEnableParam = nullptr;
    std::atomic<float>* scHighPassParam = nullptr;
    std::atomic<float>* scLowPassParam = nullptr;
    std::atomic<float>* scListenParam = nullptr;

    // Detection parameters
    std::atomic<float>* detectionModeParam = nullptr;
    std::atomic<float>* rmsWindowParam = nullptr;

    // DSP Engine
    audioforge::GateEngine gateEngine;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GateAudioProcessor)
};
