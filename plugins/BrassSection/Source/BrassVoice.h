#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "BrassEngine.h"

/**
 * BrassVoice - JUCE synthesiser voice for brass section
 *
 * Wraps the BrassEngine in a JUCE voice for polyphonic playback
 */
class BrassVoice : public juce::SynthesiserVoice
{
public:
    BrassVoice();

    bool canPlaySound(juce::SynthesiserSound*) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    // Set engine parameters (called from processor)
    void setInstrumentType(BrassEngine::InstrumentType type) { engine_.setInstrumentType(type); }
    void setArticulation(BrassEngine::Articulation articulation) { engine_.setArticulation(articulation); }
    void setBrightness(float brightness) { engine_.setBrightness(brightness); }
    void setBreathNoise(float amount) { engine_.setBreathNoise(amount); }
    void setVibratoDepth(float depth) { engine_.setVibratoDepth(depth); }
    void setVibratoRate(float rate) { engine_.setVibratoRate(rate); }
    void setExpression(float expression) { engine_.setExpression(expression); }
    void setSectionSize(int size) { engine_.setSectionSize(size); }

    void prepareToPlay(double sampleRate, int samplesPerBlock)
    {
        engine_.prepareToPlay(sampleRate, samplesPerBlock);
    }

private:
    BrassEngine engine_;
    float currentFrequency_ = 0.0f;
    float currentVelocity_ = 0.0f;
    bool isNoteOn_ = false;
    int samplesSinceNoteOn_ = 0;
};

/**
 * BrassSound - Dummy sound class (required by JUCE)
 */
class BrassSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};
