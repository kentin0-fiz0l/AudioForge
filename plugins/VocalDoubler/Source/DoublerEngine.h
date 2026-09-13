#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <vector>
#include <random>

class DoublerEngine {
public:
    DoublerEngine();
    ~DoublerEngine() = default;

    void prepare(double sampleRate, int samplesPerBlock);
    void reset();
    void process(juce::AudioBuffer<float>& buffer);

    // Parameter setters
    void setVoiceCount(int count);           // 1-4 voices
    void setWidth(float widthPercent);       // 0-100%
    void setPitchVariation(float cents);     // 0-20 cents
    void setTimingShift(float milliseconds); // 0-30 ms
    void setHumanize(float amount);          // 0-100%
    void setHighPass(float freqHz);          // 20-500 Hz
    void setLowPass(float freqHz);           // 2000-20000 Hz
    void setMix(float mix);                  // 0-1
    void setOutputGain(float gainDb);        // -12 to +12 dB

private:
    struct Voice {
        juce::AudioBuffer<float> delayBuffer;
        int delayWritePos = 0;
        float pitchShift = 0.0f;        // In cents
        float timingOffset = 0.0f;      // In samples
        float panPosition = 0.0f;       // -1 (left) to +1 (right)
        float lfoPhase = 0.0f;          // For humanization
        float lfoSpeed = 0.0f;

        // Simple pitch shifting using variable delay
        float pitchReadPos = 0.0f;
        float pitchReadSpeed = 1.0f;
    };

    void updateVoiceParameters();
    float readDelayedSample(Voice& voice, int channel, float delaySamples);

    double sampleRate_ = 44100.0;
    int voiceCount_ = 2;
    float width_ = 0.5f;
    float pitchVariation_ = 10.0f;
    float timingShift_ = 15.0f;
    float humanize_ = 50.0f;
    float mix_ = 0.5f;
    float outputGain_ = 1.0f;

    std::vector<Voice> voices_;
    static constexpr int kMaxDelayMs = 50;
    int maxDelaySamples_ = 2205; // 50ms at 44.1kHz

    // Filters for each voice
    std::vector<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                                 juce::dsp::IIR::Coefficients<float>>> highPassFilters_;
    std::vector<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                                 juce::dsp::IIR::Coefficients<float>>> lowPassFilters_;

    float highPassFreq_ = 100.0f;
    float lowPassFreq_ = 12000.0f;

    std::mt19937 randomGen_;
    std::uniform_real_distribution<float> randomDist_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DoublerEngine)
};
