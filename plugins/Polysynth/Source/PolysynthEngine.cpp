#include "PolysynthEngine.h"

PolysynthEngine::PolysynthEngine()
{
    // Initialize unison detune amounts (spread voices evenly)
    for (int i = 0; i < MAX_UNISON_VOICES; ++i)
    {
        float spread = (i - (MAX_UNISON_VOICES - 1) * 0.5f) / (MAX_UNISON_VOICES - 1);
        unisonDetuneAmounts_[i] = spread; // -0.5 to +0.5
    }

    lpFilter_.isHighpass = false;
    hpFilter_.isHighpass = true;
}

void PolysynthEngine::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    sampleRate_ = sampleRate;
    envelope_.sampleRate = sampleRate;
    reset();
}

void PolysynthEngine::reset()
{
    for (int v = 0; v < MAX_UNISON_VOICES; ++v)
    {
        unisonPhases_[v][0] = 0.0f;
        unisonPhases_[v][1] = 0.0f;
    }

    lpFilter_.reset();
    hpFilter_.reset();
    envelope_.reset();
    chorusPhase_ = 0.0f;
}

void PolysynthEngine::noteOn(float frequency, float velocity)
{
    baseFrequency_ = frequency;
    velocity_ = velocity;
    envelope_.trigger();
}

void PolysynthEngine::noteOff()
{
    envelope_.release_();
}

float PolysynthEngine::processSample()
{
    float envValue = envelope_.process();

    // Generate unison voices
    float unisonSum = 0.0f;

    for (int v = 0; v < unisonVoices_; ++v)
    {
        // Apply unison detune
        float detuneCents = unisonDetuneAmounts_[v] * unisonDetune_;
        float detuneRatio = std::pow(2.0f, detuneCents / 1200.0f);

        // Osc 1: Sawtooth
        float freq1 = baseFrequency_ * detuneRatio;
        float osc1 = generateOscillator(unisonPhases_[v][0], freq1, true);

        // Osc 2: Square (detuned)
        float freq2 = baseFrequency_ * std::pow(2.0f, detune_ / 12.0f) * detuneRatio;
        float osc2 = generateOscillator(unisonPhases_[v][1], freq2, false);

        // Mix oscillators
        unisonSum += osc1 * (1.0f - oscMix_) + osc2 * oscMix_;
    }

    // Average unison voices
    float mixed = unisonSum / static_cast<float>(unisonVoices_);

    // Apply chorus modulation
    if (chorusDepth_ > 0.0f)
    {
        chorusPhase_ += chorusRate_ * juce::MathConstants<float>::twoPi / static_cast<float>(sampleRate_);
        if (chorusPhase_ > juce::MathConstants<float>::twoPi)
            chorusPhase_ -= juce::MathConstants<float>::twoPi;

        float chorusMod = std::sin(chorusPhase_) * chorusDepth_;
        mixed *= (1.0f + chorusMod * 0.1f);
    }

    // Cascaded filtering (LP then HP)
    lpFilter_.coeff = juce::jlimit(0.01f, 0.99f, filterCutoff_ / static_cast<float>(sampleRate_) * 2.0f);
    hpFilter_.coeff = lpFilter_.coeff * 0.5f; // HP at lower frequency

    float lpOut = lpFilter_.process(mixed);
    float hpOut = hpFilter_.process(mixed);

    // Mix LP and HP based on filterMix
    float filtered = lpOut * (1.0f - filterMix_) + hpOut * filterMix_;

    // Apply simple resonance boost
    if (filterResonance_ > 0.0f)
    {
        static float feedbackSample = 0.0f;
        filtered += feedbackSample * filterResonance_ * 0.7f;
        feedbackSample = filtered * 0.5f;
    }

    return filtered * envValue * velocity_ * 0.3f;
}

float PolysynthEngine::generateOscillator(float& phase, float frequency, bool isSaw)
{
    float increment = frequency * juce::MathConstants<float>::twoPi / static_cast<float>(sampleRate_);
    phase += increment;

    if (phase > juce::MathConstants<float>::twoPi)
        phase -= juce::MathConstants<float>::twoPi;

    float t = phase / juce::MathConstants<float>::twoPi;

    if (isSaw)
    {
        // Sawtooth
        return 2.0f * t - 1.0f;
    }
    else
    {
        // Square
        return (t < 0.5f) ? 1.0f : -1.0f;
    }
}
