#include "CompressorEngine.h"

CompressorEngine::CompressorEngine() {}

void CompressorEngine::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    sampleRate_ = sampleRate;
    reset();
}

void CompressorEngine::reset()
{
    envelope_ = 0.0f;
    gainReduction_ = 0.0f;
}

void CompressorEngine::processBlock(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    float attackCoeff = getAttackCoeff();
    float releaseCoeff = getReleaseCoeff();

    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Detect peak level across all channels
        float peakLevel = 0.0f;
        for (int ch = 0; ch < numChannels; ++ch)
        {
            float absSample = std::abs(buffer.getSample(ch, sample));
            peakLevel = std::max(peakLevel, absSample);
        }

        // Convert to dB
        float inputDB = peakLevel > 0.0001f ? 20.0f * std::log10(peakLevel) : -100.0f;

        // Envelope follower
        if (inputDB > envelope_)
            envelope_ = attackCoeff * (envelope_ - inputDB) + inputDB;
        else
            envelope_ = releaseCoeff * (envelope_ - inputDB) + inputDB;

        // Compute gain reduction
        float gainDB = computeGain(envelope_);
        gainReduction_ = -gainDB;

        // Apply makeup gain
        float totalGainDB = gainDB + makeupGain_;
        float gain = std::pow(10.0f, totalGainDB / 20.0f);

        // Apply gain to all channels with mix control
        for (int ch = 0; ch < numChannels; ++ch)
        {
            float dry = buffer.getSample(ch, sample);
            float wet = dry * gain;
            buffer.setSample(ch, sample, dry * (1.0f - mix_) + wet * mix_);
        }
    }
}

float CompressorEngine::computeGain(float inputLevel)
{
    if (inputLevel < threshold_ - knee_ / 2.0f)
    {
        // Below threshold - no compression
        return 0.0f;
    }
    else if (inputLevel > threshold_ + knee_ / 2.0f)
    {
        // Above knee - full compression
        float overshoot = inputLevel - threshold_;
        return overshoot * (1.0f / ratio_ - 1.0f);
    }
    else
    {
        // In knee region - soft knee
        float kneeInput = inputLevel - threshold_ + knee_ / 2.0f;
        float kneeOutput = kneeInput * kneeInput / (2.0f * knee_);
        return kneeOutput * (1.0f / ratio_ - 1.0f);
    }
}

float CompressorEngine::getAttackCoeff()
{
    // Mode-specific attack characteristics
    float attackMs = attack_;

    switch (mode_)
    {
        case Mode::VCA:
            // Fast, clean attack
            break;
        case Mode::FET:
            // Very fast attack
            attackMs *= 0.5f;
            break;
        case Mode::Opto:
            // Slow, smooth attack
            attackMs *= 2.0f;
            break;
    }

    float attackTime = attackMs / 1000.0f;
    return std::exp(-1.0f / (attackTime * static_cast<float>(sampleRate_)));
}

float CompressorEngine::getReleaseCoeff()
{
    // Mode-specific release characteristics
    float releaseMs = release_;

    switch (mode_)
    {
        case Mode::VCA:
            // Clean, controlled release
            break;
        case Mode::FET:
            // Fast release
            releaseMs *= 0.7f;
            break;
        case Mode::Opto:
            // Very slow, musical release
            releaseMs *= 1.5f;
            break;
    }

    float releaseTime = releaseMs / 1000.0f;
    return std::exp(-1.0f / (releaseTime * static_cast<float>(sampleRate_)));
}
