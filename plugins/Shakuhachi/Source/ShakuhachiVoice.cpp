#include "ShakuhachiVoice.h"

ShakuhachiVoice::ShakuhachiVoice() {}

bool ShakuhachiVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<ShakuhachiSound*>(sound) != nullptr;
}

void ShakuhachiVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{
    currentFrequency_ = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    currentVelocity_ = velocity;
    isNoteOn_ = true;
    samplesSinceNoteOn_ = 0;

    engine_.reset();
}

void ShakuhachiVoice::stopNote(float, bool allowTailOff)
{
    isNoteOn_ = false;

    if (!allowTailOff)
        clearCurrentNote();
}

void ShakuhachiVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (currentFrequency_ <= 0.0f)
        return;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float currentSample = engine_.processSample(currentFrequency_, currentVelocity_,
                                                    isNoteOn_, samplesSinceNoteOn_);

        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        {
            outputBuffer.addSample(channel, startSample + sample, currentSample);
        }

        samplesSinceNoteOn_++;

        if (!isNoteOn_ && std::abs(currentSample) < 0.0001f)
        {
            clearCurrentNote();
            break;
        }
    }
}
