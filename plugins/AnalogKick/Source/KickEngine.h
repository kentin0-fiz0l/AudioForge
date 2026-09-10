#pragma once

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <cmath>

class KickEngine
{
public:
    KickEngine();

    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void reset();
    float processSample();

    void trigger(float velocity);
    void release();

    void setPitch(float pitch) { pitch_ = juce::jlimit(20.0f, 200.0f, pitch); }
    void setPitchEnvAmount(float amount) { pitchEnvAmount_ = juce::jlimit(0.0f, 4.0f, amount); }
    void setPitchEnvDecay(float decay) { pitchEnvDecay_ = juce::jlimit(0.001f, 0.2f, decay); }
    void setDecay(float decay) { decay_ = juce::jlimit(0.01f, 2.0f, decay); }
    void setClick(float click) { clickLevel_ = juce::jlimit(0.0f, 1.0f, click); }
    void setDrive(float drive) { drive_ = juce::jlimit(1.0f, 10.0f, drive); }
    void setTone(float tone) { tone_ = juce::jlimit(0.0f, 1.0f, tone); }

    bool isActive() const { return active_; }

private:
    double sampleRate_ = 44100.0;
    float velocity_ = 1.0f;
    bool active_ = false;

    // Parameters
    float pitch_ = 55.0f;
    float pitchEnvAmount_ = 2.5f;
    float pitchEnvDecay_ = 0.05f;
    float decay_ = 0.3f;
    float clickLevel_ = 0.3f;
    float drive_ = 1.5f;
    float tone_ = 0.5f;

    // DSP state
    float phase_ = 0.0f;
    float ampEnvelope_ = 0.0f;
    float pitchEnvelope_ = 0.0f;
    float clickEnvelope_ = 0.0f;
    float noisePhase_ = 0.0f;

    // Filters
    float lpFilter_ = 0.0f;
    float hpFilter_ = 0.0f;

    float generateNoise();
    float softClip(float sample);
};
