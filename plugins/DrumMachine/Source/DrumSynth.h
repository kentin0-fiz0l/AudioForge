/*
  DrumSynth.h - Synthesized Drum Sounds
*/

#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>
#include "DrumEngine.h"

class DrumSynth
{
public:
    DrumSynth();

    void prepare(double sampleRate, int samplesPerBlock);
    void triggerDrum(DrumEngine::DrumVoice voice, float velocity);
    void renderNextBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples);

private:
    double sampleRate_ = 44100.0;

    // Voice states
    struct VoiceState {
        bool active = false;
        float phase = 0.0f;
        float envPhase = 0.0f;
        float velocity = 0.0f;
        float pitchStart = 0.0f;
        float pitchEnd = 0.0f;
    };

    std::array<VoiceState, DrumEngine::NUM_VOICES> voices_;

    // Synthesis helpers
    float generateKick(VoiceState& voice);
    float generateSnare(VoiceState& voice);
    float generateHihat(VoiceState& voice);
    float generateTom(VoiceState& voice);
    float generateCrash(VoiceState& voice);

    float envelope(float phase, float attack, float decay);
    float noise();

    std::mt19937 noiseGen_;
    std::uniform_real_distribution<float> noiseDist_;
};
