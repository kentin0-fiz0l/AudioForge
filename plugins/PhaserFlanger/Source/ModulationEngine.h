#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class ModulationEngine {
public:
    ModulationEngine();
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void processBlock(juce::AudioBuffer<float>& buffer);

    enum class Mode { Phaser, Flanger };
    void setMode(Mode mode) { mode_ = mode; }
    void setRate(float rate) { rate_ = juce::jlimit(0.1f, 10.0f, rate); }
    void setDepth(float depth) { depth_ = juce::jlimit(0.0f, 1.0f, depth); }
    void setFeedback(float feedback) { feedback_ = juce::jlimit(-0.9f, 0.9f, feedback); }
    void setCentreFrequency(float freq) { centreFreq_ = juce::jlimit(200.0f, 2000.0f, freq); }
    void setMix(float mix) { mix_ = juce::jlimit(0.0f, 1.0f, mix); }

private:
    Mode mode_ = Mode::Phaser;
    float rate_ = 0.5f;
    float depth_ = 0.5f;
    float feedback_ = 0.5f;
    float centreFreq_ = 1000.0f;
    float mix_ = 0.5f;

    double sampleRate_ = 44100.0;
    float lfoPhase_ = 0.0f;

    // Phaser: all-pass filters
    static constexpr int NUM_STAGES = 6;
    struct AllPassFilter {
        float z1 = 0.0f;
        float processSample(float input, float coeff) {
            float output = -input + (coeff * (input + z1));
            z1 = output;
            return output;
        }
    };
    AllPassFilter apFilters_[2][NUM_STAGES]; // stereo

    // Flanger: delay line
    static constexpr int MAX_DELAY_SAMPLES = 4410; // 100ms at 44.1kHz
    juce::AudioBuffer<float> delayBuffer_;
    int delayWritePos_ = 0;

    float getLFOValue();
    void processPhaser(juce::AudioBuffer<float>& buffer);
    void processFlanger(juce::AudioBuffer<float>& buffer);
};
