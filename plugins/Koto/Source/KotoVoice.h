#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "KotoEngine.h"

class KotoVoice : public juce::SynthesiserVoice
{
public:
    KotoVoice();

    bool canPlaySound(juce::SynthesiserSound*) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override;
    void stopNote(float, bool allowTailOff) override;
    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    void setScale(KotoEngine::Scale scale) { engine_.setScale(scale); }
    void setPlayStyle(KotoEngine::PlayStyle style) { engine_.setPlayStyle(style); }
    void setBodyResonance(float amount) { engine_.setBodyResonance(amount); }
    void setAttack(float attack) { engine_.setAttack(attack); }
    void setDecay(float decay) { engine_.setDecay(decay); }
    void setPitchBend(float bend) { engine_.setPitchBend(bend); }
    void setTremoloRate(float rate) { engine_.setTremoloRate(rate); }
    void setTone(float tone) { engine_.setTone(tone); }

    void prepareToPlay(double sampleRate, int samplesPerBlock)
    {
        engine_.prepareToPlay(sampleRate, samplesPerBlock);
    }

private:
    KotoEngine engine_;
    float currentFrequency_ = 0.0f;
    float currentVelocity_ = 0.0f;
    bool isNoteOn_ = false;
    int samplesSinceNoteOn_ = 0;
};

class KotoSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};
