#pragma once

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <cmath>

class FMEngine
{
public:
    static constexpr int NUM_OPERATORS = 4;

    FMEngine();

    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void reset();
    float processSample();

    void noteOn(float frequency, float velocity);
    void noteOff();

    // Algorithm
    void setAlgorithm(int algo) { algorithm_ = juce::jlimit(0, 7, algo); }

    // Operator parameters (per operator)
    void setOpLevel(int op, float level) { operators_[op].level = level; }
    void setOpRatio(int op, float ratio) { operators_[op].ratio = ratio; }
    void setOpAttack(int op, float attack) { operators_[op].envelope.setAttack(attack); }
    void setOpDecay(int op, float decay) { operators_[op].envelope.setDecay(decay); }
    void setOpSustain(int op, float sustain) { operators_[op].envelope.setSustain(sustain); }
    void setOpRelease(int op, float release) { operators_[op].envelope.setRelease(release); }

    // Global
    void setFeedback(float fb) { feedback_ = juce::jlimit(0.0f, 1.0f, fb); }

    bool isActive() const { return operators_[0].envelope.isActive(); }

private:
    struct ADSREnvelope
    {
        enum class Stage { Idle, Attack, Decay, Sustain, Release };
        double sampleRate = 44100.0;
        Stage stage = Stage::Idle;
        float output = 0.0f;
        float attack = 0.01f, decay = 0.2f, sustain = 0.7f, release = 0.5f;

        void setAttack(float a) { attack = juce::jlimit(0.001f, 2.0f, a); }
        void setDecay(float d) { decay = juce::jlimit(0.001f, 2.0f, d); }
        void setSustain(float s) { sustain = juce::jlimit(0.0f, 1.0f, s); }
        void setRelease(float r) { release = juce::jlimit(0.001f, 5.0f, r); }

        void trigger() { stage = Stage::Attack; }
        void release_() { stage = Stage::Release; }
        void reset() { stage = Stage::Idle; output = 0.0f; }
        bool isActive() const { return stage != Stage::Idle; }

        float process()
        {
            float attackRate = 1.0f / (attack * static_cast<float>(sampleRate));
            float decayRate = 1.0f / (decay * static_cast<float>(sampleRate));
            float releaseRate = 1.0f / (release * static_cast<float>(sampleRate));

            switch (stage)
            {
                case Stage::Attack:
                    output += attackRate;
                    if (output >= 1.0f) { output = 1.0f; stage = Stage::Decay; }
                    break;
                case Stage::Decay:
                    output -= decayRate * (1.0f - sustain);
                    if (output <= sustain) { output = sustain; stage = Stage::Sustain; }
                    break;
                case Stage::Sustain:
                    output = sustain;
                    break;
                case Stage::Release:
                    output -= releaseRate * sustain;
                    if (output <= 0.0f) { output = 0.0f; stage = Stage::Idle; }
                    break;
                case Stage::Idle:
                    output = 0.0f;
                    break;
            }
            return output;
        }
    };

    struct Operator
    {
        float phase = 0.0f;
        float level = 1.0f;
        float ratio = 1.0f; // Frequency multiplier
        ADSREnvelope envelope;

        float generateSine(float phaseInput)
        {
            return std::sin(phaseInput);
        }
    };

    float processAlgorithm();

    double sampleRate_ = 44100.0;
    float baseFrequency_ = 440.0f;
    float velocity_ = 1.0f;
    int algorithm_ = 0;
    float feedback_ = 0.0f;
    float feedbackSample_ = 0.0f;

    Operator operators_[NUM_OPERATORS];
};
