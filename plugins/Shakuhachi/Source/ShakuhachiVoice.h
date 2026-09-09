#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "ShakuhachiEngine.h"

class ShakuhachiVoice : public juce::SynthesiserVoice
{
public:
    ShakuhachiVoice();

    bool canPlaySound(juce::SynthesiserSound*) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override;
    void stopNote(float, bool allowTailOff) override;
    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    void setBreathPressure(float pressure) { engine_.setBreathPressure(pressure); }
    void setEmbouchure(float emb) { engine_.setEmbouchure(emb); }
    void setAirNoise(float amount) { engine_.setAirNoise(amount); }
    void setAttack(float attack) { engine_.setAttack(attack); }
    void setRelease(float release) { engine_.setRelease(release); }
    void setVibratoRate(float rate) { engine_.setVibratoRate(rate); }
    void setVibratoDepth(float depth) { engine_.setVibratoDepth(depth); }
    void setBrightness(float brightness) { engine_.setBrightness(brightness); }

    void prepareToPlay(double sampleRate, int samplesPerBlock)
    {
        engine_.prepareToPlay(sampleRate, samplesPerBlock);
    }

private:
    ShakuhachiEngine engine_;
    float currentFrequency_ = 0.0f;
    float currentVelocity_ = 0.0f;
    bool isNoteOn_ = false;
    int samplesSinceNoteOn_ = 0;
};

class ShakuhachiSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};
