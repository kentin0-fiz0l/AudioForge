#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "TomEngine.h"
class TomVoice : public juce::SynthesiserVoice {
public:
    TomVoice();
    bool canPlaySound(juce::SynthesiserSound*) override;
    void startNote(int, float, juce::SynthesiserSound*, int) override;
    void stopNote(float, bool) override;
    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}
    void renderNextBlock(juce::AudioBuffer<float>&, int, int) override;
    void setPitch(float p) { engine_.setPitch(p); }
    void setPitchEnv(float e) { engine_.setPitchEnv(e); }
    void setDecay(float d) { engine_.setDecay(d); }
    void setBend(float b) { engine_.setBend(b); }
    void setNoise(float n) { engine_.setNoise(n); }
    void prepareToPlay(double sr, int sb) { engine_.prepareToPlay(sr, sb); }
private:
    TomEngine engine_;
};
class TomSound : public juce::SynthesiserSound {
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};
