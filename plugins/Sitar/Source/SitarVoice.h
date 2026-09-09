#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "SitarEngine.h"

class SitarVoice : public juce::SynthesiserVoice
{
public:
    SitarVoice();

    bool canPlaySound(juce::SynthesiserSound*) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override;
    void stopNote(float, bool allowTailOff) override;
    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    void setRagaScale(SitarEngine::RagaScale scale) { engine_.setRagaScale(scale); }
    void setJawariAmount(float amount) { engine_.setJawariAmount(amount); }
    void setSympatheticResonance(float resonance) { engine_.setSympatheticResonance(resonance); }
    void setAttack(float attack) { engine_.setAttack(attack); }
    void setDecay(float decay) { engine_.setDecay(decay); }
    void setPitchBend(float bend) { engine_.setPitchBend(bend); }
    void setVibratoRate(float rate) { engine_.setVibratoRate(rate); }
    void setVibratoDepth(float depth) { engine_.setVibratoDepth(depth); }
    void setDroneLevel(float level) { engine_.setDroneLevel(level); }

    void prepareToPlay(double sampleRate, int samplesPerBlock)
    {
        engine_.prepareToPlay(sampleRate, samplesPerBlock);
    }

private:
    SitarEngine engine_;
    float currentFrequency_ = 0.0f;
    float currentVelocity_ = 0.0f;
    bool isNoteOn_ = false;
    int samplesSinceNoteOn_ = 0;
};

class SitarSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};
