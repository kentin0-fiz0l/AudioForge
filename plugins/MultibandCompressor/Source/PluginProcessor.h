/*
  ==============================================================================

    PluginProcessor.h
    Created: 1 Sep 2026
    Author:  AudioForge Team

    Main plugin processor for Multiband Compressor.

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "MultibandEngine.h"

//==============================================================================
/**
 * @brief AudioForge Multiband Compressor
 *
 * Features:
 * - 4-band frequency splitting (Linkwitz-Riley 4th-order)
 * - Per-band compression (threshold, ratio, attack, release, makeup)
 * - Mid/Side processing per band
 * - Solo/Bypass per band
 * - Real-time FFT analyzer
 *
 * Parameters: 35 total
 * - 3 crossover frequencies
 * - 20 per-band compression controls (5 × 4 bands)
 * - 12 per-band switches (3 × 4 bands)
 */
class MultibandCompressorAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    MultibandCompressorAudioProcessor();
    ~MultibandCompressorAudioProcessor() override;

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
    audioforge::MultibandEngine& getMultibandEngine() { return multibandEngine; }

private:
    //==============================================================================
    // Parameters (35 total)
    juce::AudioProcessorValueTreeState parameters;

    // Crossover frequencies (3)
    std::atomic<float>* crossover1Param = nullptr;
    std::atomic<float>* crossover2Param = nullptr;
    std::atomic<float>* crossover3Param = nullptr;

    // Band 1 (Low) parameters (5 + 3 = 8)
    std::atomic<float>* band1ThresholdParam = nullptr;
    std::atomic<float>* band1RatioParam = nullptr;
    std::atomic<float>* band1AttackParam = nullptr;
    std::atomic<float>* band1ReleaseParam = nullptr;
    std::atomic<float>* band1MakeupParam = nullptr;
    std::atomic<float>* band1SoloParam = nullptr;
    std::atomic<float>* band1BypassParam = nullptr;
    std::atomic<float>* band1MidSideParam = nullptr;

    // Band 2 (Low-Mid) parameters (5 + 3 = 8)
    std::atomic<float>* band2ThresholdParam = nullptr;
    std::atomic<float>* band2RatioParam = nullptr;
    std::atomic<float>* band2AttackParam = nullptr;
    std::atomic<float>* band2ReleaseParam = nullptr;
    std::atomic<float>* band2MakeupParam = nullptr;
    std::atomic<float>* band2SoloParam = nullptr;
    std::atomic<float>* band2BypassParam = nullptr;
    std::atomic<float>* band2MidSideParam = nullptr;

    // Band 3 (High-Mid) parameters (5 + 3 = 8)
    std::atomic<float>* band3ThresholdParam = nullptr;
    std::atomic<float>* band3RatioParam = nullptr;
    std::atomic<float>* band3AttackParam = nullptr;
    std::atomic<float>* band3ReleaseParam = nullptr;
    std::atomic<float>* band3MakeupParam = nullptr;
    std::atomic<float>* band3SoloParam = nullptr;
    std::atomic<float>* band3BypassParam = nullptr;
    std::atomic<float>* band3MidSideParam = nullptr;

    // Band 4 (High) parameters (5 + 3 = 8)
    std::atomic<float>* band4ThresholdParam = nullptr;
    std::atomic<float>* band4RatioParam = nullptr;
    std::atomic<float>* band4AttackParam = nullptr;
    std::atomic<float>* band4ReleaseParam = nullptr;
    std::atomic<float>* band4MakeupParam = nullptr;
    std::atomic<float>* band4SoloParam = nullptr;
    std::atomic<float>* band4BypassParam = nullptr;
    std::atomic<float>* band4MidSideParam = nullptr;

    // DSP Engine
    audioforge::MultibandEngine multibandEngine;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultibandCompressorAudioProcessor)
};
