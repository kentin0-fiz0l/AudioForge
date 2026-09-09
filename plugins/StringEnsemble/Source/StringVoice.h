#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "StringEngine.h"

class StringVoice : public juce::SynthesiserVoice
{
public:
    StringVoice();

    bool canPlaySound(juce::SynthesiserSound*) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    void setStringType(StringEngine::StringType type) { engine_.setStringType(type); }
    void setBrightness(float brightness) { engine_.setBrightness(brightness); }
    void setAttack(float attack) { engine_.setAttack(attack); }
    void setDecay(float decay) { engine_.setDecay(decay); }
    void setVibratoDepth(float depth) { engine_.setVibratoDepth(depth); }
    void setVibratoRate(float rate) { engine_.setVibratoRate(rate); }
    void setEnsembleSize(int size) { engine_.setEnsembleSize(size); }

    void prepareToPlay(double sampleRate, int samplesPerBlock)
    {
        engine_.prepareToPlay(sampleRate, samplesPerBlock);
    }

private:
    StringEngine engine_;
    float currentFrequency_ = 0.0f;
    float currentVelocity_ = 0.0f;
    bool isNoteOn_ = false;
    int samplesSinceNoteOn_ = 0;
};

class StringSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};
