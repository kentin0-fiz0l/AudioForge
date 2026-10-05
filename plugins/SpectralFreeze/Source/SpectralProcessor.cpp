#include "SpectralProcessor.h"
#include <algorithm>
#include <cmath>

SpectralFreezeEngine::SpectralFreezeEngine()
{
    // Initialize spectrum vectors with safe defaults (FFT_SIZE/2 + 1 bins)
    // This prevents crashes if editor accesses spectrum before prepare() is called
    frozenMagnitude.resize(FFT_SIZE / 2 + 1, 0.0f);
    currentMagnitude.resize(FFT_SIZE / 2 + 1, 0.0f);
    currentPhase.resize(FFT_SIZE / 2 + 1, 0.0f);
}

void SpectralFreezeEngine::prepare(double newSampleRate, int samplesPerBlock, int numChannels)
{
    sampleRate = newSampleRate;

    // Allocate buffers for each channel
    fftBuffer.resize(numChannels);
    inputFifo.resize(numChannels);
    outputFifo.resize(numChannels);
    inputFifoIndex.resize(numChannels);
    outputFifoIndex.resize(numChannels);
    fftCounter.resize(numChannels);

    for (int ch = 0; ch < numChannels; ++ch)
    {
        fftBuffer[ch].resize(FFT_SIZE * 2, 0.0f);  // Real + imaginary
        inputFifo[ch].resize(FFT_SIZE, 0.0f);
        outputFifo[ch].resize(FFT_SIZE, 0.0f);
        inputFifoIndex[ch] = 0;
        outputFifoIndex[ch] = 0;
        fftCounter[ch] = 0;
    }

    const size_t numBins = FFT_SIZE / 2 + 1;

    // Initialize frozen spectrum, one per channel
    frozenMagnitude.assign(numBins, 0.0f);
    held.assign(static_cast<size_t>(numChannels), HeldSpectrum());

    for (auto& channel : held)
    {
        channel.magnitude.assign(numBins, 0.0f);
        channel.phase.assign(numBins, 0.0f);
        channel.phaseAdvance.assign(numBins, 0.0f);
    }

    // Initialize morph state
    currentMagnitude.assign(numBins, 0.0f);
    currentPhase.assign(numBins, 0.0f);

    // Working buffers
    magnitudeScratch.assign(numBins, 0.0f);
    phaseScratch.assign(numBins, 0.0f);
    effectScratch.assign(numBins, 0.0f);
    dryBuffer.setSize(numChannels, samplesPerBlock);

    // Generate window table
    windowTable.resize(FFT_SIZE);
    juce::dsp::WindowingFunction<float> windowFunction(FFT_SIZE, juce::dsp::WindowingFunction<float>::hann);
    windowFunction.fillWindowingTables(windowTable.data(), FFT_SIZE, juce::dsp::WindowingFunction<float>::hann);

    // Every sample is covered by FFT_SIZE / HOP_SIZE frames, each windowed
    // on the way in and again on the way out. Their sum is what the output
    // has to be divided by to come back at the input's level.
    double windowSquared = 0.0;
    for (float value : windowTable)
        windowSquared += static_cast<double>(value) * static_cast<double>(value);

    synthesisScale = static_cast<float>(HOP_SIZE / windowSquared);

    // Mark processor as prepared
    isPrepared = true;
}

void SpectralFreezeEngine::reset()
{
    for (auto& fifo : inputFifo)
        std::fill(fifo.begin(), fifo.end(), 0.0f);

    for (auto& fifo : outputFifo)
        std::fill(fifo.begin(), fifo.end(), 0.0f);

    for (auto& buffer : fftBuffer)
        std::fill(buffer.begin(), buffer.end(), 0.0f);

    std::fill(inputFifoIndex.begin(), inputFifoIndex.end(), 0);
    std::fill(outputFifoIndex.begin(), outputFifoIndex.end(), 0);
    std::fill(fftCounter.begin(), fftCounter.end(), 0);

    for (auto& channel : held)
        channel.captured = false;
}

void SpectralFreezeEngine::setFreeze(bool shouldFreeze)
{
    frozen = shouldFreeze;
}

void SpectralFreezeEngine::setBlurAmount(float amount)
{
    blurAmount = juce::jlimit(0.0f, 1.0f, amount);
}

void SpectralFreezeEngine::setLowCutFreq(float freq)
{
    lowCutFreq = juce::jlimit(20.0f, 20000.0f, freq);
}

void SpectralFreezeEngine::setHighCutFreq(float freq)
{
    highCutFreq = juce::jlimit(20.0f, 20000.0f, freq);
}

void SpectralFreezeEngine::setDryWet(float mix)
{
    dryWet = juce::jlimit(0.0f, 1.0f, mix);
}

void SpectralFreezeEngine::setStretchAmount(float amount)
{
    stretchAmount = juce::jlimit(0.0f, 1.0f, amount);
}

void SpectralFreezeEngine::setShiftAmount(float amount)
{
    shiftAmount = juce::jlimit(-1.0f, 1.0f, amount);
}

void SpectralFreezeEngine::setMorphAmount(float amount)
{
    morphAmount = juce::jlimit(0.0f, 1.0f, amount);
}

void SpectralFreezeEngine::setGateThreshold(float threshold)
{
    gateThreshold = juce::jlimit(0.0f, 1.0f, threshold);
}

void SpectralFreezeEngine::setRandomizeAmount(float amount)
{
    randomizeAmount = juce::jlimit(0.0f, 1.0f, amount);
}

void SpectralFreezeEngine::processBlock(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    // Only the channels prepare() set up; before prepare() that is none
    const int numChannels = juce::jmin(buffer.getNumChannels(), static_cast<int>(held.size()));

    // Phase 5: Smooth parameters to prevent audio clicks
    smoothedBlur = smoothedBlur * SMOOTHING_COEFF + blurAmount * (1.0f - SMOOTHING_COEFF);
    smoothedLowCut = smoothedLowCut * SMOOTHING_COEFF + lowCutFreq * (1.0f - SMOOTHING_COEFF);
    smoothedHighCut = smoothedHighCut * SMOOTHING_COEFF + highCutFreq * (1.0f - SMOOTHING_COEFF);
    smoothedDryWet = smoothedDryWet * SMOOTHING_COEFF + dryWet * (1.0f - SMOOTHING_COEFF);
    smoothedStretch = smoothedStretch * SMOOTHING_COEFF + stretchAmount * (1.0f - SMOOTHING_COEFF);
    smoothedShift = smoothedShift * SMOOTHING_COEFF + shiftAmount * (1.0f - SMOOTHING_COEFF);
    smoothedMorph = smoothedMorph * SMOOTHING_COEFF + morphAmount * (1.0f - SMOOTHING_COEFF);
    smoothedGate = smoothedGate * SMOOTHING_COEFF + gateThreshold * (1.0f - SMOOTHING_COEFF);
    smoothedRandomize = smoothedRandomize * SMOOTHING_COEFF + randomizeAmount * (1.0f - SMOOTHING_COEFF);

    // Save dry signal for mixing. The buffer was sized in prepare(), so
    // this does not allocate unless the host sends a larger block.
    dryBuffer.setSize(numChannels, numSamples, false, false, true);
    for (int ch = 0; ch < numChannels; ++ch)
        dryBuffer.copyFrom(ch, 0, buffer, ch, 0, numSamples);

    // Process each channel
    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* channelData = buffer.getWritePointer(ch);
        processFFT(channelData, numSamples, ch);
    }

    // Dry/wet mix (use smoothed value)
    if (smoothedDryWet < 1.0f)
    {
        for (int ch = 0; ch < numChannels; ++ch)
        {
            float* wet = buffer.getWritePointer(ch);
            const float* dry = dryBuffer.getReadPointer(ch);

            for (int i = 0; i < numSamples; ++i)
                wet[i] = dry[i] * (1.0f - smoothedDryWet) + wet[i] * smoothedDryWet;
        }
    }
}

void SpectralFreezeEngine::processFFT(float* channelData, int numSamples, int channel)
{
    for (int i = 0; i < numSamples; ++i)
    {
        // Write to input FIFO
        inputFifo[channel][inputFifoIndex[channel]] = channelData[i];
        inputFifoIndex[channel] = (inputFifoIndex[channel] + 1) % FFT_SIZE;

        // Read from output FIFO
        channelData[i] = outputFifo[channel][outputFifoIndex[channel]];
        outputFifo[channel][outputFifoIndex[channel]] = 0.0f;  // Clear after reading
        outputFifoIndex[channel] = (outputFifoIndex[channel] + 1) % FFT_SIZE;

        // Process FFT when hop size reached
        if (++fftCounter[channel] >= HOP_SIZE)
        {
            fftCounter[channel] = 0;

            // Copy from FIFO to FFT buffer with windowing
            for (int j = 0; j < FFT_SIZE; ++j)
            {
                int fifoIndex = (inputFifoIndex[channel] + j) % FFT_SIZE;
                fftBuffer[channel][j] = inputFifo[channel][fifoIndex] * windowTable[j];
                fftBuffer[channel][j + FFT_SIZE] = 0.0f;  // Clear imaginary part
            }

            // Forward FFT. The result is pairs: real, imaginary, for each bin.
            fft.performRealOnlyForwardTransform(fftBuffer[channel].data());

            // Extract magnitude and phase
            auto& magnitude = magnitudeScratch;
            auto& phase = phaseScratch;

            for (int j = 0; j <= FFT_SIZE / 2; ++j)
            {
                float real = fftBuffer[channel][2 * j];
                float imag = fftBuffer[channel][2 * j + 1];
                magnitude[j] = std::sqrt(real * real + imag * imag);
                phase[j] = std::atan2(imag, real);
            }

            // Apply spectral effects
            applySpectralEffects(channel, magnitude, phase);

            // Reconstruct complex spectrum. The inverse transform works out
            // the upper half for itself.
            for (int j = 0; j <= FFT_SIZE / 2; ++j)
            {
                fftBuffer[channel][2 * j] = magnitude[j] * std::cos(phase[j]);
                fftBuffer[channel][2 * j + 1] = magnitude[j] * std::sin(phase[j]);
            }

            // Inverse FFT, which also divides by FFT_SIZE
            fft.performRealOnlyInverseTransform(fftBuffer[channel].data());

            // Overlap-add to output FIFO
            for (int j = 0; j < FFT_SIZE; ++j)
            {
                int outputIndex = (outputFifoIndex[channel] + j) % FFT_SIZE;
                outputFifo[channel][outputIndex] += fftBuffer[channel][j] * windowTable[j] * synthesisScale;
            }
        }
    }
}

void SpectralFreezeEngine::applySpectralEffects(int channel, std::vector<float>& magnitude, std::vector<float>& phase)
{
    // Store current spectrum for morphing
    currentMagnitude = magnitude;
    currentPhase = phase;

    auto& channelHeld = held[static_cast<size_t>(channel)];
    const size_t numBins = magnitude.size();
    const float twoPi = juce::MathConstants<float>::twoPi;

    if (frozen)
    {
        if (! channelHeld.captured)
        {
            // First frozen frame: capture the spectrum. Until the next
            // frame says better, assume each bin holds a tone at the bin's
            // own frequency, whose phase moves bin * hop / size turns a frame.
            channelHeld.captured = true;
            channelHeld.measurePending = true;

            for (size_t bin = 0; bin < numBins; ++bin)
            {
                const float turns = static_cast<float>((bin * HOP_SIZE) % FFT_SIZE) / static_cast<float>(FFT_SIZE);

                channelHeld.magnitude[bin] = magnitude[bin];
                channelHeld.phase[bin] = phase[bin];
                channelHeld.phaseAdvance[bin] = std::remainder(twoPi * turns, twoPi);
            }

            if (channel == 0)
                frozenMagnitude = magnitude;
        }
        else
        {
            // One frame on, the input shows how far each bin's phase
            // really moves in a hop: the exact frequency of what is in it.
            // Use that wherever the same sound is still there to measure.
            if (channelHeld.measurePending)
            {
                channelHeld.measurePending = false;

                for (size_t bin = 0; bin < numBins; ++bin)
                {
                    const float captured = channelHeld.magnitude[bin];

                    if (captured > 1.0e-6f && magnitude[bin] > 0.25f * captured && magnitude[bin] < 4.0f * captured)
                        channelHeld.phaseAdvance[bin] = std::remainder(phase[bin] - channelHeld.phase[bin], twoPi);
                }
            }

            for (size_t bin = 0; bin < numBins; ++bin)
                channelHeld.phase[bin] = std::remainder(channelHeld.phase[bin] + channelHeld.phaseAdvance[bin], twoPi);
        }

        // Use frozen spectrum
        for (size_t bin = 0; bin < numBins; ++bin)
        {
            magnitude[bin] = channelHeld.magnitude[bin];
            phase[bin] = channelHeld.phase[bin];
        }
    }
    else
    {
        // Not frozen: forget what was held
        channelHeld.captured = false;

        if (channel == 0)
            std::fill(frozenMagnitude.begin(), frozenMagnitude.end(), 0.0f);
    }

    // Phase 2: Advanced Effects (applied after freeze, using smoothed parameters)
    if (smoothedStretch > 0.0f)
        applySpectralStretch(magnitude);

    if (std::abs(smoothedShift) > 0.01f)
        applySpectralShift(magnitude);

    if (smoothedMorph > 0.0f && !frozen)
        applySpectralMorph(magnitude, phase);

    if (smoothedGate > 0.0f)
        applySpectralGate(magnitude);

    if (smoothedRandomize > 0.0f)
        applySpectralRandomize(phase);

    // Apply spectral blur (smoothed)
    if (smoothedBlur > 0.0f)
        applySpectralBlur(magnitude);

    // Apply frequency filtering (smoothed)
    applyFrequencyFilter(magnitude);
}

void SpectralFreezeEngine::applySpectralBlur(std::vector<float>& magnitude)
{
    // Gaussian blur across frequency bins
    auto& blurred = effectScratch;
    blurred = magnitude;
    int kernelSize = (int)(smoothedBlur * 10.0f) + 1;  // 1-11 bins

    for (size_t i = 0; i < magnitude.size(); ++i)
    {
        float sum = 0.0f;
        float weight = 0.0f;

        for (int k = -kernelSize; k <= kernelSize; ++k)
        {
            int index = (int)i + k;
            if (index >= 0 && index < (int)magnitude.size())
            {
                float gaussian = std::exp(-0.5f * (k * k) / (kernelSize * kernelSize));
                sum += magnitude[index] * gaussian;
                weight += gaussian;
            }
        }

        blurred[i] = sum / weight;
    }

    magnitude = blurred;
}

void SpectralFreezeEngine::applyFrequencyFilter(std::vector<float>& magnitude)
{
    // Convert frequency to bin index (using smoothed parameters)
    float binToHz = (float)sampleRate / FFT_SIZE;
    int lowBin = (int)(smoothedLowCut / binToHz);
    int highBin = (int)(smoothedHighCut / binToHz);

    // Apply filters
    for (size_t i = 0; i < magnitude.size(); ++i)
    {
        if ((int)i < lowBin || (int)i > highBin)
            magnitude[i] = 0.0f;
    }
}

//==============================================================================
// Phase 2: Advanced Spectral Effects

void SpectralFreezeEngine::applySpectralStretch(std::vector<float>& magnitude)
{
    // Time-stretch by interpolating/repeating bins
    // Higher stretch = slower time evolution (more repetition)
    auto& stretched = effectScratch;
    stretched = magnitude;
    float stretchFactor = 1.0f + smoothedStretch * 4.0f;  // 1.0 to 5.0x stretch

    for (size_t i = 0; i < magnitude.size(); ++i)
    {
        float sourceIndex = (float)i / stretchFactor;
        int index0 = (int)sourceIndex;
        int index1 = index0 + 1;
        float frac = sourceIndex - index0;

        if (index0 >= 0 && index1 < (int)magnitude.size())
        {
            // Linear interpolation
            stretched[i] = magnitude[index0] * (1.0f - frac) + magnitude[index1] * frac;
        }
        else if (index0 >= 0 && index0 < (int)magnitude.size())
        {
            stretched[i] = magnitude[index0];
        }
    }

    magnitude = stretched;
}

void SpectralFreezeEngine::applySpectralShift(std::vector<float>& magnitude)
{
    // Frequency shift by shifting bins (formant shifting)
    // Positive shift = higher frequencies, negative = lower frequencies
    auto& shifted = effectScratch;
    shifted.assign(magnitude.size(), 0.0f);
    int shiftBins = (int)(smoothedShift * 100.0f);  // ±100 bins (~2 octaves at 2048 FFT)

    for (size_t i = 0; i < magnitude.size(); ++i)
    {
        int newIndex = (int)i + shiftBins;
        if (newIndex >= 0 && newIndex < (int)magnitude.size())
        {
            shifted[newIndex] = magnitude[i];
        }
    }

    magnitude = shifted;
}

void SpectralFreezeEngine::applySpectralMorph(std::vector<float>& magnitude, const std::vector<float>& phase)
{
    // Morph between current and frozen spectrum
    // This creates a crossfade effect even when not frozen
    if (frozenMagnitude[0] == 0.0f)
        return;  // No frozen spectrum to morph to

    for (size_t i = 0; i < magnitude.size(); ++i)
    {
        magnitude[i] = currentMagnitude[i] * (1.0f - smoothedMorph) + frozenMagnitude[i] * smoothedMorph;
    }
}

void SpectralFreezeEngine::applySpectralGate(std::vector<float>& magnitude)
{
    // Threshold-based spectral gate
    // Silence bins below threshold (relative to max magnitude)
    float maxMagnitude = 0.0f;
    for (size_t i = 0; i < magnitude.size(); ++i)
        maxMagnitude = std::max(maxMagnitude, magnitude[i]);

    float threshold = maxMagnitude * smoothedGate;

    for (size_t i = 0; i < magnitude.size(); ++i)
    {
        if (magnitude[i] < threshold)
            magnitude[i] = 0.0f;
    }
}

void SpectralFreezeEngine::applySpectralRandomize(std::vector<float>& phase)
{
    // Randomize phase for texture/grain effects
    // Creates a frozen but textured sound
    for (size_t i = 0; i < phase.size(); ++i)
    {
        if (smoothedRandomize > 0.5f || (std::rand() / (float)RAND_MAX) < smoothedRandomize)
        {
            // Random phase between -π and π
            float randomPhase = (std::rand() / (float)RAND_MAX) * 2.0f * juce::MathConstants<float>::pi - juce::MathConstants<float>::pi;
            phase[i] = phase[i] * (1.0f - smoothedRandomize) + randomPhase * smoothedRandomize;
        }
    }
}
