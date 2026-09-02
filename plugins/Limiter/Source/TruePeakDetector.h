/*
  ==============================================================================

    TruePeakDetector.h
    Created: 1 Sep 2026
    Author:  AudioForge Team

    True peak detection with 4x oversampling (ITU-R BS.1770-4 compliant).
    Prevents intersample peaks that cause clipping on D/A conversion.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace audioforge
{

/**
 * @brief True peak detector with 4x oversampling
 *
 * Implements ITU-R BS.1770-4 true peak measurement to detect intersample peaks
 * that would cause clipping during D/A conversion. Uses polyphase FIR
 * upsampling for accurate peak detection.
 *
 * @note This is essential for mastering to streaming platforms (Spotify,
 *       Apple Music, YouTube) which require true peak limiting.
 */
class TruePeakDetector
{
public:
    TruePeakDetector();
    ~TruePeakDetector() = default;

    /**
     * @brief Prepare the detector for processing
     *
     * @param sampleRate Sample rate in Hz
     * @param maxBlockSize Maximum number of samples per block
     */
    void prepare(double sampleRate, int maxBlockSize);

    /**
     * @brief Detect true peak level in a block of samples
     *
     * @param samples Audio samples to analyze
     * @param numSamples Number of samples in the block
     * @return True peak level in linear gain (1.0 = 0 dBFS)
     */
    float detectTruePeak(const float* samples, int numSamples);

    /**
     * @brief Process stereo audio and return true peak for both channels
     *
     * @param leftSamples Left channel samples
     * @param rightSamples Right channel samples
     * @param numSamples Number of samples per channel
     * @return Maximum true peak across both channels
     */
    float detectTruePeakStereo(const float* leftSamples,
                                const float* rightSamples,
                                int numSamples);

    /**
     * @brief Reset internal state
     */
    void reset();

private:
    // 4x oversampling using JUCE's built-in oversampler
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;

    // Oversampled buffer
    juce::AudioBuffer<float> oversampledBuffer;

    // Sample rate
    double currentSampleRate = 44100.0;

    // Max block size
    int maxSamplesPerBlock = 512;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TruePeakDetector)
};

} // namespace audioforge
