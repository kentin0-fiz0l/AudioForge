#include "SnareEngine.h"

SnareEngine::SnareEngine() {}

void SnareEngine::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    sampleRate_ = sampleRate;
    reset();
}

void SnareEngine::reset()
{
    tonePhase_ = 0.0f;
    bodyEnvelope_ = 0.0f;
    snareEnvelope_ = 0.0f;
    pitchEnvelope_ = 0.0f;
    snapEnvelope_ = 0.0f;
    active_ = false;
    bpFilter1_ = bpFilter2_ = hpFilter_ = 0.0f;
}

void SnareEngine::trigger(float velocity)
{
    velocity_ = velocity;
    active_ = true;
    tonePhase_ = 0.0f;
    bodyEnvelope_ = 1.0f;
    snareEnvelope_ = 1.0f;
    pitchEnvelope_ = 1.0f;
    snapEnvelope_ = 1.0f;
}

void SnareEngine::release()
{
    // Snare is one-shot
}

float SnareEngine::processSample()
{
    if (!active_)
        return 0.0f;

    // Exponential decay envelopes
    float bodyDecayRate = 1.0f / (decay_ * static_cast<float>(sampleRate_));
    bodyEnvelope_ *= (1.0f - bodyDecayRate);

    float snareDecayRate = 1.0f / (snareDecay_ * static_cast<float>(sampleRate_));
    snareEnvelope_ *= (1.0f - snareDecayRate);

    float pitchDecayRate = 1.0f / (0.03f * static_cast<float>(sampleRate_)); // 30ms pitch decay
    pitchEnvelope_ *= (1.0f - pitchDecayRate);

    float snapDecayRate = 1.0f / (0.002f * static_cast<float>(sampleRate_)); // 2ms snap
    snapEnvelope_ *= (1.0f - snapDecayRate);

    if (bodyEnvelope_ < 0.001f && snareEnvelope_ < 0.001f)
    {
        active_ = false;
        return 0.0f;
    }

    // Body tone: triangle wave with pitch envelope
    float currentPitch = tone_ * (1.0f + pitchEnv_ * pitchEnvelope_);
    float increment = currentPitch * juce::MathConstants<float>::twoPi / static_cast<float>(sampleRate_);
    tonePhase_ += increment;
    if (tonePhase_ > juce::MathConstants<float>::twoPi)
        tonePhase_ -= juce::MathConstants<float>::twoPi;

    // Triangle wave
    float phase01 = tonePhase_ / juce::MathConstants<float>::twoPi;
    float triangle = (phase01 < 0.5f) ? (4.0f * phase01 - 1.0f) : (3.0f - 4.0f * phase01);
    float body = triangle * bodyEnvelope_;

    // Snare rattle: filtered noise (bandpass around 3-5kHz)
    float noise = generateNoise();

    // Bandpass filter (two-stage for sharper peak)
    float bp1Coeff = 0.15f;
    bpFilter1_ += bp1Coeff * (noise - bpFilter1_);
    bpFilter2_ += bp1Coeff * (bpFilter1_ - bpFilter2_);
    float bandpass = bpFilter1_ - bpFilter2_;

    // High-pass to emphasize sizzle
    float hpCoeff = 0.995f;
    hpFilter_ = hpCoeff * (hpFilter_ + bandpass - noise);
    float snare = hpFilter_ * snareEnvelope_ * snappy_;

    // Snap: brief noise burst
    float snapNoise = generateNoise() * snapEnvelope_ * snap_;

    // Mix
    float sample = (body * 0.6f) + (snare * 0.7f) + (snapNoise * 0.4f);
    sample *= velocity_;

    return std::tanh(sample * 1.5f) * 0.5f;
}

float SnareEngine::generateNoise()
{
    noiseState_ = noiseState_ * 1103515245 + 12345;
    int temp = static_cast<int>(noiseState_ / 65536) % 32768;
    return (temp / 16384.0f) - 1.0f;
}
