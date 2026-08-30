#include "GrainBuffer.h"

GrainBuffer::GrainBuffer()
{
}

void GrainBuffer::prepare(double sampleRate, int maxBufferSize)
{
    // Default: 10 seconds of audio storage at given sample rate
    bufferSize = juce::jmax(maxBufferSize, (int)(sampleRate * 10.0));
    buffer.resize(bufferSize, 0.0f);
    reset();
}

void GrainBuffer::reset()
{
    std::fill(buffer.begin(), buffer.end(), 0.0f);
    writePos = 0;
    samplesStored = 0;
}

void GrainBuffer::writeSample(float sample)
{
    buffer[writePos] = sample;
    writePos = (writePos + 1) % bufferSize;

    if (samplesStored < bufferSize)
        samplesStored++;
}

void GrainBuffer::writeBlock(const float* samples, int numSamples)
{
    for (int i = 0; i < numSamples; ++i)
    {
        writeSample(samples[i]);
    }
}

float GrainBuffer::readSample(int position) const
{
    int idx = wrapPosition(position);
    return buffer[idx];
}

void GrainBuffer::readBlock(float* destination, int startPosition, int numSamples) const
{
    for (int i = 0; i < numSamples; ++i)
    {
        destination[i] = readSample(startPosition + i);
    }
}

int GrainBuffer::wrapPosition(int position) const
{
    // Handle negative positions (reading backwards from write head)
    while (position < 0)
        position += bufferSize;

    // Wrap to buffer size
    return position % bufferSize;
}
