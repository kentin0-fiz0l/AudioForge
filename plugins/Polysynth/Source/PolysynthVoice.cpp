#include "PolysynthVoice.h"

PolysynthVoice::PolysynthVoice() {}

bool PolysynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<PolysynthSound*>(sound) != nullptr;
}

void PolysynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{
    float frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    engine_.noteOn(frequency, velocity);
}

void PolysynthVoice::stopNote(float, bool allowTailOff)
{
    engine_.noteOff();

    if (!allowTailOff)
        clearCurrentNote();
}

void PolysynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    for (int sample = 0; sample < numSamples; ++sample)
    {
        float currentSample = engine_.processSample();

        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        {
            outputBuffer.addSample(channel, startSample + sample, currentSample);
        }

        // Check if voice should be cleared (envelope finished)
        if (!engine_.isActive())
        {
            clearCurrentNote();
            break;
        }
    }
}
