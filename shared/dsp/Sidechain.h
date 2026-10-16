/*
  ==============================================================================

    Sidechain.h
    Created: 1 Sep 2026
    Author:  AudioForge Team

    Sidechain processing with high-pass and low-pass filtering.
    Allows external audio to trigger dynamics processing.

    Shared across Gate, Compressor, De-Esser, and Multiband Compressor.

  ==============================================================================
*/

#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>

namespace audioforge
{

/**
 * @brief Sidechain processor for dynamics plugins
 *
 * Features:
 * - External input routing (use another track to trigger processing)
 * - High-pass filtering (remove low frequencies from trigger signal)
 * - Low-pass filtering (remove high frequencies from trigger signal)
 * - Listen mode (monitor the filtered sidechain signal)
 *
 * Example use cases:
 * - Kick drum triggers bass gate (remove low frequencies to isolate kick transient)
 * - Snare triggers reverb ducking (filter 2-5kHz to detect only snare hits)
 * - De-esser targets sibilance (high-pass at 4kHz to isolate "S" sounds)
 */
class Sidechain
{
public:
    Sidechain();
    ~Sidechain() = default;

    //==============================================================================
    // Configuration

    /**
     * @brief Prepare the sidechain for processing
     *
     * @param sampleRate Sample rate in Hz
     * @param maxBlockSize Maximum samples per block
     */
    void prepare(double sampleRate, int maxBlockSize);

    /**
     * @brief Enable/disable sidechain
     *
     * @param enabled If true, use external input; if false, use main input.
     *                The filters apply in both cases.
     */
    void setEnabled(bool enabled) { sidechainEnabled = enabled; }

    /**
     * @brief Set high-pass filter frequency
     *
     * @param hz Frequency in Hz (20-2000Hz)
     */
    void setHighPassFreq(float hz);

    /**
     * @brief Set low-pass filter frequency
     *
     * @param hz Frequency in Hz (200-20000Hz)
     */
    void setLowPassFreq(float hz);

    /**
     * @brief Enable/disable listen mode
     *
     * @param enabled If true, output the filtered detector signal; if false, output the processed main signal
     */
    void setListenMode(bool enabled) { listenModeEnabled = enabled; }

    /**
     * @brief Reset internal state
     */
    void reset();

    //==============================================================================
    // Processing

    /**
     * @brief Filter the detector signal in place
     *
     * Applies the high-pass and low-pass filters to isolate target
     * frequencies. Filtering applies whether the detector is fed from the
     * external key input or the main input. Each channel (up to two) keeps
     * its own filter state.
     *
     * @param block Detector samples (one or two channels)
     */
    void process(juce::dsp::AudioBlock<float> block);

    /**
     * @brief Get filtered sidechain level (for metering/detection)
     *
     * Call this after process() to get the envelope level.
     *
     * @param samples Filtered sidechain samples
     * @param numSamples Number of samples
     * @return Peak level (linear, 0-1)
     */
    float getLevel(const float* samples, int numSamples);

    //==============================================================================
    // Getters

    bool isEnabled() const { return sidechainEnabled; }
    bool isListenModeEnabled() const { return listenModeEnabled; }
    float getHighPassFreq() const { return highPassFreq; }
    float getLowPassFreq() const { return lowPassFreq; }

private:
    //==============================================================================
    // DSP Components

    // High-pass filter (remove low frequencies)
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                     juce::dsp::IIR::Coefficients<float>> highPassFilter;

    // Low-pass filter (remove high frequencies)
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                     juce::dsp::IIR::Coefficients<float>> lowPassFilter;

    //==============================================================================
    // State

    bool sidechainEnabled = false;
    bool listenModeEnabled = false;

    double currentSampleRate = 44100.0;
    int maxSamplesPerBlock = 512;

    float highPassFreq = 80.0f;   // Hz
    float lowPassFreq = 8000.0f;  // Hz

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Sidechain)
};

} // namespace audioforge
