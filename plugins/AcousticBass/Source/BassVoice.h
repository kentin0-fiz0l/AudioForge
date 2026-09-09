#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "BassEngine.h"

class BassVoice : public juce::SynthesiserVoice
{
public:
    BassVoice();

    bool canPlaySound(juce::SynthesiserSound*) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override;
    void stopNote(float, bool allowTailOff) override;
    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    void setBassType(BassEngine::BassType type) { engine_.setBassType(type); }
    void setPlayStyle(BassEngine::PlayStyle style) { engine_.setPlayStyle(style); }
    void setTone(float tone) { engine_.setTone(tone); }
    void setAttack(float attack) { engine_.setAttack(attack); }
    void setDecay(float decay) { engine_.setDecay(decay); }
    void setFretNoise(float amount) { engine_.setFretNoise(amount); }
    void setSlideAmount(float amount) { engine_.setSlideAmount(amount); }

    void prepareToPlay(double sampleRate, int samplesPerBlock)
    {
        engine_.prepareToPlay(sampleRate, samplesPerBlock);
    }

private:
    BassEngine engine_;
    float currentFrequency_ = 0.0f;
    float currentVelocity_ = 0.0f;
    bool isNoteOn_ = false;
    int samplesSinceNoteOn_ = 0;
};

class BassSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};
