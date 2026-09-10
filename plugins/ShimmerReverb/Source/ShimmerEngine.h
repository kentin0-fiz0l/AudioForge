#pragma once
#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <vector>

class ShimmerEngine {
public:
    void prepareToPlay(double sr, int sb);
    void processBlock(juce::AudioBuffer<float>&);
    void setSize(float size) { size_ = juce::jlimit(0.0f, 1.0f, size); }
    void setDamping(float damp) { damping_ = juce::jlimit(0.0f, 1.0f, damp); }
    void setShimmer(float shimmer) { shimmer_ = juce::jlimit(0.0f, 1.0f, shimmer); }
    void setPitchMode(int mode) { pitchMode_ = juce::jlimit(0, 2, mode); } // 0=up, 1=down, 2=both
    void setFeedback(float fb) { feedback_ = juce::jlimit(0.0f, 0.85f, fb); }
    void setMix(float mix) { mix_ = juce::jlimit(0.0f, 1.0f, mix); }
private:
    double sampleRate_ = 44100.0;
    float size_ = 0.5f, damping_ = 0.5f, shimmer_ = 0.5f, feedback_ = 0.3f, mix_ = 0.5f;
    int pitchMode_ = 2; // both
    juce::dsp::Reverb reverb_;
    std::vector<float> pitchBuffer_[2];
    int pitchWritePos_ = 0;
    float pitchPhase_ = 0.0f;
};
