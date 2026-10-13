#include "KotoVoice.h"

KotoVoice::KotoVoice() {}

bool KotoVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<KotoSound*>(sound) != nullptr;
}

void KotoVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int currentPitchWheelPosition)
{
    currentFrequency_ = static_cast<float>(juce::MidiMessage::getMidiNoteInHertz(engine_.snapToScale(midiNoteNumber)));
    currentVelocity_ = velocity;
    isNoteOn_ = true;
    samplesSinceNoteOn_ = 0;

    engine_.reset();
    engine_.setPitchWheel(currentPitchWheelPosition);
}

void KotoVoice::stopNote(float, bool allowTailOff)
{
    isNoteOn_ = false;

    if (!allowTailOff)
        clearCurrentNote();
}

void KotoVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
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

        if (engine_.isReleaseFinished())
        {
            clearCurrentNote();
            break;
        }
    }
}
