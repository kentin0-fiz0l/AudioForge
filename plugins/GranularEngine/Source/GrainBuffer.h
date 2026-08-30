#pragma once

#include <vector>
#include <juce_core/juce_core.h>

/**
 * GrainBuffer
 *
 * Circular buffer for storing incoming audio samples.
 * Allows random access for grain extraction at any position.
 */
class GrainBuffer
{
public:
    GrainBuffer();
    ~GrainBuffer() = default;

    //==============================================================================
    // Buffer Management
    void prepare(double sampleRate, int maxBufferSize);
    void reset();

    //==============================================================================
    // Write Operations
    void writeSample(float sample);
    void writeBlock(const float* samples, int numSamples);

    //==============================================================================
    // Read Operations
    float readSample(int position) const;
    void readBlock(float* destination, int startPosition, int numSamples) const;

    //==============================================================================
    // Position Management
    int getWritePosition() const { return writePos; }
    int getBufferSize() const { return bufferSize; }
    int getSamplesAvailable() const { return samplesStored; }

    // Convert relative position (-bufferSize to 0) to absolute buffer index
    int wrapPosition(int position) const;

private:
    //==============================================================================
    // Circular Buffer
    std::vector<float> buffer;
    int bufferSize = 0;
    int writePos = 0;
    int samplesStored = 0;  // How many samples have been written (up to bufferSize)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrainBuffer)
};
