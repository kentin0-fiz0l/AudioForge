/**
 * Reverb Audio Processor
 *
 * Algorithmic reverb based on Freeverb design with:
 * - 8 parallel comb filters with damping
 * - 4 series all-pass filters for diffusion
 * - Stereo processing with decorrelation
 * - Pre-delay, room size, damping, width, mix, and freeze controls
 */

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "dsp/Metering.h"

class ReverbAudioProcessor : public juce::AudioProcessor
{
public:
    ReverbAudioProcessor();
    ~ReverbAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Parameter access
    juce::AudioParameterFloat* roomSizeParam;
    juce::AudioParameterFloat* dampingParam;
    juce::AudioParameterFloat* widthParam;
    juce::AudioParameterFloat* preDelayParam;
    juce::AudioParameterFloat* mixParam;
    juce::AudioParameterBool* freezeParam;

    // Metering
    AudioForge::DSP::ThreadSafeMeter inputMeter;
    AudioForge::DSP::ThreadSafeMeter outputMeter;

private:
    // Comb filter structure
    struct CombFilter
    {
        std::vector<float> buffer;
        int bufferSize = 0;
        int bufferIndex = 0;
        float feedback = 0.84f;
        float damp1 = 0.5f;
        float damp2 = 0.5f;
        float filterStore = 0.0f;

        void setBufferSize(int size)
        {
            bufferSize = size;
            buffer.resize(size, 0.0f);
            bufferIndex = 0;
        }

        void clear()
        {
            std::fill(buffer.begin(), buffer.end(), 0.0f);
            filterStore = 0.0f;
            bufferIndex = 0;
        }

        float process(float input)
        {
            float output = buffer[bufferIndex];
            filterStore = (output * damp2) + (filterStore * damp1);
            buffer[bufferIndex] = input + (filterStore * feedback);

            if (++bufferIndex >= bufferSize)
                bufferIndex = 0;

            return output;
        }
    };

    // All-pass filter structure
    struct AllPassFilter
    {
        std::vector<float> buffer;
        int bufferSize = 0;
        int bufferIndex = 0;
        float feedback = 0.5f;

        void setBufferSize(int size)
        {
            bufferSize = size;
            buffer.resize(size, 0.0f);
            bufferIndex = 0;
        }

        void clear()
        {
            std::fill(buffer.begin(), buffer.end(), 0.0f);
            bufferIndex = 0;
        }

        float process(float input)
        {
            float bufOut = buffer[bufferIndex];
            float output = -input + bufOut;
            buffer[bufferIndex] = input + (bufOut * feedback);

            if (++bufferIndex >= bufferSize)
                bufferIndex = 0;

            return output;
        }
    };

    // Reverb components (8 comb filters + 4 all-pass filters per channel)
    static constexpr int numCombs = 8;
    static constexpr int numAllPasses = 4;

    CombFilter combsLeft[numCombs];
    CombFilter combsRight[numCombs];
    AllPassFilter allPassesLeft[numAllPasses];
    AllPassFilter allPassesRight[numAllPasses];

    // Pre-delay buffer
    std::vector<float> preDelayBufferLeft;
    std::vector<float> preDelayBufferRight;
    int preDelayIndex = 0;
    int maxPreDelaySamples = 4410; // 100ms @ 44.1kHz

    // Sample rate scaling factor
    float sampleRateScale = 1.0f;

    // Tuning constants (delay times in samples @ 44.1kHz)
    static constexpr int combTuningL[numCombs] = {1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617};
    static constexpr int combTuningR[numCombs] = {1116+23, 1188+23, 1277+23, 1356+23, 1422+23, 1491+23, 1557+23, 1617+23};
    static constexpr int allPassTuning[numAllPasses] = {556, 441, 341, 225};

    void updateReverb();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverbAudioProcessor)
};
