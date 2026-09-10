#include "SnareVoice.h"

SnareVoice::SnareVoice() {}

bool SnareVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SnareSound*>(sound) != nullptr;
}

void SnareVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{
    engine_.trigger(velocity);
}

void SnareVoice::stopNote(float, bool allowTailOff)
{
    if (!allowTailOff)
        clearCurrentNote();
}

void SnareVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
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
