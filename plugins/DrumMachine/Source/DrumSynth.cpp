/*
  DrumSynth.cpp - Synthesized Drum Sounds Implementation
*/

#include "DrumSynth.h"
#include <cmath>

DrumSynth::DrumSynth()
    : noiseDist_(-1.0f, 1.0f)
{
    std::random_device rd;
    noiseGen_.seed(rd());
}

void DrumSynth::prepare(double sampleRate, int samplesPerBlock)
{
    sampleRate_ = sampleRate;

    // Reset all voices
    for (auto& voice : voices_) {
        voice.active = false;
        voice.phase = 0.0f;
        voice.envPhase = 0.0f;
    }
}

void DrumSynth::triggerDrum(DrumEngine::DrumVoice voiceType, float velocity)
{
    auto& voice = voices_[voiceType];

    voice.active = true;
    voice.phase = 0.0f;
    voice.envPhase = 0.0f;
    voice.velocity = velocity;

    // Set pitch parameters for each drum
    switch (voiceType) {
        case DrumEngine::KICK:
            voice.pitchStart = 150.0f;  // Hz
            voice.pitchEnd = 50.0f;
            break;
        case DrumEngine::SNARE:
            voice.pitchStart = 200.0f;
            voice.pitchEnd = 180.0f;
            break;
        case DrumEngine::TOM:
            voice.pitchStart = 120.0f;
            voice.pitchEnd = 100.0f;
            break;
        default:
            voice.pitchStart = 0.0f;
            voice.pitchEnd = 0.0f;
            break;
    }
}

float DrumSynth::envelope(float phase, float attack, float decay)
{
    if (phase < attack) {
        return phase / attack;
    } else {
        float decayPhase = (phase - attack) / decay;
        return std::exp(-5.0f * decayPhase);  // Exponential decay
    }
}

float DrumSynth::noise()
{
    return noiseDist_(noiseGen_);
}

float DrumSynth::generateKick(VoiceState& voice)
{
    // Kick: sine wave with pitch envelope
    float attack = 0.001f;
    float decay = 0.3f;
    float env = envelope(voice.envPhase, attack, decay);

    // Pitch envelope (start high, sweep down)
    float t = voice.envPhase / decay;
    float currentPitch = voice.pitchStart + (voice.pitchEnd - voice.pitchStart) * t;

    // Generate sine wave
    float sample = std::sin(voice.phase) * env * voice.velocity * 0.8f;

    // Update phase
    float phaseIncrement = (currentPitch / static_cast<float>(sampleRate_)) * 2.0f * juce::MathConstants<float>::pi;
    voice.phase += phaseIncrement;

    return sample;
}

float DrumSynth::generateSnare(VoiceState& voice)
{
    // Snare: noise + tone
    float attack = 0.001f;
    float decay = 0.15f;
    float env = envelope(voice.envPhase, attack, decay);

    // Tone component (200Hz)
    float tone = std::sin(voice.phase) * 0.3f;

    // Noise component
    float noiseVal = noise() * 0.7f;

    float sample = (tone + noiseVal) * env * voice.velocity * 0.6f;

    // Update phase
    float phaseIncrement = (voice.pitchStart / static_cast<float>(sampleRate_)) * 2.0f * juce::MathConstants<float>::pi;
    voice.phase += phaseIncrement;

    return sample;
}

float DrumSynth::generateHihat(VoiceState& voice)
{
    // Hihat: filtered noise
    float attack = 0.0005f;
    float decay = 0.05f;  // Very short
    float env = envelope(voice.envPhase, attack, decay);

    // High-passed noise
    float noiseVal = noise();

    float sample = noiseVal * env * voice.velocity * 0.4f;

    return sample;
}

float DrumSynth::generateTom(VoiceState& voice)
{
    // Tom: pitched tone with decay
    float attack = 0.001f;
    float decay = 0.2f;
    float env = envelope(voice.envPhase, attack, decay);

    // Pitch envelope
    float t = voice.envPhase / decay;
    float currentPitch = voice.pitchStart + (voice.pitchEnd - voice.pitchStart) * t;

    // Generate tone
    float sample = std::sin(voice.phase) * env * voice.velocity * 0.7f;

    // Update phase
    float phaseIncrement = (currentPitch / static_cast<float>(sampleRate_)) * 2.0f * juce::MathConstants<float>::pi;
    voice.phase += phaseIncrement;

    return sample;
}

float DrumSynth::generateCrash(VoiceState& voice)
{
    // Crash: filtered noise with long decay
    float attack = 0.001f;
    float decay = 0.8f;
    float env = envelope(voice.envPhase, attack, decay);

    // Filtered noise
    float noiseVal = noise();

    float sample = noiseVal * env * voice.velocity * 0.5f;

    return sample;
}

void DrumSynth::renderNextBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    float sampleIncrement = 1.0f / static_cast<float>(sampleRate_);

    for (int i = 0; i < numSamples; ++i) {
        float mixedSample = 0.0f;

        // Render all active voices
        for (size_t v = 0; v < voices_.size(); ++v) {
            auto& voice = voices_[v];

            if (!voice.active)
                continue;

            float sample = 0.0f;

            switch (static_cast<DrumEngine::DrumVoice>(v)) {
                case DrumEngine::KICK:
                    sample = generateKick(voice);
                    break;
                case DrumEngine::SNARE:
                    sample = generateSnare(voice);
                    break;
                case DrumEngine::HIHAT:
                    sample = generateHihat(voice);
                    break;
                case DrumEngine::TOM:
                    sample = generateTom(voice);
                    break;
                case DrumEngine::CRASH:
                    sample = generateCrash(voice);
                    break;
                default:
                    break;
            }

            mixedSample += sample;

            // Update envelope phase
            voice.envPhase += sampleIncrement;

            // Deactivate voice if envelope finished
            if (voice.envPhase > 1.0f) {
                voice.active = false;
            }
        }

        // Output to all channels
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
            buffer.addSample(channel, startSample + i, mixedSample * 0.5f);
        }
    }
}
