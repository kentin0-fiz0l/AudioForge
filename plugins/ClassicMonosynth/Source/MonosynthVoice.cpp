#include "MonosynthVoice.h"

MonosynthVoice::MonosynthVoice() {}

bool MonosynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<MonosynthSound*>(sound) != nullptr;
}

void MonosynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{
    currentFrequency_ = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    isNoteOn_ = true;

    engine_.noteOn();
}

void MonosynthVoice::stopNote(float, bool allowTailOff)
{
    isNoteOn_ = false;
    engine_.noteOff();

    if (!allowTailOff)
        clearCurrentNote();
}

void MonosynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (currentFrequency_ <= 0.0f)
        return;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float currentSample = engine_.processSample(currentFrequency_);

        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        {
            outputBuffer.addSample(channel, startSample + sample, currentSample * 0.3f);
        }
    }
}
