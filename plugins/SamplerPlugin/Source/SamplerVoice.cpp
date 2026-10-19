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
    released = false;

    // Reset playback state
    playbackPosition = playback.startFrame;
    loopDirection = 1;

    // Pitch shift, and the step from the sample's own rate to this one
    if (playback.sample)
        playbackRate = playback.pitchRatio * playback.sample->getSampleRate() / sampleRate;
    else
        playbackRate = 1.0;

    // Trigger envelopes
    ampEnvelope.trigger();
    filterEnvelope.trigger();
}

void SamplerVoice::start(int midiNote, const Playback& newPlayback, double sampleRate)
{
    // A voice taken over from another note starts from silence
    if (active)
        reset();

    playback = newPlayback;
    filterLeft.reset();
    filterRight.reset();
    noteOn(midiNote, 1.0f, sampleRate);  // The velocity is in the playback's gain
}

void SamplerVoice::noteOff()
{
    if (playback.oneShot || released)
        return;

    released = true;
    ampEnvelope.release();
    filterEnvelope.release();
}

void SamplerVoice::reset()
{
    active = false;
    released = false;
    currentNote = -1;
    noteVelocity = 0.0f;
    playbackPosition = 0.0;
    playback = {};

    ampEnvelope.reset();
    filterEnvelope.reset();
    filterLeft.reset();
    filterRight.reset();
}

float SamplerVoice::getLevel() const
{
    return ampEnvelope.getLevel();
}

void SamplerVoice::processSample(double sampleRate,
                                 float attack, float decay, float sustain, float release,
                                 float filterCutoff, float filterResonance, int filterType,
                                 float filterEnvAmount,
                                 float filtAttack, float filtDecay, float filtSustain, float filtRelease,
                                 float& left, float& right)
{
    if (!active || !playback.sample)
    {
        return;
    }

    // Update envelope parameters, the voice's own over the plugin's
    ampEnvelope.setAttack(playback.attack >= 0.0f ? playback.attack : attack);
    ampEnvelope.setDecay(playback.decay >= 0.0f ? playback.decay : decay);
    ampEnvelope.setSustain(playback.sustain >= 0.0f ? playback.sustain : sustain);
    ampEnvelope.setRelease(playback.release >= 0.0f ? playback.release : release);

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
        return;
    }

    // Get audio data
    const auto& audioData = playback.sample->getAudioData();
    const int lastFrame = playback.sample->getLengthInSamples() - 1;
    const int endFrame = playback.endFrame >= 0 ? juce::jmin(playback.endFrame, lastFrame) : lastFrame;

    // Check if we've reached the end (for one-shot samples)
    if (playback.loopMode == Sample::LoopMode::None && playbackPosition >= endFrame)
    {
        active = false;
        return;
    }

    // Read sample with interpolation
    float sampleLeft = getInterpolatedSample(audioData, 0, playbackPosition);
    float sampleRight = audioData.getNumChannels() > 1 ?
        getInterpolatedSample(audioData, 1, playbackPosition) : sampleLeft;

    // Apply velocity, level and amplitude envelope
    const float gain = noteVelocity * ampEnv;
    sampleLeft *= gain * playback.gainLeft;
    sampleRight *= gain * playback.gainRight;

    // Apply filter, unless it's a low-pass wide open
    const bool filterOpen = filterType == 0 && filterCutoff >= 19999.0f && filterEnvAmount == 0.0f;
    if (!filterOpen)
    {
        float modCutoff = filterCutoff + (filtEnv * filterEnvAmount * 10000.0f);
        modCutoff = juce::jlimit(20.0f, 20000.0f, modCutoff);

        for (auto* filter : { &filterLeft, &filterRight })
        {
            filter->setParameters(modCutoff, filterResonance, sampleRate);
            filter->setFilterType(static_cast<MultiModeFilter::FilterType>(filterType));
        }

        sampleLeft = filterLeft.processSample(sampleLeft);
        sampleRight = filterRight.processSample(sampleRight);
    }

    left += sampleLeft;
    right += sampleRight;

    // Advance playback position
    playbackPosition += playbackRate * loopDirection;

    // Handle looping
    const int loopStart = playback.loopStart;
    const int loopEnd = juce::jmin(playback.loopEnd, lastFrame);

    if (loopEnd > loopStart)
    {
        if (playback.loopMode == Sample::LoopMode::Forward)
        {
            // Forward loop: from past the loop's last frame back to its first
            if (playbackPosition >= loopEnd + 1)
            {
                playbackPosition -= (loopEnd + 1 - loopStart);
            }
        }
        else if (playback.loopMode == Sample::LoopMode::PingPong)
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
        }
    }
}

float SamplerVoice::getInterpolatedSample(const juce::AudioBuffer<float>& buffer,
                                         int channel, double position) const
{
    int pos = static_cast<int>(position);
    float frac = static_cast<float>(position - pos);
    const int last = buffer.getNumSamples() - 1;

    if (pos < 0 || pos > last)
    {
        return 0.0f;
    }

    // Linear interpolation
    float sample1 = buffer.getSample(channel, pos);
    float sample2 = buffer.getSample(channel, juce::jmin(pos + 1, last));

    return sample1 + frac * (sample2 - sample1);
}
