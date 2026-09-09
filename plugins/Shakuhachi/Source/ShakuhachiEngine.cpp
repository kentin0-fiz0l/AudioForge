#include "ShakuhachiEngine.h"

ShakuhachiEngine::ShakuhachiEngine()
{
    tube_.feedback = 0.98f;
}

void ShakuhachiEngine::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    sampleRate_ = sampleRate;
    reset();
}

void ShakuhachiEngine::reset()
{
    tube_.reset();
    toneLp_.reset();
    breathLp_.reset();

    vibratoPhase_ = 0.0f;
    releaseEnvelope_ = 1.0f;
}

float ShakuhachiEngine::processSample(float baseFrequency, float velocity, bool noteOn, int samplesSinceNoteOn)
{
    // Apply embouchure (meri/kari pitch bend)
    // Meri (lowering) = negative, Kari (raising) = positive
    float pitchBend = embouchure_ * 2.0f; // +/- 2 semitones
    float frequency = baseFrequency * std::pow(2.0f, pitchBend / 12.0f);

    // Apply vibrato
    vibratoPhase_ += vibratoRate_ * juce::MathConstants<float>::twoPi / static_cast<float>(sampleRate_);
    if (vibratoPhase_ > juce::MathConstants<float>::twoPi)
        vibratoPhase_ -= juce::MathConstants<float>::twoPi;

    float vibrato = 1.0f + vibratoDepth_ * std::sin(vibratoPhase_);
    frequency *= vibrato;

    // Update waveguide delay length
    tube_.delayLength = static_cast<float>(sampleRate_) / frequency;

    // Generate breath noise (primary excitation)
    float breathNoise = generateBreathNoise();

    // Filter breath noise to simulate turbulence
    breathLp_.coeff = 0.4f + breathPressure_ * 0.3f;
    float filteredNoise = breathLp_.process(breathNoise);

    // Process through tube waveguide
    float tubeOutput = tube_.process(filteredNoise * breathPressure_ * velocity);

    // Apply tone filtering (brightness)
    toneLp_.coeff = 0.2f + brightness_ * 0.7f;
    float tonedOutput = toneLp_.process(tubeOutput);

    // Mix pure noise with resonated tone (characteristic shakuhachi breathiness)
    float mixed = tonedOutput * (1.0f - airNoise_ * 0.7f) + breathNoise * airNoise_ * 0.3f * velocity;

    // Apply breath attack envelope
    mixed = applyBreathAttack(mixed, samplesSinceNoteOn);

    // Envelope
    float attackEnv = 1.0f;
    if (samplesSinceNoteOn < static_cast<int>(attackTime_ * sampleRate_))
    {
        float progress = static_cast<float>(samplesSinceNoteOn) / (attackTime_ * static_cast<float>(sampleRate_));
        attackEnv = progress;
    }

    if (!noteOn)
    {
        releaseEnvelope_ *= std::pow(0.9f, 1.0f / (releaseTime_ * static_cast<float>(sampleRate_) / 100.0f));
    }
    else
    {
        releaseEnvelope_ = 1.0f;
    }

    return mixed * attackEnv * releaseEnvelope_ * 0.6f;
}

float ShakuhachiEngine::generateBreathNoise()
{
    // Pink-ish noise (filtered white noise for more natural breath sound)
    float white = random_.nextFloat() * 2.0f - 1.0f;

    // Simple pink noise approximation
    static float b0 = 0.0f, b1 = 0.0f, b2 = 0.0f, b3 = 0.0f, b4 = 0.0f, b5 = 0.0f, b6 = 0.0f;
    b0 = 0.99886f * b0 + white * 0.0555179f;
    b1 = 0.99332f * b1 + white * 0.0750759f;
    b2 = 0.96900f * b2 + white * 0.1538520f;
    b3 = 0.86650f * b3 + white * 0.3104856f;
    b4 = 0.55000f * b4 + white * 0.5329522f;
    b5 = -0.7616f * b5 - white * 0.0168980f;
    float pink = b0 + b1 + b2 + b3 + b4 + b5 + b6 + white * 0.5362f;
    b6 = white * 0.115926f;

    return pink * 0.11f;
}

float ShakuhachiEngine::applyBreathAttack(float sample, int samplesSinceNoteOn)
{
    // Shakuhachi has a characteristic breath attack where the first ~200ms
    // is mostly air noise before the tone stabilizes
    float attackSamples = attackTime_ * static_cast<float>(sampleRate_);

    if (samplesSinceNoteOn < static_cast<int>(attackSamples))
    {
        float progress = static_cast<float>(samplesSinceNoteOn) / attackSamples;

        // During attack, emphasize breath noise
        float breathAmount = 1.0f - progress;
        float breathNoise = generateBreathNoise();

        return sample * progress + breathNoise * breathAmount * 0.5f;
    }

    return sample;
}
