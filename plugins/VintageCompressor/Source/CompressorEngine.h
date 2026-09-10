#pragma once

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <cmath>

class CompressorEngine
{
public:
    enum class Mode { VCA, FET, Opto };

    CompressorEngine();

    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void reset();
    void processBlock(juce::AudioBuffer<float>& buffer);

    void setMode(Mode mode) { mode_ = mode; }
    void setThreshold(float threshold) { threshold_ = juce::jlimit(-60.0f, 0.0f, threshold); }
    void setRatio(float ratio) { ratio_ = juce::jlimit(1.0f, 20.0f, ratio); }
    void setAttack(float attack) { attack_ = juce::jlimit(0.1f, 100.0f, attack); }
    void setRelease(float release) { release_ = juce::jlimit(10.0f, 1000.0f, release); }
    void setKnee(float knee) { knee_ = juce::jlimit(0.0f, 12.0f, knee); }
    void setMakeupGain(float gain) { makeupGain_ = juce::jlimit(0.0f, 24.0f, gain); }
    void setMix(float mix) { mix_ = juce::jlimit(0.0f, 1.0f, mix); }

    float getGainReduction() const { return gainReduction_; }

private:
    double sampleRate_ = 44100.0;
    Mode mode_ = Mode::VCA;

    // Parameters
    float threshold_ = -20.0f;
    float ratio_ = 4.0f;
    float attack_ = 10.0f;
    float release_ = 100.0f;
    float knee_ = 3.0f;
    float makeupGain_ = 0.0f;
    float mix_ = 1.0f;

    // State
    float envelope_ = 0.0f;
    float gainReduction_ = 0.0f;

    float computeGain(float inputLevel);
    float getAttackCoeff();
    float getReleaseCoeff();
};
