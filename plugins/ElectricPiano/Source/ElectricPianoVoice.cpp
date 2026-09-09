#include "ElectricPianoVoice.h"

ElectricPianoVoice::ElectricPianoVoice()
{
}

bool ElectricPianoVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<ElectricPianoSound*>(sound) != nullptr;
}

void ElectricPianoVoice::startNote(int midiNoteNumber, float velocity,
                                   juce::SynthesiserSound*, int)
{
    currentFrequency_ = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    currentVelocity_ = velocity;
    isNoteOn_ = true;

    engine_.reset();
}

void ElectricPianoVoice::stopNote(float, bool allowTailOff)
{
    isNoteOn_ = false;

    if (!allowTailOff)
    {
        clearCurrentNote();
    }
}

void ElectricPianoVoice::pitchWheelMoved(int)
{
    // Pitch wheel support can be added here if desired
}

void ElectricPianoVoice::controllerMoved(int, int)
{
    // CC support can be added here if desired
}

void ElectricPianoVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                                         int startSample, int numSamples)
{
    if (currentFrequency_ <= 0.0f)
        return;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float currentSample = engine_.processSample(currentFrequency_, currentVelocity_, isNoteOn_);

        // Add to all output channels
        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        {
            outputBuffer.addSample(channel, startSample + sample, currentSample);
        }

        // Stop voice if amplitude has decayed to near zero and note is off
        if (!isNoteOn_ && std::abs(currentSample) < 0.001f)
        {
            clearCurrentNote();
            break;
        }
    }
}
