#include "Voice.h"
#include <dsp/WaveformGenerators.h>
#include <cmath>

Voice::Voice()
{
    // Set default envelope
    envelope.setAttack(0.8f);
    envelope.setDecay(0.5f);
    envelope.setSustain(0.9f);
    envelope.setRelease(1.5f);
}

void Voice::noteOn(int midiNote, float velocity_, double sampleRate)
{
    currentNote = midiNote;
    velocity = velocity_;
    active = true;

    // Convert MIDI note to frequency
    baseFrequency = AudioForge::DSP::WaveformGenerators::midiNoteToFrequency(midiNote);

    // Trigger envelope
    envelope.trigger();

    // Reset filters
    filterL.reset();
    filterR.reset();

    // Reset all unison voices
    for (auto& voice : unisonVoices)
        voice.reset();
}

void Voice::noteOff()
{
    envelope.release();
}

bool Voice::isActive() const
{
    return envelope.isActive();
}

float Voice::getLevel() const
{
    return envelope.getLevel();
}

void Voice::reset()
{
    active = false;
    currentNote = -1;
    velocity = 0.0f;
    envelope.reset();
    filterL.reset();
    filterR.reset();

    for (auto& voice : unisonVoices)
        voice.reset();
}

void Voice::updateUnisonVoices(int waveform, int numVoices, float detuneAmount, float stereoWidth)
{
    // Clamp number of voices
    numVoices = std::max(3, std::min(MAX_UNISON_VOICES, numVoices));

    // Update each unison voice with frequency, detune, and pan
    for (int i = 0; i < numVoices; ++i)
    {
        // Calculate detune offset: spread voices evenly from -detuneAmount to +detuneAmount
        float detuneOffset = 0.0f;
        if (numVoices > 1)
        {
            float normalizedPosition = static_cast<float>(i) / (numVoices - 1); // 0.0 to 1.0
            detuneOffset = (normalizedPosition * 2.0f - 1.0f) * detuneAmount; // -detuneAmount to +detuneAmount
        }

        // Calculate stereo pan: spread voices across stereo field
        float pan = 0.0f;
        if (numVoices > 1)
        {
            float normalizedPosition = static_cast<float>(i) / (numVoices - 1);
            pan = (normalizedPosition * 2.0f - 1.0f) * stereoWidth; // -stereoWidth to +stereoWidth
        }

        unisonVoices[i].setFrequency(baseFrequency, detuneOffset, pan);
        unisonVoices[i].setWaveform(waveform);
    }
}

void Voice::processSample(double sampleRate,
                         int waveform,
                         int unisonVoiceCount,
                         float detuneAmount,
                         float stereoWidth,
                         float filterCutoff,
                         float filterResonance,
                         int filterType,
                         float& outLeft,
                         float& outRight)
{
    outLeft = 0.0f;
    outRight = 0.0f;

    if (!isActive())
        return;

    // Update unison voices
    updateUnisonVoices(waveform, unisonVoiceCount, detuneAmount, stereoWidth);

    // Get envelope value
    float deltaTime = static_cast<float>(1.0 / sampleRate);
    float envValue = envelope.getNextValue(deltaTime);

    // Sum unison voices with stereo panning
    float monoSum = 0.0f;
    float leftSum = 0.0f;
    float rightSum = 0.0f;

    for (int i = 0; i < unisonVoiceCount; ++i)
    {
        float sample = unisonVoices[i].processSample(sampleRate);
        float pan = unisonVoices[i].getPan();

        // Constant power panning
        float panRadians = (pan + 1.0f) * 0.25f * M_PI; // Map -1..1 to 0..PI/2
        float leftGain = std::cos(panRadians);
        float rightGain = std::sin(panRadians);

        leftSum += sample * leftGain;
        rightSum += sample * rightGain;
    }

    // Normalize by number of voices
    float normalization = 1.0f / std::sqrt(static_cast<float>(unisonVoiceCount));
    leftSum *= normalization;
    rightSum *= normalization;

    // Apply stereo filtering
    filterL.setParameters(filterCutoff, filterResonance, sampleRate);
    filterL.setFilterType(static_cast<MultiModeFilter::FilterType>(filterType));
    filterR.setParameters(filterCutoff, filterResonance, sampleRate);
    filterR.setFilterType(static_cast<MultiModeFilter::FilterType>(filterType));

    leftSum = filterL.processSample(leftSum);
    rightSum = filterR.processSample(rightSum);

    // Apply envelope and velocity
    outLeft = leftSum * envValue * velocity;
    outRight = rightSum * envValue * velocity;
}

void Voice::updateEnvelope(float attack, float decay, float sustain, float release)
{
    envelope.setAttack(attack);
    envelope.setDecay(decay);
    envelope.setSustain(sustain);
    envelope.setRelease(release);
}
