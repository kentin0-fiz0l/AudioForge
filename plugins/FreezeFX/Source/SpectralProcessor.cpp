#include "SpectralProcessor.h"
#include <cstring>

SpectralProcessor::SpectralProcessor()
{
    // Everything is sized for the largest frame, so that a smaller one only
    // uses part of it
    for (int i = 0; i < 4; ++i)
        ffts[i] = std::make_unique<juce::dsp::FFT>(10 + i);

    const int maxBins = maxFFTSize / 2 + 1;

    fftBuffer.resize(maxFFTSize * 2, 0.0f);  // Complex data (real + imaginary)
    windowBuffer.resize(maxFFTSize, 0.0f);

    // Both double buffers for lock-free access
    for (int i = 0; i < 2; ++i)
    {
        magnitudeSpectrum[i].reserve(maxBins);
        phaseSpectrum[i].reserve(maxBins);
    }

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
        channel.inputFIFO.reserve(maxFFTSize);
        channel.outputFIFO.reserve(maxFFTSize);
        channel.inputFIFO.resize(fftSize, 0.0f);
        channel.outputFIFO.resize(fftSize, 0.0f);
    }

    reset();
}

void SpectralProcessor::reset()
{
    for (auto& channel : channelData)
    {
        std::fill(channel.inputFIFO.begin(), channel.inputFIFO.end(), 0.0f);
        std::fill(channel.outputFIFO.begin(), channel.outputFIFO.end(), 0.0f);

        // New samples fill the last hop of the window; the rest is history
        channel.inputWritePos = fftSize - hopSize;
        channel.outputReadPos = 0;
    }
}

void SpectralProcessor::setFFTSize(int size)
{
    // Validate and set FFT size (must be power of 2)
    fftSize = juce::jlimit(1024, maxFFTSize, juce::nextPowerOfTwo(size));

    // Calculate FFT order (2^order = fftSize)
    fftOrder = (int)std::log2(fftSize);
    fft = ffts[fftOrder - 10].get();

    // Update hop size
    hopSize = fftSize / overlapFactor;

    const int numBins = getNumBins();
    for (int i = 0; i < 2; ++i)
    {
        magnitudeSpectrum[i].resize(numBins, 0.0f);
        phaseSpectrum[i].resize(numBins, 0.0f);
    }

    createWindow();
    fitChannelsToFrame();
}

void SpectralProcessor::setOverlapFactor(int factor)
{
    overlapFactor = juce::jlimit(2, 8, factor);
    hopSize = fftSize / overlapFactor;

    fitChannelsToFrame();
}

void SpectralProcessor::fitChannelsToFrame()
{
    // The next frame is due once a hop of new samples has arrived, so the
    // input FIFO holds fftSize - hopSize samples of history. Keep the most
    // recent of what it held, and pad with silence before it if there is
    // not enough. The FIFOs have room for the largest frame (see prepare),
    // so none of this allocates.
    const int history = fftSize - hopSize;

    for (auto& channel : channelData)
    {
        auto& input = channel.inputFIFO;
        const int kept = juce::jmin(channel.inputWritePos, history);

        if (static_cast<int>(input.size()) < fftSize)
            input.resize(static_cast<size_t>(fftSize), 0.0f);

        std::memmove(input.data() + history - kept, input.data() + channel.inputWritePos - kept,
                     static_cast<size_t>(kept) * sizeof(float));
        std::fill(input.begin(), input.begin() + history - kept, 0.0f);
        input.resize(static_cast<size_t>(fftSize));
        channel.inputWritePos = history;

        // Frames of the old size do not overlap-add with frames of the new
        channel.outputFIFO.assign(static_cast<size_t>(fftSize), 0.0f);
        channel.outputReadPos = 0;
    }
}

void SpectralProcessor::createWindow()
{
    // Square root of a periodic Hann window. It is applied before the FFT and
    // again after the inverse FFT, so each frame is shaped by a full Hann,
    // and Hann frames overlapped by 2, 4 or 8 sum to a constant.
    for (int i = 0; i < fftSize; ++i)
    {
        windowBuffer[i] = std::sqrt(0.5f * (1.0f - std::cos(2.0f * juce::MathConstants<float>::pi * i / fftSize)));
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

            // A full hop of new samples has arrived: process an FFT frame
            if (channel.inputWritePos >= fftSize)
            {
                // Drop the hop of output already played and make room for
                // the tail of the new frame
                std::copy(channel.outputFIFO.begin() + hopSize,
                         channel.outputFIFO.end(),
                         channel.outputFIFO.begin());
                std::fill(channel.outputFIFO.end() - hopSize, channel.outputFIFO.end(), 0.0f);
                channel.outputReadPos = 0;

                // Overlap-add the frame onto what earlier frames left there
                processFFTFrame(ch, channel.inputFIFO.data(), channel.outputFIFO.data());

                shiftInput(channel);
            }

            // Read output sample from FIFO. At most hopSize samples are read
            // between frames, so this stays inside the buffer.
            output[i] = channel.outputFIFO[channel.outputReadPos];
            channel.outputReadPos++;
        }
    }
}

void SpectralProcessor::pushInput(const juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const int numCh = juce::jmin(buffer.getNumChannels(), numChannels);

    for (int ch = 0; ch < numCh; ++ch)
    {
        auto& channel = channelData[ch];
        const float* input = buffer.getReadPointer(ch);

        for (int i = 0; i < numSamples; ++i)
        {
            channel.inputFIFO[channel.inputWritePos] = input[i];
            channel.inputWritePos++;

            if (channel.inputWritePos >= fftSize)
                shiftInput(channel);
        }
    }
}

void SpectralProcessor::clearOutput()
{
    for (auto& channel : channelData)
    {
        std::fill(channel.outputFIFO.begin(), channel.outputFIFO.end(), 0.0f);
        channel.outputReadPos = 0;
    }
}

void SpectralProcessor::shiftInput(ChannelData& channel)
{
    // Drop the oldest hop, leaving room for the next one at the end
    std::copy(channel.inputFIFO.begin() + hopSize,
             channel.inputFIFO.end(),
             channel.inputFIFO.begin());

    channel.inputWritePos = fftSize - hopSize;
}

void SpectralProcessor::processFFTFrame(int channel, const float* input, float* output)
{
    // 1. Copy input to FFT buffer and apply window. The FFT takes fftSize
    //    real samples and needs the second half of the buffer as workspace.
    for (int i = 0; i < fftSize; ++i)
    {
        fftBuffer[i] = input[i] * windowBuffer[i];
        fftBuffer[fftSize + i] = 0.0f;
    }

    // 2. Forward FFT (time → frequency), giving interleaved real/imaginary pairs
    fft->performRealOnlyForwardTransform(fftBuffer.data());

    // 3. Compute magnitude and phase
    computeMagnitudePhase(fftBuffer.data(), getNumBins());

    // 4. Spectral processing callback (freezing, phase evolution, etc.)
    // Callback operates on the buffer we just wrote to
    if (spectralCallback)
    {
        int currentWriteIdx = activeWriteBuffer.load(std::memory_order_acquire);
        spectralCallback(channel, magnitudeSpectrum[currentWriteIdx], phaseSpectrum[currentWriteIdx]);
    }

    // 5. Reconstruct complex spectrum from magnitude and phase
    reconstructComplex(fftBuffer.data(), getNumBins());

    // 6. Inverse FFT (frequency → time), leaving fftSize real samples at the
    //    start of the buffer
    fft->performRealOnlyInverseTransform(fftBuffer.data());

    // 7. Apply window and overlap-add to output. Overlapped Hann frames sum
    //    to overlapFactor / 2.
    float normalizationFactor = 2.0f / static_cast<float>(overlapFactor);

    for (int i = 0; i < fftSize; ++i)
    {
        output[i] += fftBuffer[i] * windowBuffer[i] * normalizationFactor;
    }
}

void SpectralProcessor::computeMagnitudePhase(const float* complexData, int numBins)
{
    // Lock-free: Swap to new write buffer FIRST, then write
    // This ensures UI reads from the previous buffer (which is stable)
    int oldWriteIdx = activeWriteBuffer.load(std::memory_order_acquire);
    int newWriteIdx = 1 - oldWriteIdx;
    activeWriteBuffer.store(newWriteIdx, std::memory_order_release);

    // Now write to the new buffer (UI is reading from the old one)
    for (int i = 0; i < numBins; ++i)
    {
        float real = complexData[i * 2];
        float imag = complexData[i * 2 + 1];

        magnitudeSpectrum[newWriteIdx][i] = std::sqrt(real * real + imag * imag);
        phaseSpectrum[newWriteIdx][i] = std::atan2(imag, real);
    }
}

void SpectralProcessor::reconstructComplex(float* complexData, int numBins)
{
    // Lock-free: Read from the current write buffer (the one we just wrote to)
    int currentWriteIdx = activeWriteBuffer.load(std::memory_order_acquire);

    for (int i = 0; i < numBins; ++i)
    {
        float magnitude = magnitudeSpectrum[currentWriteIdx][i];
        float phase = phaseSpectrum[currentWriteIdx][i];

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

// Thread-safe getters using lock-free double buffering
// UI thread reads from the buffer that audio thread is NOT writing to
std::vector<float> SpectralProcessor::getMagnitudeSpectrum() const
{
    // Read the current write index atomically
    int writeIdx = activeWriteBuffer.load(std::memory_order_acquire);
    // Read from the OTHER buffer (the one audio thread just finished writing)
    int readIdx = 1 - writeIdx;
    return magnitudeSpectrum[readIdx];  // Return copy of read buffer
}

std::vector<float> SpectralProcessor::getPhaseSpectrum() const
{
    // Read the current write index atomically
    int writeIdx = activeWriteBuffer.load(std::memory_order_acquire);
    // Read from the OTHER buffer (the one audio thread just finished writing)
    int readIdx = 1 - writeIdx;
    return phaseSpectrum[readIdx];  // Return copy of read buffer
}

void SpectralProcessor::setMagnitudeSpectrum(const std::vector<float>& magnitude)
{
    // Write to the read buffer (the one UI is currently displaying)
    // This way the freeze feature can update what's shown
    int writeIdx = activeWriteBuffer.load(std::memory_order_acquire);
    int readIdx = 1 - writeIdx;

    if (magnitude.size() == magnitudeSpectrum[readIdx].size())
    {
        magnitudeSpectrum[readIdx] = magnitude;
    }
}

void SpectralProcessor::setPhaseSpectrum(const std::vector<float>& phase)
{
    // Write to the read buffer (the one UI is currently displaying)
    int writeIdx = activeWriteBuffer.load(std::memory_order_acquire);
    int readIdx = 1 - writeIdx;

    if (phase.size() == phaseSpectrum[readIdx].size())
    {
        phaseSpectrum[readIdx] = phase;
    }
}
