#pragma once
#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <cmath>
#include <vector>
#include "../../../shared/dsp/ReverbLevels.h"

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
    float shift(int ch, float wet);
    float readDelayed(int ch, float delay) const;

    float size_ = 0.5f, damping_ = 0.5f, shimmer_ = 0.5f, feedback_ = 0.3f, mix_ = 0.5f;
    int pitchMode_ = 2; // both
    juce::Reverb reverb_;
    // Delay line for the pitch shifter, one per channel
    std::vector<float> pitchBuffer_[2];
    int pitchWritePos_ = 0;
    float windowLength_ = 0.0f;         // Length of one shifter grain, in samples
    float pitchPhase_[2] = {0.0f, 0.0f}; // Position in the grain: [0] octave up, [1] octave down
    // Shimmer from the previous sample, fed back into the reverb input
    float feedbackSample_[2] = {0.0f, 0.0f};
    juce::dsp::IIR::Filter<float> feedbackHighPass_[2];
    juce::dsp::IIR::Filter<float> feedbackLowPass_[2];
};
