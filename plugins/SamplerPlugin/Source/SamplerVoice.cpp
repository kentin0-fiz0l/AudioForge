#include "SamplerVoice.h"
#include <cmath>

SamplerVoice::SamplerVoice()
{
}

void SamplerVoice::noteOn(int midiNote, float velocity, double sampleRate)
{
    currentNote = midiNote;
    noteVelocity = velocity;
    active = true;

    // Reset playback state
    playbackPosition = 0.0;
    loopDirection = 1;

    // Calculate pitch shift if we have a sample
    if (currentSample && currentZone)
    {
        playbackRate = calculatePlaybackRate(
            midiNote,
            currentSample->getRootNote(),
            currentSample->getSampleRate(),
            sampleRate
        );
    }
    else
    {
        playbackRate = 1.0;
    }

    // Trigger envelopes
    ampEnvelope.trigger();
    filterEnvelope.trigger();
}

void SamplerVoice::noteOff()
{
    ampEnvelope.release();
    filterEnvelope.release();
}

void SamplerVoice::reset()
{
    active = false;
    currentNote = -1;
    noteVelocity = 0.0f;
    playbackPosition = 0.0;
    currentSample.reset();
    currentZone = nullptr;

    ampEnvelope.reset();
    filterEnvelope.reset();
    filter.reset();
}

float SamplerVoice::getLevel() const
{
    return ampEnvelope.getLevel();
}

void SamplerVoice::setSample(std::shared_ptr<Sample> sample, const KeyZone* zone)
{
    currentSample = sample;
    currentZone = zone;
}

float SamplerVoice::processSample(double sampleRate,
                                 float attack, float decay, float sustain, float release,
                                 float filterCutoff, float filterResonance, int filterType,
                                 float filterEnvAmount,
                                 float filtAttack, float filtDecay, float filtSustain, float filtRelease)
{
    if (!active || !currentSample)
    {
        return 0.0f;
    }

    // Update envelope parameters
    ampEnvelope.setAttack(attack);
    ampEnvelope.setDecay(decay);
    ampEnvelope.setSustain(sustain);
    ampEnvelope.setRelease(release);

    filterEnvelope.setAttack(filtAttack);
    filterEnvelope.setDecay(filtDecay);
    filterEnvelope.setSustain(filtSustain);
    filterEnvelope.setRelease(filtRelease);

    // Process envelopes
    float deltaTime = 1.0f / static_cast<float>(sampleRate);
    float ampEnv = ampEnvelope.getNextValue(deltaTime);
    float filtEnv = filterEnvelope.getNextValue(deltaTime);

    // Check if envelope has finished
    if (!ampEnvelope.isActive())
    {
        active = false;
        return 0.0f;
    }

    // Get audio data
    const auto& audioData = currentSample->getAudioData();
    int numSamples = currentSample->getLengthInSamples();

    // Check if we've reached the end (for one-shot samples)
    if (playbackPosition >= numSamples - 1)
    {
        if (currentSample->getLoopMode() == Sample::LoopMode::None)
        {
            active = false;
            return 0.0f;
        }
    }

    // Read sample with interpolation (mix both channels to mono)
    float sample = 0.0f;
    if (audioData.getNumChannels() > 0)
    {
        float leftSample = getInterpolatedSample(audioData, 0, playbackPosition);
        float rightSample = audioData.getNumChannels() > 1 ?
            getInterpolatedSample(audioData, 1, playbackPosition) : leftSample;

        sample = (leftSample + rightSample) * 0.5f;
    }

    // Apply velocity
    sample *= noteVelocity;

    // Apply amplitude envelope
    sample *= ampEnv;

    // Apply filter
    float modCutoff = filterCutoff + (filtEnv * filterEnvAmount * 10000.0f);
    modCutoff = juce::jlimit(20.0f, 20000.0f, modCutoff);

    filter.setParameters(modCutoff, filterResonance, sampleRate);
    filter.setFilterType(static_cast<MultiModeFilter::FilterType>(filterType));

    sample = filter.processSample(sample);

    // Advance playback position
    playbackPosition += playbackRate;

    // Handle looping
    if (currentSample->getLoopMode() != Sample::LoopMode::None)
    {
        int loopStart = currentSample->getLoopStartSamples();
        int loopEnd = currentSample->getLoopEndSamples();

        if (currentSample->getLoopMode() == Sample::LoopMode::Forward)
        {
            // Forward loop
            if (playbackPosition >= loopEnd)
            {
                playbackPosition = loopStart + (playbackPosition - loopEnd);
            }
        }
        else if (currentSample->getLoopMode() == Sample::LoopMode::PingPong)
        {
            // Ping-pong loop
            if (loopDirection > 0 && playbackPosition >= loopEnd)
            {
                loopDirection = -1;
                playbackPosition = loopEnd - (playbackPosition - loopEnd);
            }
            else if (loopDirection < 0 && playbackPosition <= loopStart)
            {
                loopDirection = 1;
                playbackPosition = loopStart + (loopStart - playbackPosition);
            }

            // Update playback rate with direction
            playbackPosition += playbackRate * loopDirection;
        }
    }

    return sample;
}

float SamplerVoice::getInterpolatedSample(const juce::AudioBuffer<float>& buffer,
                                         int channel, double position) const
{
    int pos = static_cast<int>(position);
    float frac = static_cast<float>(position - pos);

    if (pos < 0 || pos >= buffer.getNumSamples() - 1)
    {
        return 0.0f;
    }

    // Linear interpolation
    float sample1 = buffer.getSample(channel, pos);
    float sample2 = buffer.getSample(channel, pos + 1);

    return sample1 + frac * (sample2 - sample1);
}

double SamplerVoice::calculatePlaybackRate(int midiNote, int rootNote,
                                           double sampleRate, double targetSampleRate)
{
    // Calculate frequency for target MIDI note
    // f = 440 * 2^((n-69)/12) where 69 is A4 (440 Hz)
    double targetFreq = 440.0 * std::pow(2.0, (midiNote - 69) / 12.0);
    double rootFreq = 440.0 * std::pow(2.0, (rootNote - 69) / 12.0);

    // Playback rate = (targetFreq / rootFreq) * (sampleRate / targetSampleRate)
    double pitchRatio = targetFreq / rootFreq;
    double sampleRateRatio = sampleRate / targetSampleRate;

    return pitchRatio * sampleRateRatio;
}
