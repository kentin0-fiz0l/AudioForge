#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

class TremoloVibratoEngine {
public:
    TremoloVibratoEngine();
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void processBlock(juce::AudioBuffer<float>& buffer);

    enum class Mode { Tremolo, Vibrato };
    void setMode(Mode mode) { mode_ = mode; }
    void setRate(float rate) { rate_ = juce::jlimit(0.1f, 20.0f, rate); }
    void setDepth(float depth) { depth_ = juce::jlimit(0.0f, 1.0f, depth); }
    void setWaveform(int wf) { waveform_ = juce::jlimit(0, 2, wf); } // 0=sine, 1=tri, 2=square
    void setMix(float mix) { mix_ = juce::jlimit(0.0f, 1.0f, mix); }

private:
    Mode mode_ = Mode::Tremolo;
    float rate_ = 5.0f;
    float depth_ = 0.5f;
    int waveform_ = 0;
    float mix_ = 1.0f;

    double sampleRate_ = 44100.0;
    float lfoPhase_ = 0.0f;

    // Vibrato: delay line for pitch shifting
    static constexpr int MAX_DELAY_SAMPLES = 4410; // 100ms at 44.1kHz
    juce::AudioBuffer<float> delayBuffer_;
    int delayWritePos_ = 0;

    float getLFOValue();
    void processTremolo(juce::AudioBuffer<float>& buffer);
    void processVibrato(juce::AudioBuffer<float>& buffer);
};
