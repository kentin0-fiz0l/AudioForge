#include "Voice.h"
#include <juce_core/juce_core.h>

SynthVoice::SynthVoice()
{
}

void SynthVoice::noteOn(int midiNoteNumber, float noteVelocity, double sampleRate)
{
    active = true;
    currentNote = midiNoteNumber;
    velocity = noteVelocity;

    // Convert MIDI note to frequency: f = 440 * 2^((n-69)/12)
    float frequency = 440.0f * std::pow(2.0f, (midiNoteNumber - 69) / 12.0f);
    oscillator.setFrequency(frequency, sampleRate);

    // Start envelope in attack phase
    envelopeStage = EnvelopeStage::Attack;
    envelopeLevel = 0.0f;
    envelopeTime = 0.0f;
}

void SynthVoice::noteOff()
{
    if (!active)
        return;

    // Enter release phase
    envelopeStage = EnvelopeStage::Release;
    envelopeTime = 0.0f;
}

float SynthVoice::processSample(double sampleRate,
                                int waveform,
                                float attack,
                                float decay,
                                float sustain,
                                float release)
{
    if (!active)
        return 0.0f;

    // Update envelope
    float deltaTime = 1.0f / static_cast<float>(sampleRate);
    updateEnvelope(deltaTime, attack, decay, sustain, release);

    // Generate oscillator sample
    auto waveformEnum = static_cast<Oscillator::Waveform>(waveform);
    float sample = oscillator.getNextSample(waveformEnum);

    // Apply envelope and velocity
    sample *= envelopeLevel * velocity;

    return sample;
}

void SynthVoice::reset()
{
    active = false;
    currentNote = -1;
    velocity = 0.0f;
    envelopeStage = EnvelopeStage::Off;
    envelopeLevel = 0.0f;
    envelopeTime = 0.0f;
    oscillator.reset();
}

void SynthVoice::updateEnvelope(float deltaTime, float attack, float decay,
                               float sustain, float release)
{
    envelopeTime += deltaTime;

    switch (envelopeStage)
    {
        case EnvelopeStage::Off:
            envelopeLevel = 0.0f;
            break;

        case EnvelopeStage::Attack:
            if (attack <= 0.0f)
            {
                // Instant attack
                envelopeLevel = 1.0f;
                envelopeStage = EnvelopeStage::Decay;
                envelopeTime = 0.0f;
            }
            else
            {
                // Linear attack
                envelopeLevel = juce::jmin(1.0f, envelopeTime / attack);

                if (envelopeLevel >= 1.0f)
                {
                    envelopeStage = EnvelopeStage::Decay;
                    envelopeTime = 0.0f;
                }
            }
            break;

        case EnvelopeStage::Decay:
            if (decay <= 0.0f)
            {
                // Instant decay
                envelopeLevel = sustain;
                envelopeStage = EnvelopeStage::Sustain;
                envelopeTime = 0.0f;
            }
            else
            {
                // Exponential decay from 1.0 to sustain level
                float decayAmount = (1.0f - sustain) * (envelopeTime / decay);
                envelopeLevel = 1.0f - juce::jmin(decayAmount, 1.0f - sustain);

                if (envelopeLevel <= sustain)
                {
                    envelopeLevel = sustain;
                    envelopeStage = EnvelopeStage::Sustain;
                    envelopeTime = 0.0f;
                }
            }
            break;

        case EnvelopeStage::Sustain:
            // Hold at sustain level
            envelopeLevel = sustain;
            break;

        case EnvelopeStage::Release:
            if (release <= 0.0f)
            {
                // Instant release
                envelopeLevel = 0.0f;
                active = false;
                envelopeStage = EnvelopeStage::Off;
            }
            else
            {
                // Linear release from current level to 0
                float releaseDecay = envelopeTime / release;
                float startLevel = envelopeLevel; // Capture level at start of release

                // We need to store the starting level - for now use a simple approach
                // TODO: Store release start level in the Voice class
                envelopeLevel = juce::jmax(0.0f, sustain * (1.0f - releaseDecay));

                if (envelopeLevel <= 0.0f || envelopeTime >= release)
                {
                    envelopeLevel = 0.0f;
                    active = false;
                    envelopeStage = EnvelopeStage::Off;
                }
            }
            break;
    }
}
