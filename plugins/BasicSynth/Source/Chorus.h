#pragma once

#include <vector>
#include <cmath>

/**
 * Stereo Chorus Effect
 *
 * Creates a thickening/widening effect by modulating delayed copies
 * of the input signal. Uses two delay lines with phase-offset LFOs
 * for stereo spread.
 *
 * Classic chorus sound: warm, shimmering, slightly detuned
 */
class Chorus
{
public:
    Chorus();

    /**
     * Prepare the chorus for playback.
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
     * Set chorus parameters.
     *
     * @param rate LFO rate in Hz (0.1-10.0)
     * @param depth Modulation depth (0.0-1.0)
     * @param mix Dry/wet mix (0.0=dry, 1.0=wet)
     */
    void setParameters(float rate, float depth, float mix);

    /**
     * Reset effect state (clear delay buffers).
     */
    void reset();

private:
    // Delay lines (circular buffers)
    std::vector<float> delayBufferLeft;
    std::vector<float> delayBufferRight;
    int delayBufferSize = 0;
    int writePosition = 0;

    // LFO state (separate for each channel for stereo)
    float lfoPhaseLeft = 0.0f;
    float lfoPhaseRight = 0.25f;  // 90 degree offset for stereo width

    // Parameters
    float lfoRate = 0.5f;         // Hz
    float modulationDepth = 0.5f; // 0.0 to 1.0
    float dryWetMix = 0.5f;       // 0.0 to 1.0

    // Sample rate
    double currentSampleRate = 44100.0;

    // Delay time range (in samples)
    static constexpr float MIN_DELAY_MS = 5.0f;   // 5ms minimum
    static constexpr float MAX_DELAY_MS = 30.0f;  // 30ms maximum
    static constexpr float CENTER_DELAY_MS = 15.0f; // Center delay time

    /**
     * Read from delay buffer with linear interpolation.
     *
     * @param buffer Delay buffer to read from
     * @param delaySamples Delay time in samples (can be fractional)
     * @return Interpolated sample value
     */
    float readDelayBuffer(const std::vector<float>& buffer, float delaySamples);

    /**
     * Generate LFO value (sine wave).
     *
     * @param phase LFO phase (0.0-1.0)
     * @return LFO value (-1.0 to 1.0)
     */
    float generateLFO(float phase) const;
};
