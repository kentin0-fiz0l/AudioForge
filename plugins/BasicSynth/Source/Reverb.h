#pragma once

#include <vector>
#include <array>

/**
 * Algorithmic Reverb
 *
 * Simplified Schroeder reverb with comb filters and allpass filters.
 * Creates ambient space simulation with adjustable size and damping.
 *
 * Classic plate/room reverb sound for synth pads and leads.
 */
class Reverb
{
public:
    Reverb();

    /**
     * Prepare reverb for playback.
     *
     * @param sampleRate Sample rate in Hz
     * @param maxBlockSize Maximum expected block size
     */
    void prepare(double sampleRate, int maxBlockSize);

    /**
     * Process one mono sample, output stereo.
     *
     * @param input Mono input sample
     * @param leftOut Left channel output
     * @param rightOut Right channel output
     */
    void processSample(float input, float& leftOut, float& rightOut);

    /**
     * Set reverb parameters.
     *
     * @param roomSize Room size (0.0-1.0, maps to decay time)
     * @param damping High frequency damping (0.0-1.0)
     * @param mix Dry/wet mix (0.0=dry, 1.0=wet)
     */
    void setParameters(float roomSize, float damping, float mix);

    /**
     * Reset reverb state (clear all delay buffers).
     */
    void reset();

private:
    // Comb filter delays (in samples at 44.1kHz, will be scaled)
    static constexpr int NUM_COMBS = 4;
    static constexpr int COMB_DELAYS[NUM_COMBS] = {1557, 1617, 1491, 1422};

    // Allpass filter delays (in samples at 44.1kHz, will be scaled)
    static constexpr int NUM_ALLPASS = 2;
    static constexpr int ALLPASS_DELAYS[NUM_ALLPASS] = {225, 341};

    // Comb filter structure
    struct CombFilter
    {
        std::vector<float> buffer;
        int bufferSize = 0;
        int writePos = 0;
        float feedback = 0.5f;
        float damping = 0.5f;
        float filterState = 0.0f;
    };

    // Allpass filter structure
    struct AllpassFilter
    {
        std::vector<float> buffer;
        int bufferSize = 0;
        int writePos = 0;
        float feedback = 0.5f;
    };

    // Filter arrays
    std::array<CombFilter, NUM_COMBS> combFilters;
    std::array<AllpassFilter, NUM_ALLPASS> allpassFilters;

    // Parameters
    float reverbRoomSize = 0.5f;
    float reverbDamping = 0.5f;
    float dryWetMix = 0.3f;

    // Sample rate
    double currentSampleRate = 44100.0;

    /**
     * Process one sample through a comb filter.
     *
     * @param comb Comb filter to process
     * @param input Input sample
     * @return Filtered output
     */
    float processComb(CombFilter& comb, float input);

    /**
     * Process one sample through an allpass filter.
     *
     * @param allpass Allpass filter to process
     * @param input Input sample
     * @return Filtered output
     */
    float processAllpass(AllpassFilter& allpass, float input);

    /**
     * Update filter feedback coefficients based on parameters.
     */
    void updateCoefficients();
};
