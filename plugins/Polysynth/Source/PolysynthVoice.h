#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PolysynthEngine.h"

class PolysynthVoice : public juce::SynthesiserVoice
{
public:
    PolysynthVoice();

    bool canPlaySound(juce::SynthesiserSound*) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override;
    void stopNote(float, bool allowTailOff) override;
    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    void setOscMix(float mix) { engine_.setOscMix(mix); }
    void setDetune(float semitones) { engine_.setDetune(semitones); }
    void setUnisonVoices(int voices) { engine_.setUnisonVoices(voices); }
    void setUnisonDetune(float cents) { engine_.setUnisonDetune(cents); }

    void setFilterCutoff(float cutoff) { engine_.setFilterCutoff(cutoff); }
    void setFilterResonance(float resonance) { engine_.setFilterResonance(resonance); }
    void setFilterMix(float mix) { engine_.setFilterMix(mix); }

    void setAttack(float attack) { engine_.setAttack(attack); }
    void setDecay(float decay) { engine_.setDecay(decay); }
    void setSustain(float sustain) { engine_.setSustain(sustain); }
    void setRelease(float release) { engine_.setRelease(release); }

    void setChorusDepth(float depth) { engine_.setChorusDepth(depth); }
    void setChorusRate(float rate) { engine_.setChorusRate(rate); }

    void prepareToPlay(double sampleRate, int samplesPerBlock)
    {
        engine_.prepareToPlay(sampleRate, samplesPerBlock);
    }

private:
    PolysynthEngine engine_;
};

class PolysynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};
