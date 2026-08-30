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
    float playbackPosition = 0.0f;  // Current playback position (float for pitch shifting)
    float playbackRate = 1.0f;      // Playback speed (1.0 = normal, 2.0 = octave up)
    bool reverse = false;           // Play grain backwards
    bool active = false;            // Is this grain playing?
    float panPosition = 0.5f;       // Stereo pan (0=left, 1=right)

    void reset()
    {
        playbackPosition = 0.0f;
        playbackRate = 1.0f;
        reverse = false;
        active = false;
        panPosition = 0.5f;
    }

    bool isFinished() const
    {
        if (reverse)
            return playbackPosition < 0.0f;
        else
            return playbackPosition >= (float)samples.size();
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
    void setPitchShift(float semitones);          // Pitch shift in semitones (-24 to +24)
    void setSprayAmount(float amount);            // Position randomization (0-1)
    void setReverseProbability(float probability); // Chance of reverse grain (0-1)
    void setStereoWidth(float width);             // Stereo spread (0-2, 1=normal)

    float getGrainDensity() const { return grainDensity; }
    float getTimeStretch() const { return timeStretch; }
    float getReadPosition() const { return readPosition; }
    float getPitchShift() const { return pitchShift; }
    float getSprayAmount() const { return sprayAmount; }
    float getReverseProbability() const { return reverseProbability; }
    float getStereoWidth() const { return stereoWidth; }

    //==============================================================================
    // Grain Processing
    void processBlock(const GrainBuffer& buffer,
                      GrainExtractor& extractor,
                      float* leftOutput,
                      float* rightOutput,
                      int numSamples);

private:
    //==============================================================================
    // Grain Triggering
    void triggerGrain(const GrainBuffer& buffer, GrainExtractor& extractor);
    Grain* findInactiveGrain();

    //==============================================================================
    // Parameters
    double sampleRate = 44100.0;
    float grainDensity = 10.0f;       // Grains per second
    float timeStretch = 1.0f;         // Playback speed multiplier
    float readPosition = 0.5f;        // Read position in buffer (0-1)
    float pitchShift = 0.0f;          // Pitch shift in semitones
    float sprayAmount = 0.0f;         // Position randomization (0-1)
    float reverseProbability = 0.0f;  // Probability of reverse grain (0-1)
    float stereoWidth = 1.0f;         // Stereo width (0-2)

    //==============================================================================
    // Grain Pool
    std::vector<Grain> grainPool;
    static constexpr int MAX_GRAINS = 64;  // Maximum simultaneous grains

    //==============================================================================
    // Scheduling
    float grainTimer = 0.0f;       // Timer for next grain trigger
    float samplesPerGrain = 0.0f;  // Samples between grain triggers

    //==============================================================================
    // Random Number Generator
    juce::Random random;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrainScheduler)
};
