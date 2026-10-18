#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <synth/VoiceManager.h>
#include "SamplerVoice.h"
#include "SampleLibrary.h"
#include "KeyZone.h"
#include "SfzSampleMap.h"
#include "../../../midi/MIDILearnManager.h"
#include "../../../shared/preset/PresetManager.h"

/**
 * Sampler Plugin Processor
 *
 * Professional multi-sample instrument with key zone mapping,
 * velocity layers, loop playback, ADSR envelopes, and filtering.
 *
 * Features:
 * - Multi-sample support (up to 512 MB)
 * - Key zone mapping with velocity layers
 * - Round-robin playback
 * - Loop points (forward, pingpong)
 * - ADSR envelope + filter modulation
 * - 32-voice polyphony with voice stealing
 * - SFZ instruments: a saved state with sfz="/path/file.sfz" on its root
 *   loads one, and plays it in place of the key zones
 * - MIDI: sustain pedal (CC64), volume (CC7) and expression (CC11)
 */
class SamplerPluginProcessor : public juce::AudioProcessor
{
public:
    SamplerPluginProcessor();
    ~SamplerPluginProcessor() override;

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
    AudioForge::MIDILearnManager& getMIDILearnManager() { return midiLearnManager_; }
    AudioForge::PresetManager& getPresetManager() { return presetManager_; }

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    // Sample management
    bool loadSample(const juce::File& file);

    /**
     * Load an SFZ instrument (see SfzParser.h for what it reads). While one
     * is loaded, notes play it rather than the key zones. Notes on what it
     * left out go to the log.
     */
    bool loadSfz(const juce::File& file, juce::String& error);
    void clearSfz();
    const SfzSampleMap* getSfz() const { return sfzMap.get(); }
    SampleLibrary& getSampleLibrary() { return sampleLibrary; }
    KeyZoneMap& getKeyZoneMap() { return keyZoneMap; }
    const KeyZoneMap& getKeyZoneMap() const { return keyZoneMap; }

    //==============================================================================
    // Parameter access
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    //==============================================================================
    // Parameter IDs
    static constexpr const char* PARAM_AMP_ATTACK = "ampAttack";
    static constexpr const char* PARAM_AMP_DECAY = "ampDecay";
    static constexpr const char* PARAM_AMP_SUSTAIN = "ampSustain";
    static constexpr const char* PARAM_AMP_RELEASE = "ampRelease";

    static constexpr const char* PARAM_FILTER_CUTOFF = "filterCutoff";
    static constexpr const char* PARAM_FILTER_RESONANCE = "filterResonance";
    static constexpr const char* PARAM_FILTER_TYPE = "filterType";
    static constexpr const char* PARAM_FILTER_ENV_AMOUNT = "filterEnvAmount";

    static constexpr const char* PARAM_FILT_ATTACK = "filtAttack";
    static constexpr const char* PARAM_FILT_DECAY = "filtDecay";
    static constexpr const char* PARAM_FILT_SUSTAIN = "filtSustain";
    static constexpr const char* PARAM_FILT_RELEASE = "filtRelease";

    static constexpr const char* PARAM_SAMPLE_START = "sampleStart";
    static constexpr const char* PARAM_SAMPLE_END = "sampleEnd";
    static constexpr const char* PARAM_LOOP_MODE = "loopMode";
    static constexpr const char* PARAM_LOOP_START = "loopStart";
    static constexpr const char* PARAM_LOOP_END = "loopEnd";

    static constexpr const char* PARAM_MASTER_VOLUME = "masterVolume";
    static constexpr const char* PARAM_MASTER_TUNE = "masterTune";
    static constexpr const char* PARAM_VOICE_COUNT = "voiceCount";

private:
    //==============================================================================
    juce::AudioProcessorValueTreeState apvts;
    AudioForge::VoiceManager<SamplerVoice, 32> voiceManager;

    SampleLibrary sampleLibrary;
    KeyZoneMap keyZoneMap;
    std::unique_ptr<SfzSampleMap> sfzMap;

    // Held while the instrument changes; the audio thread skips a block
    // rather than wait for it
    juce::CriticalSection instrumentLock;

    double currentSampleRate = 44100.0;

    // MIDI controller state
    bool sustainPedal = false;
    std::array<bool, 128> heldByPedal {};
    float channelVolume = 1.0f;     // CC7
    float expression = 1.0f;        // CC11
    juce::SmoothedValue<float> controllerGain { 1.0f };
    juce::Random random { 1 };      // For SFZ random round robin; seeded, so renders repeat

    void handleMidi(const juce::MidiMessage& message);
    void startNote(int midiNote, int velocity);
    void releaseNote(int midiNote);
    SamplerVoice& findVoiceToStart();
    void renderVoices(juce::AudioBuffer<float>& buffer, int startSample, int endSample);

    //==============================================================================
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    AudioForge::MIDILearnManager midiLearnManager_;
    AudioForge::PresetManager presetManager_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SamplerPluginProcessor)
};
