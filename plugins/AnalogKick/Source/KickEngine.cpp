#include "KickEngine.h"

KickEngine::KickEngine() {}

void KickEngine::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    sampleRate_ = sampleRate;
    reset();
}

void KickEngine::reset()
{
    phase_ = 0.0f;
    ampEnvelope_ = 0.0f;
    pitchEnvelope_ = 0.0f;
    clickEnvelope_ = 0.0f;
    active_ = false;
    lpFilter_ = 0.0f;
    hpFilter_ = 0.0f;
}

void KickEngine::trigger(float velocity)
{
    velocity_ = velocity;
    active_ = true;
    phase_ = 0.0f;
    ampEnvelope_ = 1.0f;
    pitchEnvelope_ = 1.0f;
    clickEnvelope_ = 1.0f;
    noisePhase_ = 0.0f;
}

void KickEngine::release()
{
    // Kick is one-shot, release does nothing
}

float KickEngine::processSample()
{
    if (!active_)
        return 0.0f;

    // Exponential decay envelopes
    float ampDecayRate = 1.0f / (decay_ * static_cast<float>(sampleRate_));
    ampEnvelope_ *= (1.0f - ampDecayRate);

    float pitchDecayRate = 1.0f / (pitchEnvDecay_ * static_cast<float>(sampleRate_));
    pitchEnvelope_ *= (1.0f - pitchDecayRate);

    float clickDecayRate = 1.0f / (0.005f * static_cast<float>(sampleRate_)); // 5ms click
    clickEnvelope_ *= (1.0f - clickDecayRate);

    if (ampEnvelope_ < 0.001f)
    {
        active_ = false;
        return 0.0f;
    }

    // Pitch envelope: starts high, drops to fundamental
    float currentPitch = pitch_ * (1.0f + pitchEnvAmount_ * pitchEnvelope_);
    float increment = currentPitch * juce::MathConstants<float>::twoPi / static_cast<float>(sampleRate_);
    phase_ += increment;
    if (phase_ > juce::MathConstants<float>::twoPi)
        phase_ -= juce::MathConstants<float>::twoPi;

    // Body: sine wave
    float body = std::sin(phase_);

    // Click: short noise burst
    float click = generateNoise() * clickEnvelope_ * clickLevel_;

    // Mix body + click
    float sample = (body * (1.0f - tone_ * 0.5f)) + click;

    // Apply amplitude envelope
    sample *= ampEnvelope_ * velocity_;

    // Drive/saturation
    sample = softClip(sample * drive_);

    // Gentle low-pass to smooth harsh transients
    lpFilter_ += 0.3f * (sample - lpFilter_);
    sample = lpFilter_;

    // Subtle high-pass to remove DC
    hpFilter_ += 0.005f * (sample - hpFilter_);
    sample -= hpFilter_;

    return sample * 0.7f;
}

float KickEngine::generateNoise()
{
    noisePhase_ = noisePhase_ * 1103515245 + 12345;
    int temp = static_cast<int>(noisePhase_ / 65536) % 32768;
    return (temp / 16384.0f) - 1.0f;
}

float KickEngine::softClip(float sample)
{
    return std::tanh(sample);
}
