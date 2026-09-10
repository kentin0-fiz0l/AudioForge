#pragma once

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <cmath>

class SnareEngine
{
public:
    SnareEngine();

    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void reset();
    float processSample();

    void trigger(float velocity);
    void release();

    void setTone(float tone) { tone_ = juce::jlimit(100.0f, 400.0f, tone); }
    void setSnappy(float snappy) { snappy_ = juce::jlimit(0.0f, 1.0f, snappy); }
    void setDecay(float decay) { decay_ = juce::jlimit(0.01f, 1.0f, decay); }
    void setSnareDecay(float decay) { snareDecay_ = juce::jlimit(0.01f, 0.5f, decay); }
    void setPitchEnv(float env) { pitchEnv_ = juce::jlimit(0.0f, 2.0f, env); }
    void setSnap(float snap) { snap_ = juce::jlimit(0.0f, 1.0f, snap); }

    bool isActive() const { return active_; }

private:
    double sampleRate_ = 44100.0;
    float velocity_ = 1.0f;
    bool active_ = false;

    // Parameters
    float tone_ = 180.0f;
    float snappy_ = 0.7f;
    float decay_ = 0.15f;
    float snareDecay_ = 0.12f;
    float pitchEnv_ = 0.8f;
    float snap_ = 0.5f;

    // DSP state
    float tonePhase_ = 0.0f;
    float bodyEnvelope_ = 0.0f;
    float snareEnvelope_ = 0.0f;
    float pitchEnvelope_ = 0.0f;
    float snapEnvelope_ = 0.0f;
    uint32_t noiseState_ = 12345;

    // Filters for snare rattle
    float bpFilter1_ = 0.0f;
    float bpFilter2_ = 0.0f;
    float hpFilter_ = 0.0f;

    float generateNoise();
};
