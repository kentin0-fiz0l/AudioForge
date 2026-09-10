#pragma once
#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <cmath>

class HiHatEngine
{
public:
    HiHatEngine();
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void reset();
    float processSample();
    void trigger(float velocity);
    void release();
    
    void setTone(float tone) { tone_ = juce::jlimit(0.0f, 1.0f, tone); }
    void setDecay(float decay) { decay_ = juce::jlimit(0.01f, 0.5f, decay); }
    void setMetallic(float metallic) { metallic_ = juce::jlimit(0.0f, 1.0f, metallic); }
    void setOpenness(float openness) { openness_ = juce::jlimit(0.0f, 1.0f, openness); }
    bool isActive() const { return active_; }

private:
    double sampleRate_ = 44100.0;
    float velocity_ = 1.0f;
    bool active_ = false;
    float tone_ = 0.5f, decay_ = 0.08f, metallic_ = 0.7f, openness_ = 0.3f;
    float envelope_ = 0.0f;
    uint32_t noiseState_ = 54321;
    float bpFilter_[6] = {0};
    float hpFilter_ = 0.0f;
    float generateNoise();
};
