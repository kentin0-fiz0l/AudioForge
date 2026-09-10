#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "FMEngine.h"

class FMVoice : public juce::SynthesiserVoice
{
public:
    FMVoice();
    bool canPlaySound(juce::SynthesiserSound*) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override;
    void stopNote(float, bool allowTailOff) override;
    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    void setAlgorithm(int algo) { engine_.setAlgorithm(algo); }
    void setOpLevel(int op, float level) { engine_.setOpLevel(op, level); }
    void setOpRatio(int op, float ratio) { engine_.setOpRatio(op, ratio); }
    void setOpAttack(int op, float attack) { engine_.setOpAttack(op, attack); }
    void setOpDecay(int op, float decay) { engine_.setOpDecay(op, decay); }
    void setOpSustain(int op, float sustain) { engine_.setOpSustain(op, sustain); }
    void setOpRelease(int op, float release) { engine_.setOpRelease(op, release); }
    void setFeedback(float fb) { engine_.setFeedback(fb); }

    void prepareToPlay(double sampleRate, int samplesPerBlock) { engine_.prepareToPlay(sampleRate, samplesPerBlock); }

private:
    FMEngine engine_;
};

class FMSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};
