#include "Reverb.h"
#include <algorithm>

Reverb::Reverb()
{
}

void Reverb::prepare(double sampleRate, int maxBlockSize)
{
    currentSampleRate = sampleRate;

    // Scale delay times to current sample rate
    float sampleRateScale = static_cast<float>(sampleRate / 44100.0);

    // Initialize comb filters
    for (int i = 0; i < NUM_COMBS; ++i)
    {
        int scaledSize = static_cast<int>(COMB_DELAYS[i] * sampleRateScale);
        combFilters[i].bufferSize = scaledSize;
        combFilters[i].buffer.resize(scaledSize, 0.0f);
        combFilters[i].writePos = 0;
        combFilters[i].filterState = 0.0f;
    }

    // Initialize allpass filters
    for (int i = 0; i < NUM_ALLPASS; ++i)
    {
        int scaledSize = static_cast<int>(ALLPASS_DELAYS[i] * sampleRateScale);
        allpassFilters[i].bufferSize = scaledSize;
        allpassFilters[i].buffer.resize(scaledSize, 0.0f);
        allpassFilters[i].writePos = 0;
        allpassFilters[i].feedback = 0.5f;
    }

    updateCoefficients();
}

void Reverb::processSample(float input, float& leftOut, float& rightOut)
{
    // Sum parallel comb filters
    float combSum = 0.0f;
    for (int i = 0; i < NUM_COMBS; ++i)
    {
        combSum += processComb(combFilters[i], input);
    }

    // Average comb outputs
    combSum /= static_cast<float>(NUM_COMBS);

    // Pass through allpass filters in series
    float allpassOut = combSum;
    for (int i = 0; i < NUM_ALLPASS; ++i)
    {
        allpassOut = processAllpass(allpassFilters[i], allpassOut);
    }

    // Create stereo spread by inverting phase on right channel
    // (simple but effective stereo widening)
    float wet = allpassOut;
    leftOut = input * (1.0f - dryWetMix) + wet * dryWetMix;
    rightOut = input * (1.0f - dryWetMix) - wet * dryWetMix * 0.7f; // Slightly different phase

    return;
}

void Reverb::setParameters(float roomSize, float damping, float mix)
{
    reverbRoomSize = std::max(0.0f, std::min(1.0f, roomSize));
    reverbDamping = std::max(0.0f, std::min(1.0f, damping));
    dryWetMix = std::max(0.0f, std::min(1.0f, mix));

    updateCoefficients();
}

void Reverb::reset()
{
    // Clear all comb filter buffers
    for (int i = 0; i < NUM_COMBS; ++i)
    {
        std::fill(combFilters[i].buffer.begin(), combFilters[i].buffer.end(), 0.0f);
        combFilters[i].writePos = 0;
        combFilters[i].filterState = 0.0f;
    }

    // Clear all allpass filter buffers
    for (int i = 0; i < NUM_ALLPASS; ++i)
    {
        std::fill(allpassFilters[i].buffer.begin(), allpassFilters[i].buffer.end(), 0.0f);
        allpassFilters[i].writePos = 0;
    }
}

float Reverb::processComb(CombFilter& comb, float input)
{
    // Read from delay line
    float delayed = comb.buffer[comb.writePos];

    // Apply damping filter (simple one-pole lowpass)
    comb.filterState = delayed * (1.0f - comb.damping) + comb.filterState * comb.damping;

    // Feedback with filtered value
    float output = input + comb.filterState * comb.feedback;

    // Write to buffer
    comb.buffer[comb.writePos] = output;

    // Advance write position
    comb.writePos = (comb.writePos + 1) % comb.bufferSize;

    return delayed;
}

float Reverb::processAllpass(AllpassFilter& allpass, float input)
{
    // Read from delay line
    float delayed = allpass.buffer[allpass.writePos];

    // Allpass formula: output = -input + delayed + (feedback * input)
    float output = -input + delayed;

    // Write to buffer: input + (feedback * delayed)
    allpass.buffer[allpass.writePos] = input + (allpass.feedback * delayed);

    // Advance write position
    allpass.writePos = (allpass.writePos + 1) % allpass.bufferSize;

    return output;
}

void Reverb::updateCoefficients()
{
    // Map room size (0-1) to feedback (0.5-0.95)
    // Larger rooms = longer decay = higher feedback
    float feedback = 0.5f + reverbRoomSize * 0.45f;

    // Update all comb filters
    for (int i = 0; i < NUM_COMBS; ++i)
    {
        combFilters[i].feedback = feedback;
        combFilters[i].damping = reverbDamping;
    }
}
