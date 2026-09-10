#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

class PannerEngine {
public:
    PannerEngine();
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void processBlock(juce::AudioBuffer<float>& buffer);

    void setRate(float rate) { rate_ = juce::jlimit(0.01f, 20.0f, rate); }
    void setDepth(float depth) { depth_ = juce::jlimit(0.0f, 1.0f, depth); }
    void setWaveform(int wf) { waveform_ = juce::jlimit(0, 2, wf); } // 0=sine, 1=tri, 2=square
    void setPhaseOffset(float offset) { phaseOffset_ = juce::jlimit(0.0f, 1.0f, offset); }
    void setStereoWidth(float width) { stereoWidth_ = juce::jlimit(0.0f, 2.0f, width); }

private:
    float rate_ = 1.0f;
    float depth_ = 1.0f;
    int waveform_ = 0;
    float phaseOffset_ = 0.5f; // L/R phase relationship (0.5 = 180° for classic autopan)
    float stereoWidth_ = 1.0f;

    double sampleRate_ = 44100.0;
    float lfoPhase_ = 0.0f;

    float getLFOValue(float phase);
    float getPanLaw(float pan); // Equal power panning
};
