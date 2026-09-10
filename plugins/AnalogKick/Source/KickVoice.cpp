#include "KickVoice.h"

KickVoice::KickVoice() {}

bool KickVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<KickSound*>(sound) != nullptr;
}

void KickVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{
    engine_.trigger(velocity);
}

void KickVoice::stopNote(float, bool allowTailOff)
{
    if (!allowTailOff)
        clearCurrentNote();
}

void KickVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    for (int sample = 0; sample < numSamples; ++sample)
    {
        float currentSample = engine_.processSample();
        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
            outputBuffer.addSample(channel, startSample + sample, currentSample);

        if (!engine_.isActive())
        {
            clearCurrentNote();
            break;
        }
    }
}
