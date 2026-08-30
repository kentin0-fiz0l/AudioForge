#include "SpectralProcessor.h"

SpectralProcessor::SpectralProcessor()
{
    setFFTSize(2048);  // Default FFT size
}

void SpectralProcessor::prepare(double newSampleRate, int samplesPerBlock, int newNumChannels)
{
    sampleRate = newSampleRate;
    numChannels = newNumChannels;

    // Allocate channel data
    channelData.resize(numChannels);

    for (auto& channel : channelData)
    {
        channel.inputFIFO.resize(fftSize, 0.0f);
        channel.outputFIFO.resize(fftSize * 2, 0.0f);  // Extra space for overlap
        channel.inputWritePos = 0;
        channel.outputReadPos = 0;
    }

    reset();
}

void SpectralProcessor::reset()
{
    for (auto& channel : channelData)
    {
        std::fill(channel.inputFIFO.begin(), channel.inputFIFO.end(), 0.0f);
        std::fill(channel.outputFIFO.begin(), channel.outputFIFO.end(), 0.0f);
        channel.inputWritePos = 0;
        channel.outputReadPos = 0;
    }
}

void SpectralProcessor::setFFTSize(int size)
{
    // Validate and set FFT size (must be power of 2)
    fftSize = juce::nextPowerOfTwo(size);
    fftSize = juce::jlimit(1024, 8192, fftSize);

    // Calculate FFT order (2^order = fftSize)
    fftOrder = (int)std::log2(fftSize);

    // Update hop size
    hopSize = fftSize / overlapFactor;

    // Create FFT objects
    forwardFFT = std::make_unique<juce::dsp::FFT>(fftOrder);
    inverseFFT = std::make_unique<juce::dsp::FFT>(fftOrder);

    // Allocate buffers
    fftBuffer.resize(fftSize * 2, 0.0f);         // Complex data (real + imaginary)
    windowBuffer.resize(fftSize, 0.0f);
    magnitudeSpectrum.resize(fftSize / 2 + 1, 0.0f);
    phaseSpectrum.resize(fftSize / 2 + 1, 0.0f);

    // Create window function
    createHannWindow();

    // Reallocate channel FIFOs
    for (auto& channel : channelData)
    {
        channel.inputFIFO.resize(fftSize, 0.0f);
        channel.outputFIFO.resize(fftSize * 2, 0.0f);
    }
}

void SpectralProcessor::setOverlapFactor(int factor)
{
    overlapFactor = juce::jlimit(2, 8, factor);
    hopSize = fftSize / overlapFactor;
}

void SpectralProcessor::createHannWindow()
{
    // Hann window: 0.5 * (1 - cos(2*pi*n / (N-1)))
    for (int i = 0; i < fftSize; ++i)
    {
        windowBuffer[i] = 0.5f * (1.0f - std::cos(2.0f * juce::MathConstants<float>::pi * i / (fftSize - 1)));
    }
}

void SpectralProcessor::processBlock(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const int numCh = juce::jmin(buffer.getNumChannels(), numChannels);

    for (int ch = 0; ch < numCh; ++ch)
    {
        auto& channel = channelData[ch];
        const float* input = buffer.getReadPointer(ch);
        float* output = buffer.getWritePointer(ch);

        for (int i = 0; i < numSamples; ++i)
        {
            // Write input sample to FIFO
            channel.inputFIFO[channel.inputWritePos] = input[i];
            channel.inputWritePos++;

            // When we have enough samples, process FFT frame
            if (channel.inputWritePos >= hopSize)
            {
                // Process FFT frame
                processFFTFrame(channel.inputFIFO.data(), channel.outputFIFO.data() + channel.outputReadPos);

                // Shift input FIFO (overlap)
                std::copy(channel.inputFIFO.begin() + hopSize,
                         channel.inputFIFO.end(),
                         channel.inputFIFO.begin());

                channel.inputWritePos = fftSize - hopSize;
            }

            // Read output sample from FIFO
            output[i] = channel.outputFIFO[channel.outputReadPos];
            channel.outputFIFO[channel.outputReadPos] = 0.0f;  // Clear for next overlap-add
            channel.outputReadPos++;

            // Wrap output read position
            if (channel.outputReadPos >= fftSize * 2)
                channel.outputReadPos = 0;
        }
    }
}

void SpectralProcessor::processFFTFrame(const float* input, float* output)
{
    // 1. Copy input to FFT buffer and apply window
    for (int i = 0; i < fftSize; ++i)
    {
        fftBuffer[i * 2] = input[i] * windowBuffer[i];      // Real part
        fftBuffer[i * 2 + 1] = 0.0f;                        // Imaginary part
    }

    // 2. Forward FFT (time → frequency)
    forwardFFT->performFrequencyOnlyForwardTransform(fftBuffer.data());

    // 3. Compute magnitude and phase
    computeMagnitudePhase(fftBuffer.data(), getNumBins());

    // 4. Spectral processing happens here (currently pass-through)
    // In Phase 2, we'll replace magnitude with frozen spectrum
    // In Phase 3, we'll evolve the phase

    // 5. Reconstruct complex spectrum from magnitude and phase
    reconstructComplex(fftBuffer.data(), getNumBins());

    // 6. Inverse FFT (frequency → time)
    inverseFFT->performRealOnlyInverseTransform(fftBuffer.data());

    // 7. Apply window and overlap-add to output
    float normalizationFactor = 1.0f / (fftSize * overlapFactor / 4.0f);  // Normalize for overlap-add

    for (int i = 0; i < fftSize; ++i)
    {
        output[i] += fftBuffer[i * 2] * windowBuffer[i] * normalizationFactor;
    }
}

void SpectralProcessor::computeMagnitudePhase(const float* complexData, int numBins)
{
    for (int i = 0; i < numBins; ++i)
    {
        float real = complexData[i * 2];
        float imag = complexData[i * 2 + 1];

        magnitudeSpectrum[i] = std::sqrt(real * real + imag * imag);
        phaseSpectrum[i] = std::atan2(imag, real);
    }
}

void SpectralProcessor::reconstructComplex(float* complexData, int numBins)
{
    for (int i = 0; i < numBins; ++i)
    {
        float magnitude = magnitudeSpectrum[i];
        float phase = phaseSpectrum[i];

        complexData[i * 2] = magnitude * std::cos(phase);      // Real
        complexData[i * 2 + 1] = magnitude * std::sin(phase);  // Imaginary
    }

    // Mirror for real IFFT (conjugate symmetry)
    for (int i = numBins; i < fftSize; ++i)
    {
        int mirrorBin = fftSize - i;
        complexData[i * 2] = complexData[mirrorBin * 2];           // Real (same)
        complexData[i * 2 + 1] = -complexData[mirrorBin * 2 + 1];  // Imag (negated)
    }
}

void SpectralProcessor::applyWindow(float* data, int length)
{
    for (int i = 0; i < length; ++i)
    {
        data[i] *= windowBuffer[i];
    }
}

void SpectralProcessor::setMagnitudeSpectrum(const std::vector<float>& magnitude)
{
    if (magnitude.size() == magnitudeSpectrum.size())
    {
        magnitudeSpectrum = magnitude;
    }
}

void SpectralProcessor::setPhaseSpectrum(const std::vector<float>& phase)
{
    if (phase.size() == phaseSpectrum.size())
    {
        phaseSpectrum = phase;
    }
}
