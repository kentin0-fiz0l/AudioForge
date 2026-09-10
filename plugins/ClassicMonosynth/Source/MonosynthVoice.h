#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "MonosynthEngine.h"

class MonosynthVoice : public juce::SynthesiserVoice
{
public:
    MonosynthVoice();

    bool canPlaySound(juce::SynthesiserSound*) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override;
    void stopNote(float, bool allowTailOff) override;
    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    void setOsc1Level(float level) { engine_.setOsc1Level(level); }
    void setOsc2Level(float level) { engine_.setOsc2Level(level); }
    void setOsc3Level(float level) { engine_.setOsc3Level(level); }
    void setOsc2Detune(float semitones) { engine_.setOsc2Detune(semitones); }
    void setOsc3Detune(float semitones) { engine_.setOsc3Detune(semitones); }
    void setPulseWidth(float width) { engine_.setPulseWidth(width); }
    void setOscSync(bool enabled) { engine_.setOscSync(enabled); }

    void setFilterCutoff(float cutoff) { engine_.setFilterCutoff(cutoff); }
    void setFilterResonance(float resonance) { engine_.setFilterResonance(resonance); }
    void setFilterEnvAmount(float amount) { engine_.setFilterEnvAmount(amount); }

    void setFilterAttack(float attack) { engine_.setFilterAttack(attack); }
    void setFilterDecay(float decay) { engine_.setFilterDecay(decay); }
    void setFilterSustain(float sustain) { engine_.setFilterSustain(sustain); }
    void setFilterRelease(float release) { engine_.setFilterRelease(release); }

    void setAmpAttack(float attack) { engine_.setAmpAttack(attack); }
    void setAmpDecay(float decay) { engine_.setAmpDecay(decay); }
    void setAmpSustain(float sustain) { engine_.setAmpSustain(sustain); }
    void setAmpRelease(float release) { engine_.setAmpRelease(release); }

    void setLfo1Rate(float rate) { engine_.setLfo1Rate(rate); }
    void setLfo1Amount(float amount) { engine_.setLfo1Amount(amount); }
    void setLfo2Rate(float rate) { engine_.setLfo2Rate(rate); }
    void setLfo2Amount(float amount) { engine_.setLfo2Amount(amount); }

    void setGlideTime(float time) { engine_.setGlideTime(time); }

    void prepareToPlay(double sampleRate, int samplesPerBlock)
    {
        engine_.prepareToPlay(sampleRate, samplesPerBlock);
    }

private:
    MonosynthEngine engine_;
    float currentFrequency_ = 0.0f;
    bool isNoteOn_ = false;
};

class MonosynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};
