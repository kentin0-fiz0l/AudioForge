#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

/**
 * SpectralProcessor
 *
 * Handles FFT analysis/synthesis with overlap-add for spectral processing.
 * Implements Short-Time Fourier Transform (STFT) for real-time spectral effects.
 */
class SpectralProcessor
{
public:
    SpectralProcessor();
    ~SpectralProcessor() = default;

    //==============================================================================
    // Setup
    void prepare(double sampleRate, int samplesPerBlock, int numChannels);
    void reset();

    //==============================================================================
    // Processing
    void processBlock(juce::AudioBuffer<float>& buffer);

    //==============================================================================
    // FFT Configuration
    void setFFTSize(int size);           // 1024, 2048, 4096, 8192
    void setOverlapFactor(int factor);   // 2, 4, 8 (higher = smoother, more CPU)

    int getFFTSize() const { return fftSize; }
    int getOverlapFactor() const { return overlapFactor; }
    int getHopSize() const { return hopSize; }
    int getNumBins() const { return fftSize / 2 + 1; }

    //==============================================================================
    // Spectral Data Access (for freezing/visualization)
    const std::vector<float>& getMagnitudeSpectrum() const { return magnitudeSpectrum; }
    const std::vector<float>& getPhaseSpectrum() const { return phaseSpectrum; }

    void setMagnitudeSpectrum(const std::vector<float>& magnitude);
    void setPhaseSpectrum(const std::vector<float>& phase);

private:
    //==============================================================================
    // FFT Processing
    void processFFTFrame(const float* input, float* output);
    void applyWindow(float* data, int length);
    void computeMagnitudePhase(const float* complexData, int numBins);
    void reconstructComplex(float* complexData, int numBins);

    //==============================================================================
    // FFT Objects
    std::unique_ptr<juce::dsp::FFT> forwardFFT;
    std::unique_ptr<juce::dsp::FFT> inverseFFT;

    int fftOrder = 11;      // 2^11 = 2048 samples
    int fftSize = 2048;
    int overlapFactor = 4;
    int hopSize = 512;      // fftSize / overlapFactor

    //==============================================================================
    // Buffers
    std::vector<float> fftBuffer;           // Complex FFT data (size = fftSize * 2)
    std::vector<float> windowBuffer;        // Hann window (size = fftSize)
    std::vector<float> magnitudeSpectrum;   // Magnitude spectrum (size = fftSize/2 + 1)
    std::vector<float> phaseSpectrum;       // Phase spectrum (size = fftSize/2 + 1)

    //==============================================================================
    // Overlap-Add Buffers (per channel)
    struct ChannelData
    {
        std::vector<float> inputFIFO;       // Input samples waiting for FFT
        std::vector<float> outputFIFO;      // Output samples from IFFT
        int inputWritePos = 0;
        int outputReadPos = 0;
    };

    std::vector<ChannelData> channelData;
    int numChannels = 2;

    //==============================================================================
    // Windowing
    void createHannWindow();

    double sampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectralProcessor)
};
