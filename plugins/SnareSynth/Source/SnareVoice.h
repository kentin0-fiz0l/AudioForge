#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "SnareEngine.h"

class SnareVoice : public juce::SynthesiserVoice
{
public:
    SnareVoice();
    bool canPlaySound(juce::SynthesiserSound*) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override;
    void stopNote(float, bool allowTailOff) override;
    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    void setTone(float tone) { engine_.setTone(tone); }
    void setSnappy(float snappy) { engine_.setSnappy(snappy); }
    void setDecay(float decay) { engine_.setDecay(decay); }
    void setSnareDecay(float decay) { engine_.setSnareDecay(decay); }
    void setPitchEnv(float env) { engine_.setPitchEnv(env); }
    void setSnap(float snap) { engine_.setSnap(snap); }

    void prepareToPlay(double sampleRate, int samplesPerBlock) { engine_.prepareToPlay(sampleRate, samplesPerBlock); }

private:
    SnareEngine engine_;
};

class SnareSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};
