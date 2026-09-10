#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "KickEngine.h"

class KickVoice : public juce::SynthesiserVoice
{
public:
    KickVoice();
    bool canPlaySound(juce::SynthesiserSound*) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override;
    void stopNote(float, bool allowTailOff) override;
    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    void setPitch(float pitch) { engine_.setPitch(pitch); }
    void setPitchEnvAmount(float amount) { engine_.setPitchEnvAmount(amount); }
    void setPitchEnvDecay(float decay) { engine_.setPitchEnvDecay(decay); }
    void setDecay(float decay) { engine_.setDecay(decay); }
    void setClick(float click) { engine_.setClick(click); }
    void setDrive(float drive) { engine_.setDrive(drive); }
    void setTone(float tone) { engine_.setTone(tone); }

    void prepareToPlay(double sampleRate, int samplesPerBlock) { engine_.prepareToPlay(sampleRate, samplesPerBlock); }

private:
    KickEngine engine_;
};

class KickSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};
