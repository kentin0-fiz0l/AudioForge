#include "HiHatVoice.h"
HiHatVoice::HiHatVoice() {}
bool HiHatVoice::canPlaySound(juce::SynthesiserSound* s) { return dynamic_cast<HiHatSound*>(s) != nullptr; }
void HiHatVoice::startNote(int, float vel, juce::SynthesiserSound*, int) { engine_.trigger(vel); }
void HiHatVoice::stopNote(float, bool allowTailOff) { if (!allowTailOff) clearCurrentNote(); }
void HiHatVoice::renderNextBlock(juce::AudioBuffer<float>& buf, int start, int num) {
    for (int s = 0; s < num; ++s) {
        float sample = engine_.processSample();
        for (int ch = 0; ch < buf.getNumChannels(); ++ch)
            buf.addSample(ch, start + s, sample);
        if (!engine_.isActive()) { clearCurrentNote(); break; }
    }
}
