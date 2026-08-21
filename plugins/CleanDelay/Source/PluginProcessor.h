#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <dsp/AudioForgeDSP.h>

/**
 * CleanDelay Audio Processor
 *
 * A stereo delay effect with:
 * - Adjustable delay time (1ms - 2000ms)
 * - Feedback control (0-100%)
 * - Wet/dry mix (0-100%)
 * - Ping-pong mode (alternating L/R delays)
 * - Tempo sync (planned for future)
 */
class CleanDelayProcessor : public juce::AudioProcessor
{
public:
    CleanDelayProcessor();
    ~CleanDelayProcessor() override;

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
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return delayTimeParam->get() * 3.0; }

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
    float getCurrentInputLevel() const { return inputLevelMeter.getLevel(); }
    float getCurrentOutputLevel() const { return outputLevelMeter.getLevel(); }

    // Parameter IDs
    static constexpr const char* PARAM_DELAY_TIME = "delayTime";
    static constexpr const char* PARAM_FEEDBACK = "feedback";
    static constexpr const char* PARAM_MIX = "mix";
    static constexpr const char* PARAM_PINGPONG = "pingPong";

private:
    //==============================================================================
    // Parameters
    juce::AudioParameterFloat* delayTimeParam;  // Delay time in seconds
    juce::AudioParameterFloat* feedbackParam;   // Feedback amount (0.0 to 0.99)
    juce::AudioParameterFloat* mixParam;        // Wet/dry mix (0.0 to 1.0)
    juce::AudioParameterBool* pingPongParam;    // Ping-pong mode on/off

    // Delay lines (one per channel)
    static constexpr int MAX_DELAY_SAMPLES = 96000 * 2; // 2 seconds at 96kHz
    std::array<float, MAX_DELAY_SAMPLES> delayBufferLeft;
    std::array<float, MAX_DELAY_SAMPLES> delayBufferRight;
    int writePosition = 0;
    double currentSampleRate = 44100.0;

    // Level metering
    AudioForge::DSP::ThreadSafeMeter inputLevelMeter;
    AudioForge::DSP::ThreadSafeMeter outputLevelMeter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CleanDelayProcessor)
};
