#pragma once

#include "GrainBuffer.h"
#include "GrainExtractor.h"
#include <vector>
#include <juce_core/juce_core.h>

/**
 * Grain
 *
 * Represents a single grain being played back.
 */
struct Grain
{
    std::vector<float> samples;  // Windowed grain samples
    int playbackPosition = 0;    // Current playback position
    bool active = false;         // Is this grain playing?

    void reset()
    {
        playbackPosition = 0;
        active = false;
    }

    bool isFinished() const
    {
        return playbackPosition >= (int)samples.size();
    }
};

/**
 * GrainScheduler
 *
 * Schedules and plays back multiple overlapping grains.
 * Implements grain density control and time-stretching.
 */
class GrainScheduler
{
public:
    GrainScheduler();
    ~GrainScheduler() = default;

    //==============================================================================
    // Configuration
    void prepare(double sampleRate, int maxGrainSize);
    void reset();

    //==============================================================================
    // Parameters
    void setGrainDensity(float grainsPerSecond);  // How many grains to trigger per second
    void setTimeStretch(float factor);            // Playback speed (0.25-4x)
    void setReadPosition(float position);         // Where to read from buffer (0-1)

    float getGrainDensity() const { return grainDensity; }
    float getTimeStretch() const { return timeStretch; }
    float getReadPosition() const { return readPosition; }

    //==============================================================================
    // Grain Processing
    void processBlock(const GrainBuffer& buffer,
                      GrainExtractor& extractor,
                      float* output,
                      int numSamples);

private:
    //==============================================================================
    // Grain Triggering
    void triggerGrain(const GrainBuffer& buffer, GrainExtractor& extractor);
    Grain* findInactiveGrain();

    //==============================================================================
    // Parameters
    double sampleRate = 44100.0;
    float grainDensity = 10.0f;    // Grains per second
    float timeStretch = 1.0f;      // Playback speed multiplier
    float readPosition = 0.5f;     // Read position in buffer (0-1)

    //==============================================================================
    // Grain Pool
    std::vector<Grain> grainPool;
    static constexpr int MAX_GRAINS = 64;  // Maximum simultaneous grains

    //==============================================================================
    // Scheduling
    float grainTimer = 0.0f;       // Timer for next grain trigger
    float samplesPerGrain = 0.0f;  // Samples between grain triggers

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrainScheduler)
};
