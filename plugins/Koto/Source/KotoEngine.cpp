#include "KotoEngine.h"

KotoEngine::KotoEngine()
{
    // Initialize body resonance filters (paulownia wood formants)
    bodyFilter1_.frequency = 250.0f;
    bodyFilter1_.q = 3.0f;
    bodyFilter1_.gain = 2.0f;

    bodyFilter2_.frequency = 800.0f;
    bodyFilter2_.q = 2.5f;
    bodyFilter2_.gain = 1.5f;

    bodyFilter3_.frequency = 2000.0f;
    bodyFilter3_.q = 2.0f;
    bodyFilter3_.gain = 1.3f;
}

void KotoEngine::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    sampleRate_ = sampleRate;

    bodyFilter1_.updateCoefficients(sampleRate);
    bodyFilter2_.updateCoefficients(sampleRate);
    bodyFilter3_.updateCoefficients(sampleRate);

    reset();
}

void KotoEngine::reset()
{
    string_.reset();
    bodyFilter1_.reset();
    bodyFilter2_.reset();
    bodyFilter3_.reset();

    tremoloPhase_ = 0.0f;
    lastTremoloTrigger_ = 0;
    toneLp_ = 0.0f;
}

float KotoEngine::processSample(float baseFrequency, float velocity, bool noteOn, int samplesSinceNoteOn)
{
    // Apply pitch bend
    float frequency = baseFrequency * std::pow(2.0f, pitchBendAmount_ / 12.0f);

    // Update string delay length
    float delayLength = static_cast<float>(sampleRate_) / frequency;
    string_.delayLength = delayLength;
    // Longer decay = higher feedback. The Decay control runs up to 2.0, and
    // without the limit this mapping passes 1.0 at a decay of 1.5.
    string_.feedback = juce::jmin(MAX_STRING_FEEDBACK, 0.994f + decayTime_ * 0.004f);

    // Update tone filter coefficient
    toneCoeff_ = 0.3f + tone_ * 0.6f;

    // Excitation signal
    float excitation = 0.0f;

    // Handle different play styles
    switch (playStyle_)
    {
        case PlayStyle::Pluck:
            // Single pluck at note start
            if (noteOn && samplesSinceNoteOn < 10)
            {
                excitation = (random_.nextFloat() * 2.0f - 1.0f) * velocity;
            }
            break;

        case PlayStyle::Tremolo:
            // Rapid re-plucking
            {
                tremoloPhase_ += tremoloRate_ / static_cast<float>(sampleRate_);
                if (tremoloPhase_ >= 1.0f)
                {
                    tremoloPhase_ -= 1.0f;
                    lastTremoloTrigger_ = 0;
                }

                // Short pluck burst every tremolo cycle
                if (noteOn && lastTremoloTrigger_ < 5)
                {
                    // The string is still ringing from the previous plucks when
                    // the next one arrives. Scale each pluck by how much of the
                    // last one is left, so the level settles at that of a single
                    // pluck instead of piling up.
                    const float tripsPerPluck = frequency / juce::jmax(0.1f, tremoloRate_);
                    const float retained = std::pow(string_.feedback, tripsPerPluck);

                    excitation = (random_.nextFloat() * 2.0f - 1.0f) * velocity * 0.7f
                                 * std::sqrt(1.0f - retained * retained);
                    lastTremoloTrigger_++;
                }
            }
            break;

        case PlayStyle::Scrape:
            // Continuous noisy excitation
            if (noteOn)
            {
                // Noise fed in continuously builds up in the string by a factor
                // of 1 / sqrt(1 - feedback^2). Divide that out so the level is
                // the same at every decay setting: an RMS of 0.15 at full
                // velocity (0.26 is 0.15 * sqrt(3), uniform noise having an RMS
                // of 1 / sqrt(3)).
                const float g = string_.feedback;

                excitation = (random_.nextFloat() * 2.0f - 1.0f) * velocity * 0.26f
                             * std::sqrt(1.0f - g * g);
            }
            break;
    }

    // Process string
    float stringOutput = string_.process(excitation);

    // Apply tone filtering (simulate pick position)
    toneLp_ += toneCoeff_ * (stringOutput - toneLp_);
    float tonedOutput = toneLp_;

    // Apply body resonance
    float finalOutput = tonedOutput;
    applyBodyResonance(finalOutput);

    // Envelope
    float attackEnv = 1.0f;
    if (samplesSinceNoteOn < static_cast<int>(attackTime_ * sampleRate_))
    {
        attackEnv = static_cast<float>(samplesSinceNoteOn) / (attackTime_ * static_cast<float>(sampleRate_));
    }

    float decayEnv = noteOn ? 1.0f : std::pow(0.9995f, static_cast<float>(samplesSinceNoteOn));

    return finalOutput * attackEnv * decayEnv * 0.8f;
}

void KotoEngine::applyBodyResonance(float& sample)
{
    if (bodyResonance_ > 0.0f)
    {
        // Cascade three formant filters
        float resonant = sample;
        resonant = bodyFilter1_.process(resonant);
        resonant = bodyFilter2_.process(resonant);
        resonant = bodyFilter3_.process(resonant);

        // Mix wet/dry based on body resonance amount
        sample = sample * (1.0f - bodyResonance_ * 0.6f) + resonant * bodyResonance_ * 0.6f;
    }
}
