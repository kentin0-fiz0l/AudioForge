#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "../../../midi/MIDILearnManager.h"
#include "../../../shared/preset/PresetManager.h"
#include "SpectralProcessor.h"
#include "FrozenSpectrum.h"
#include "PhaseEvolver.h"

/**
 * FreezeFX Plugin Processor
 *
 * Real-time spectral freezing plugin with phase evolution.
 */
class FreezeFXProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    FreezeFXProcessor();
    ~FreezeFXProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    //==============================================================================
    const juce::String getName() const override { return "FreezeFX"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    //==============================================================================
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    // Parameter IDs
    static constexpr const char* PARAM_FREEZE = "freeze";
    static constexpr const char* PARAM_FREEZE_MIX = "freezeMix";
    static constexpr const char* PARAM_FFT_SIZE = "fftSize";
    static constexpr const char* PARAM_OVERLAP = "overlap";
    static constexpr const char* PARAM_PHASE_RANDOM = "phaseRandom";
    static constexpr const char* PARAM_PHASE_SPEED = "phaseSpeed";
    static constexpr const char* PARAM_SPECTRAL_BLUR = "spectralBlur";
    static constexpr const char* PARAM_HIGH_PASS = "highPass";
    static constexpr const char* PARAM_LOW_PASS = "lowPass";

    //==============================================================================
    // Parameter access (for UI)
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    AudioForge::MIDILearnManager& getMIDILearnManager() { return midiLearnManager_; }
    AudioForge::PresetManager& getPresetManager() { return presetManager_; }

    // Access to spectral data (for visualization)
    const SpectralProcessor& getSpectralProcessor() const { return spectralProcessor; }
    bool isCurrentlyFrozen() const { return frozenForUI.load(); }

private:
    //==============================================================================
    // Spectral Processing
    void processSpectrum(int channel, std::vector<float>& magnitude, std::vector<float>& phase);

    //==============================================================================
    // DSP Components
    SpectralProcessor spectralProcessor;

    // What one channel is holding while frozen
    struct FrozenChannel
    {
        FrozenSpectrum spectrum;
        PhaseEvolver phaseEvolver;

        // The phase the frozen sound has reached in each bin, and how far it
        // moves on every frame. A sound only holds its pitch and level if
        // its phase keeps turning at the rate it had when it was captured.
        std::vector<float> phase;
        std::vector<float> phaseAdvance;

        bool capturePending = false;  // Freeze was just switched on; capture the next frame
        bool measurePending = false;  // The frame after that one shows how fast each phase turns
    };
    std::vector<FrozenChannel> frozenChannels;

    //==============================================================================
    // Parameters (managed by AudioProcessorValueTreeState)
    juce::AudioProcessorValueTreeState apvts;
    AudioForge::MIDILearnManager midiLearnManager_;
    AudioForge::PresetManager presetManager_;

    //==============================================================================
    // State
    bool wasFrozen = false;  // Track previous freeze state for edge detection
    std::atomic<bool> frozenForUI { false };  // The same, for the editor's thread

    //==============================================================================
    // Performance: Pre-allocated buffers for spectral blending (avoid per-frame allocation)
    std::vector<float> tempFrozenMagnitude;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FreezeFXProcessor)
};
