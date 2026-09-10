#pragma once
#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <vector>
class DelayEngine {
public:
    void prepareToPlay(double sr, int sb);
    void processBlock(juce::AudioBuffer<float>&);
    void setTime(float time) { time_ = juce::jlimit(10.0f, 2000.0f, time); }
    void setFeedback(float fb) { feedback_ = juce::jlimit(0.0f, 0.95f, fb); }
    void setWow(float wow) { wow_ = juce::jlimit(0.0f, 1.0f, wow); }
    void setFlutter(float flutter) { flutter_ = juce::jlimit(0.0f, 1.0f, flutter); }
    void setSaturation(float sat) { saturation_ = juce::jlimit(0.0f, 1.0f, sat); }
    void setMix(float mix) { mix_ = juce::jlimit(0.0f, 1.0f, mix); }
private:
    double sampleRate_ = 44100.0;
    float time_ = 500.0f, feedback_ = 0.4f, wow_ = 0.2f, flutter_ = 0.1f, saturation_ = 0.3f, mix_ = 0.5f;
    std::vector<float> delayBuffer_[2];
    int writePos_ = 0;
    float wowPhase_ = 0.0f, flutterPhase_ = 0.0f;
    float lpFilter_[2] = {0.0f};
};
