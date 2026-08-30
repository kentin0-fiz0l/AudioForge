#include "GrainScheduler.h"

GrainScheduler::GrainScheduler()
{
    grainPool.resize(MAX_GRAINS);
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

void GrainScheduler::processBlock(const GrainBuffer& buffer,
                                   GrainExtractor& extractor,
                                   float* output,
                                   int numSamples)
{
    // Clear output
    std::fill(output, output + numSamples, 0.0f);

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

        // Sum all active grains to output
        for (auto& grain : grainPool)
        {
            if (grain.active && !grain.isFinished())
            {
                output[i] += grain.samples[grain.playbackPosition];
                grain.playbackPosition++;

                // Check if grain finished
                if (grain.isFinished())
                {
                    grain.reset();
                }
            }
        }
    }

    // Normalize output (divide by max possible overlapping grains)
    float normalization = 1.0f / std::sqrt((float)MAX_GRAINS);
    for (int i = 0; i < numSamples; ++i)
    {
        output[i] *= normalization;
    }
}

void GrainScheduler::triggerGrain(const GrainBuffer& buffer, GrainExtractor& extractor)
{
    // Find an inactive grain
    Grain* grain = findInactiveGrain();
    if (grain == nullptr)
        return;  // No available grains

    // Calculate read position in buffer
    int bufferSize = buffer.getBufferSize();
    int samplesAvailable = buffer.getSamplesAvailable();
    int writePos = buffer.getWritePosition();

    // Read position: 0 = oldest sample, 1 = newest sample
    int samplesBack = (int)((1.0f - readPosition) * samplesAvailable);
    int readPos = writePos - samplesBack;

    // Extract grain from buffer
    int grainSize = extractor.getGrainSize();
    extractor.extractGrain(buffer, readPos, grain->samples.data());

    // Activate grain
    grain->active = true;
    grain->playbackPosition = 0;

    // Note: Time-stretching will be implemented by adjusting grain overlap
    // For now, grains play at normal speed (timeStretch parameter ready for Phase 3)
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
