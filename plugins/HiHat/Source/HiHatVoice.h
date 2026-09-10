#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "HiHatEngine.h"
class HiHatVoice : public juce::SynthesiserVoice {
public:
    HiHatVoice();
    bool canPlaySound(juce::SynthesiserSound*) override;
    void startNote(int, float velocity, juce::SynthesiserSound*, int) override;
    void stopNote(float, bool) override;
    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}
    void renderNextBlock(juce::AudioBuffer<float>&, int, int) override;
    void setTone(float t) { engine_.setTone(t); }
    void setDecay(float d) { engine_.setDecay(d); }
    void setMetallic(float m) { engine_.setMetallic(m); }
    void setOpenness(float o) { engine_.setOpenness(o); }
    void prepareToPlay(double sr, int sb) { engine_.prepareToPlay(sr, sb); }
private:
    HiHatEngine engine_;
};
class HiHatSound : public juce::SynthesiserSound {
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};
