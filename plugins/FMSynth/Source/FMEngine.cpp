#include "FMEngine.h"

FMEngine::FMEngine()
{
    for (int i = 0; i < NUM_OPERATORS; ++i)
    {
        operators_[i].ratio = (i == 0) ? 1.0f : static_cast<float>(i + 1);
        operators_[i].level = (i < 2) ? 1.0f : 0.5f; // Carriers louder
    }
}

void FMEngine::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    sampleRate_ = sampleRate;
    for (int i = 0; i < NUM_OPERATORS; ++i)
        operators_[i].envelope.sampleRate = sampleRate;
    reset();
}

void FMEngine::reset()
{
    for (int i = 0; i < NUM_OPERATORS; ++i)
    {
        operators_[i].phase = 0.0f;
        operators_[i].envelope.reset();
    }
    feedbackSample_ = 0.0f;
}

void FMEngine::noteOn(float frequency, float velocity)
{
    baseFrequency_ = frequency;
    velocity_ = velocity;
    for (int i = 0; i < NUM_OPERATORS; ++i)
        operators_[i].envelope.trigger();
}

void FMEngine::noteOff()
{
    for (int i = 0; i < NUM_OPERATORS; ++i)
        operators_[i].envelope.release_();
}

float FMEngine::processSample()
{
    // Update operator phases
    for (int i = 0; i < NUM_OPERATORS; ++i)
    {
        float freq = baseFrequency_ * operators_[i].ratio;
        float increment = freq * juce::MathConstants<float>::twoPi / static_cast<float>(sampleRate_);
        operators_[i].phase += increment;
        if (operators_[i].phase > juce::MathConstants<float>::twoPi)
            operators_[i].phase -= juce::MathConstants<float>::twoPi;
    }

    return processAlgorithm() * velocity_ * 0.3f;
}

float FMEngine::processAlgorithm()
{
    float op[4] = {0, 0, 0, 0};
    float env[4];

    // Process envelopes
    for (int i = 0; i < NUM_OPERATORS; ++i)
        env[i] = operators_[i].envelope.process();

    // Apply feedback to operator 0
    float fbPhase = operators_[0].phase + feedbackSample_ * feedback_ * 3.0f;

    // FM Algorithms (DX7-inspired)
    switch (algorithm_)
    {
        case 0: // All parallel (4 carriers)
            op[0] = operators_[0].generateSine(fbPhase) * env[0] * operators_[0].level;
            op[1] = operators_[1].generateSine(operators_[1].phase) * env[1] * operators_[1].level;
            op[2] = operators_[2].generateSine(operators_[2].phase) * env[2] * operators_[2].level;
            op[3] = operators_[3].generateSine(operators_[3].phase) * env[3] * operators_[3].level;
            feedbackSample_ = op[0];
            return (op[0] + op[1] + op[2] + op[3]) * 0.25f;

        case 1: // 1->2->3->4 (serial)
            op[3] = operators_[3].generateSine(operators_[3].phase) * env[3];
            op[2] = operators_[2].generateSine(operators_[2].phase + op[3] * operators_[3].level) * env[2];
            op[1] = operators_[1].generateSine(operators_[1].phase + op[2] * operators_[2].level) * env[1];
            op[0] = operators_[0].generateSine(fbPhase + op[1] * operators_[1].level) * env[0];
            feedbackSample_ = op[0];
            return op[0] * operators_[0].level;

        case 2: // (1->2) + (3->4) (two pairs)
            op[1] = operators_[1].generateSine(operators_[1].phase) * env[1];
            op[0] = operators_[0].generateSine(fbPhase + op[1] * operators_[1].level) * env[0] * operators_[0].level;
            op[3] = operators_[3].generateSine(operators_[3].phase) * env[3];
            op[2] = operators_[2].generateSine(operators_[2].phase + op[3] * operators_[3].level) * env[2] * operators_[2].level;
            feedbackSample_ = op[0];
            return (op[0] + op[2]) * 0.5f;

        case 3: // (1+2)->3->4 (stacked)
            op[1] = operators_[1].generateSine(operators_[1].phase) * env[1] * operators_[1].level;
            op[0] = operators_[0].generateSine(fbPhase) * env[0] * operators_[0].level;
            op[2] = operators_[2].generateSine(operators_[2].phase + (op[0] + op[1]) * 0.5f) * env[2];
            op[3] = operators_[3].generateSine(operators_[3].phase + op[2] * operators_[2].level) * env[3];
            feedbackSample_ = op[0];
            return op[3] * operators_[3].level;

        case 4: { // 1->(2+3+4) (one modulator to three carriers)
            op[0] = operators_[0].generateSine(fbPhase) * env[0];
            float mod = op[0] * operators_[0].level;
            op[1] = operators_[1].generateSine(operators_[1].phase + mod) * env[1] * operators_[1].level;
            op[2] = operators_[2].generateSine(operators_[2].phase + mod) * env[2] * operators_[2].level;
            op[3] = operators_[3].generateSine(operators_[3].phase + mod) * env[3] * operators_[3].level;
            feedbackSample_ = op[0];
            return (op[1] + op[2] + op[3]) / 3.0f;
        }

        case 5: // (1->2) + 3 + 4
            op[1] = operators_[1].generateSine(operators_[1].phase) * env[1];
            op[0] = operators_[0].generateSine(fbPhase + op[1] * operators_[1].level) * env[0] * operators_[0].level;
            op[2] = operators_[2].generateSine(operators_[2].phase) * env[2] * operators_[2].level;
            op[3] = operators_[3].generateSine(operators_[3].phase) * env[3] * operators_[3].level;
            feedbackSample_ = op[0];
            return (op[0] + op[2] + op[3]) / 3.0f;

        case 6: // 1->2->3 + 4
            op[2] = operators_[2].generateSine(operators_[2].phase) * env[2];
            op[1] = operators_[1].generateSine(operators_[1].phase + op[2] * operators_[2].level) * env[1];
            op[0] = operators_[0].generateSine(fbPhase + op[1] * operators_[1].level) * env[0] * operators_[0].level;
            op[3] = operators_[3].generateSine(operators_[3].phase) * env[3] * operators_[3].level;
            feedbackSample_ = op[0];
            return (op[0] + op[3]) * 0.5f;

        case 7: { // (1+2+3)->4
            op[0] = operators_[0].generateSine(fbPhase) * env[0] * operators_[0].level;
            op[1] = operators_[1].generateSine(operators_[1].phase) * env[1] * operators_[1].level;
            op[2] = operators_[2].generateSine(operators_[2].phase) * env[2] * operators_[2].level;
            float modSum = (op[0] + op[1] + op[2]) / 3.0f;
            op[3] = operators_[3].generateSine(operators_[3].phase + modSum) * env[3];
            feedbackSample_ = op[0];
            return op[3] * operators_[3].level;
        }

        default:
            return 0.0f;
    }
}
