#pragma once
#include <juce_core/juce_core.h>
#include <cmath>
class TomEngine {
public:
    TomEngine();
    void prepareToPlay(double sr, int) { sampleRate_ = sr; reset(); }
    void reset() { phase_ = 0.0f; bodyEnv_ = pitchEnv_ = noiseEnv_ = 0.0f; active_ = false; lpFilter_ = hpFilter_ = 0.0f; }
    float processSample();
    void trigger(float vel) { velocity_ = vel; active_ = true; bodyEnv_ = pitchEnv_ = noiseEnv_ = 1.0f; phase_ = 0.0f; }
    void release() {}
    void setPitch(float p) { pitch_ = juce::jlimit(40.0f, 300.0f, p); }
    void setPitchEnv(float e) { pitchEnv_ = juce::jlimit(0.0f, 3.0f, e); }
    void setDecay(float d) { decay_ = juce::jlimit(0.05f, 2.0f, d); }
    void setBend(float b) { bend_ = juce::jlimit(0.0f, 1.0f, b); }
    void setNoise(float n) { noiseMix_ = juce::jlimit(0.0f, 1.0f, n); }
    bool isActive() const { return active_; }
private:
    double sampleRate_ = 44100.0; float velocity_ = 1.0f; bool active_ = false;
    float pitch_ = 90.0f, pitchEnv_ = 1.5f, decay_ = 0.4f, bend_ = 0.5f, noiseMix_ = 0.2f;
    float phase_ = 0.0f, bodyEnv_ = 0.0f, pitchEnv_state = 0.0f, noiseEnv_ = 0.0f;
    float lpFilter_ = 0.0f, hpFilter_ = 0.0f;
    uint32_t noiseState_ = 98765;
    float generateNoise() { noiseState_ = noiseState_ * 1103515245 + 12345; return ((int)(noiseState_/65536) % 32768) / 16384.0f - 1.0f; }
};
