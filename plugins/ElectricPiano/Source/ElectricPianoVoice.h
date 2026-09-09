#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "ElectricPianoEngine.h"

/**
 * ElectricPianoVoice - JUCE synthesiser voice for electric piano
 *
 * Wraps the ElectricPianoEngine in a JUCE voice for polyphonic playback
 */
class ElectricPianoVoice : public juce::SynthesiserVoice
{
public:
    ElectricPianoVoice();

    bool canPlaySound(juce::SynthesiserSound*) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    // Set engine parameters (called from processor)
    void setModel(ElectricPianoEngine::EPModel model) { engine_.setModel(model); }
    void setTone(float tone) { engine_.setTone(tone); }
    void setDecay(float decay) { engine_.setDecay(decay); }
    void setMechanicalNoise(float amount) { engine_.setMechanicalNoise(amount); }
    void setVelocityCurve(float curve) { engine_.setVelocityCurve(curve); }
    void setTremoloDepth(float depth) { engine_.setTremoloDepth(depth); }
    void setTremoloRate(float rate) { engine_.setTremoloRate(rate); }
    void setChorusDepth(float depth) { engine_.setChorusDepth(depth); }
    void setReverbMix(float mix) { engine_.setReverbMix(mix); }

    void prepareToPlay(double sampleRate, int samplesPerBlock)
    {
        engine_.prepareToPlay(sampleRate, samplesPerBlock);
    }

private:
    ElectricPianoEngine engine_;
    float currentFrequency_ = 0.0f;
    float currentVelocity_ = 0.0f;
    bool isNoteOn_ = false;
};

/**
 * ElectricPianoSound - Dummy sound class (required by JUCE)
 */
class ElectricPianoSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};
