#include "GrainScheduler.h"

GrainScheduler::GrainScheduler()
{
    grainPool.resize(MAX_GRAINS);
    random.setSeed(juce::Time::currentTimeMillis());
}

void GrainScheduler::prepare(double newSampleRate, int maxGrainSize)
{
    sampleRate = newSampleRate;

    // Allocate grain buffers
    for (auto& grain : grainPool)
    {
        grain.samples.resize(maxGrainSize);
        grain.reset();
    }

    // Calculate samples per grain based on density
    samplesPerGrain = (float)sampleRate / grainDensity;
    grainTimer = 0.0f;

    reset();
}

void GrainScheduler::reset()
{
    for (auto& grain : grainPool)
    {
        grain.reset();
    }

    grainTimer = 0.0f;
}

void GrainScheduler::setGrainDensity(float grainsPerSecond)
{
    grainDensity = juce::jlimit(1.0f, 100.0f, grainsPerSecond);
    samplesPerGrain = (float)sampleRate / grainDensity;
}

void GrainScheduler::setTimeStretch(float factor)
{
    timeStretch = juce::jlimit(0.25f, 4.0f, factor);
}

void GrainScheduler::setReadPosition(float position)
{
    readPosition = juce::jlimit(0.0f, 1.0f, position);
}

void GrainScheduler::setPitchShift(float semitones)
{
    pitchShift = juce::jlimit(-24.0f, 24.0f, semitones);
}

void GrainScheduler::setSprayAmount(float amount)
{
    sprayAmount = juce::jlimit(0.0f, 1.0f, amount);
}

void GrainScheduler::setReverseProbability(float probability)
{
    reverseProbability = juce::jlimit(0.0f, 1.0f, probability);
}

void GrainScheduler::setStereoWidth(float width)
{
    stereoWidth = juce::jlimit(0.0f, 2.0f, width);
}

void GrainScheduler::setGrainDirection(int direction)
{
    grainDirection = juce::jlimit(0, 2, direction);
}

void GrainScheduler::processBlock(const GrainBuffer& buffer,
                                   GrainExtractor& extractor,
                                   float* leftOutput,
                                   float* rightOutput,
                                   int numSamples)
{
    // Clear outputs
    std::fill(leftOutput, leftOutput + numSamples, 0.0f);
    std::fill(rightOutput, rightOutput + numSamples, 0.0f);

    // Check if buffer has enough samples
    if (buffer.getSamplesAvailable() < extractor.getGrainSize())
        return;

    // Process each sample
    for (int i = 0; i < numSamples; ++i)
    {
        // Check if we should trigger a new grain
        grainTimer += 1.0f;
        if (grainTimer >= samplesPerGrain)
        {
            triggerGrain(buffer, extractor);
            grainTimer -= samplesPerGrain;
        }

        // Sum all active grains to output (with pitch shifting and stereo)
        for (auto& grain : grainPool)
        {
            if (grain.active && !grain.isFinished())
            {
                // Get sample with linear interpolation for pitch shifting
                int pos = (int)grain.playbackPosition;
                float frac = grain.playbackPosition - pos;

                float sample1 = (pos >= 0 && pos < (int)grain.samples.size())
                    ? grain.samples[pos] : 0.0f;
                float sample2 = (pos + 1 >= 0 && pos + 1 < (int)grain.samples.size())
                    ? grain.samples[pos + 1] : 0.0f;

                float sample = sample1 + (sample2 - sample1) * frac;

                // Apply stereo panning (equal power panning)
                float panAngle = grain.panPosition * juce::MathConstants<float>::halfPi;
                float leftGain = std::cos(panAngle);
                float rightGain = std::sin(panAngle);

                leftOutput[i] += sample * leftGain;
                rightOutput[i] += sample * rightGain;

                // Advance playback position based on rate and direction
                if (grain.pingpong)
                {
                    // Pingpong mode: bounce direction at endpoints
                    if (grain.reverse)
                        grain.playbackPosition -= grain.playbackRate;
                    else
                        grain.playbackPosition += grain.playbackRate;

                    // Check for direction change at endpoints
                    if (grain.playbackPosition >= (float)grain.samples.size())
                    {
                        grain.reverse = true;
                        grain.playbackPosition = (float)grain.samples.size() - 1.0f;
                    }
                    else if (grain.playbackPosition < 0.0f)
                    {
                        grain.reverse = false;
                        grain.playbackPosition = 0.0f;
                    }
                }
                else
                {
                    // Normal mode: forward or backward
                    if (grain.reverse)
                        grain.playbackPosition -= grain.playbackRate;
                    else
                        grain.playbackPosition += grain.playbackRate;

                    // Check if grain finished
                    if (grain.isFinished())
                    {
                        grain.reset();
                    }
                }
            }
        }
    }

    // Normalize output (divide by max possible overlapping grains)
    float normalization = 1.0f / std::sqrt((float)MAX_GRAINS);
    for (int i = 0; i < numSamples; ++i)
    {
        leftOutput[i] *= normalization;
        rightOutput[i] *= normalization;
    }
}

void GrainScheduler::triggerGrain(const GrainBuffer& buffer, GrainExtractor& extractor)
{
    // Find an inactive grain
    Grain* grain = findInactiveGrain();
    if (grain == nullptr)
        return;  // No available grains

    // Calculate read position in buffer with spray/scatter
    int samplesAvailable = buffer.getSamplesAvailable();
    int writePos = buffer.getWritePosition();

    // Base position: 0 = oldest sample, 1 = newest sample
    float basePosition = readPosition;

    // Add spray (random offset)
    if (sprayAmount > 0.0f)
    {
        float sprayRange = sprayAmount * 0.5f;  // ±50% of spray amount
        float sprayOffset = (random.nextFloat() * 2.0f - 1.0f) * sprayRange;
        basePosition = juce::jlimit(0.0f, 1.0f, basePosition + sprayOffset);
    }

    int samplesBack = (int)((1.0f - basePosition) * samplesAvailable);
    int readPos = writePos - samplesBack;

    // Extract grain from buffer
    extractor.extractGrain(buffer, readPos, grain->samples.data());

    // Set pitch shift (convert semitones to playback rate)
    // 12 semitones = 1 octave = 2x playback rate
    grain->playbackRate = std::pow(2.0f, pitchShift / 12.0f);

    // Determine grain playback direction
    if (grainDirection == 0)  // Forward
    {
        grain->reverse = false;
        grain->pingpong = false;
        grain->playbackPosition = 0.0f;
    }
    else if (grainDirection == 1)  // Backward
    {
        grain->reverse = true;
        grain->pingpong = false;
        grain->playbackPosition = (float)grain->samples.size() - 1.0f;
    }
    else if (grainDirection == 2)  // Pingpong
    {
        grain->reverse = false;  // Start forward
        grain->pingpong = true;
        grain->playbackPosition = 0.0f;
    }
    else  // Fallback to random reversal based on probability
    {
        grain->reverse = (random.nextFloat() < reverseProbability);
        grain->pingpong = false;

        if (grain->reverse)
            grain->playbackPosition = (float)grain->samples.size() - 1.0f;
        else
            grain->playbackPosition = 0.0f;
    }

    // Set stereo pan position (0.5 = center, apply stereo width)
    float centerPan = 0.5f;
    if (stereoWidth != 1.0f)
    {
        // Random pan position
        float randomPan = random.nextFloat();
        // Apply stereo width: 0 = mono center, 1 = normal stereo, 2 = ultra-wide
        float panOffset = (randomPan - 0.5f) * stereoWidth;
        grain->panPosition = juce::jlimit(0.0f, 1.0f, centerPan + panOffset);
    }
    else
    {
        grain->panPosition = centerPan;
    }

    // Activate grain
    grain->active = true;
}

Grain* GrainScheduler::findInactiveGrain()
{
    for (auto& grain : grainPool)
    {
        if (!grain.active)
            return &grain;
    }

    return nullptr;  // All grains are active
}
