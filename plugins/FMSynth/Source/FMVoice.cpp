#include "FMVoice.h"

FMVoice::FMVoice() {}

bool FMVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<FMSound*>(sound) != nullptr;
}

void FMVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{
    float frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    engine_.noteOn(frequency, velocity);
}

void FMVoice::stopNote(float, bool allowTailOff)
{
    engine_.noteOff();
    if (!allowTailOff)
        clearCurrentNote();
}

void FMVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
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
