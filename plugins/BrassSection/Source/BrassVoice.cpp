#include "BrassVoice.h"

BrassVoice::BrassVoice()
{
}

bool BrassVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<BrassSound*>(sound) != nullptr;
}

void BrassVoice::startNote(int midiNoteNumber, float velocity,
                           juce::SynthesiserSound*, int)
{
    currentFrequency_ = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    currentVelocity_ = velocity;
    isNoteOn_ = true;
    samplesSinceNoteOn_ = 0;

    engine_.reset();
}

void BrassVoice::stopNote(float, bool allowTailOff)
{
    isNoteOn_ = false;

    if (!allowTailOff)
    {
        clearCurrentNote();
    }
}

void BrassVoice::pitchWheelMoved(int)
{
    // Pitch wheel support can be added here if desired
}

void BrassVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
    // Mod wheel (CC1) controls expression
    if (controllerNumber == 1)
    {
        float expression = static_cast<float>(newControllerValue) / 127.0f;
        engine_.setExpression(expression);
    }
}

void BrassVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                                 int startSample, int numSamples)
{
    if (currentFrequency_ <= 0.0f)
        return;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float currentSample = engine_.processSample(currentFrequency_, currentVelocity_,
                                                    isNoteOn_, samplesSinceNoteOn_);

        // Add to all output channels
        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        {
            outputBuffer.addSample(channel, startSample + sample, currentSample);
        }

        samplesSinceNoteOn_++;

        // Stop voice if amplitude has decayed to near zero and note is off
        if (!isNoteOn_ && std::abs(currentSample) < 0.001f)
        {
            clearCurrentNote();
            break;
        }
    }
}
