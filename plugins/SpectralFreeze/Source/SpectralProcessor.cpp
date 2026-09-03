#include "SpectralProcessor.h"
#include <algorithm>
#include <cmath>

SpectralProcessor::SpectralProcessor()
{
    // Initialize spectrum vectors with safe defaults (FFT_SIZE/2 + 1 bins)
    // This prevents crashes if editor accesses spectrum before prepare() is called
    frozenMagnitude.resize(FFT_SIZE / 2 + 1, 0.0f);
    frozenPhase.resize(FFT_SIZE / 2 + 1, 0.0f);
    currentMagnitude.resize(FFT_SIZE / 2 + 1, 0.0f);
    currentPhase.resize(FFT_SIZE / 2 + 1, 0.0f);
}

void SpectralProcessor::prepare(double newSampleRate, int samplesPerBlock, int numChannels)
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

    // Initialize frozen spectrum
    frozenMagnitude.resize(FFT_SIZE / 2 + 1, 0.0f);
    frozenPhase.resize(FFT_SIZE / 2 + 1, 0.0f);

    // Initialize morph state
    currentMagnitude.resize(FFT_SIZE / 2 + 1, 0.0f);
    currentPhase.resize(FFT_SIZE / 2 + 1, 0.0f);

    // Generate window table
    windowTable.resize(FFT_SIZE);
    juce::dsp::WindowingFunction<float> windowFunction(FFT_SIZE, juce::dsp::WindowingFunction<float>::hann);
    windowFunction.fillWindowingTables(windowTable.data(), FFT_SIZE, juce::dsp::WindowingFunction<float>::hann);

    // Mark processor as prepared
    isPrepared = true;
}

void SpectralProcessor::reset()
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
}

void SpectralProcessor::setFreeze(bool shouldFreeze)
{
    frozen = shouldFreeze;
}

void SpectralProcessor::setBlurAmount(float amount)
{
    blurAmount = juce::jlimit(0.0f, 1.0f, amount);
}

void SpectralProcessor::setLowCutFreq(float freq)
{
    lowCutFreq = juce::jlimit(20.0f, 20000.0f, freq);
}

void SpectralProcessor::setHighCutFreq(float freq)
{
    highCutFreq = juce::jlimit(20.0f, 20000.0f, freq);
}

void SpectralProcessor::setDryWet(float mix)
{
    dryWet = juce::jlimit(0.0f, 1.0f, mix);
}

void SpectralProcessor::setStretchAmount(float amount)
{
    stretchAmount = juce::jlimit(0.0f, 1.0f, amount);
}

void SpectralProcessor::setShiftAmount(float amount)
{
    shiftAmount = juce::jlimit(-1.0f, 1.0f, amount);
}

void SpectralProcessor::setMorphAmount(float amount)
{
    morphAmount = juce::jlimit(0.0f, 1.0f, amount);
}

void SpectralProcessor::setGateThreshold(float threshold)
{
    gateThreshold = juce::jlimit(0.0f, 1.0f, threshold);
}

void SpectralProcessor::setRandomizeAmount(float amount)
{
    randomizeAmount = juce::jlimit(0.0f, 1.0f, amount);
}

void SpectralProcessor::processBlock(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

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

    // Save dry signal for mixing
    juce::AudioBuffer<float> dryBuffer(numChannels, numSamples);
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

void SpectralProcessor::processFFT(float* channelData, int numSamples, int channel)
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

            // Forward FFT
            fft.performRealOnlyForwardTransform(fftBuffer[channel].data());

            // Extract magnitude and phase
            std::vector<float> magnitude(FFT_SIZE / 2 + 1);
            std::vector<float> phase(FFT_SIZE / 2 + 1);

            for (int j = 0; j <= FFT_SIZE / 2; ++j)
            {
                float real = fftBuffer[channel][j];
                float imag = fftBuffer[channel][j + FFT_SIZE];
                magnitude[j] = std::sqrt(real * real + imag * imag);
                phase[j] = std::atan2(imag, real);
            }

            // Apply spectral effects
            applySpectralEffects(magnitude, phase);

            // Reconstruct complex spectrum
            for (int j = 0; j <= FFT_SIZE / 2; ++j)
            {
                fftBuffer[channel][j] = magnitude[j] * std::cos(phase[j]);
                fftBuffer[channel][j + FFT_SIZE] = magnitude[j] * std::sin(phase[j]);
            }

            // Inverse FFT
            fft.performRealOnlyInverseTransform(fftBuffer[channel].data());

            // Overlap-add to output FIFO
            for (int j = 0; j < FFT_SIZE; ++j)
            {
                int outputIndex = (outputFifoIndex[channel] + j) % FFT_SIZE;
                outputFifo[channel][outputIndex] += fftBuffer[channel][j] * windowTable[j] / FFT_SIZE;
            }
        }
    }
}

void SpectralProcessor::applySpectralEffects(std::vector<float>& magnitude, std::vector<float>& phase)
{
    // Store current spectrum for morphing
    currentMagnitude = magnitude;
    currentPhase = phase;

    // Freeze spectrum if enabled
    if (frozen)
    {
        if (frozenMagnitude[0] == 0.0f)
        {
            // First freeze: capture current spectrum
            freezeSpectrum(magnitude, phase);
        }

        // Use frozen spectrum
        magnitude = frozenMagnitude;
        phase = frozenPhase;
    }
    else
    {
        // Not frozen: clear frozen spectrum
        std::fill(frozenMagnitude.begin(), frozenMagnitude.end(), 0.0f);
        std::fill(frozenPhase.begin(), frozenPhase.end(), 0.0f);
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

void SpectralProcessor::freezeSpectrum(const std::vector<float>& magnitude, const std::vector<float>& phase)
{
    frozenMagnitude = magnitude;
    frozenPhase = phase;
}

void SpectralProcessor::applySpectralBlur(std::vector<float>& magnitude)
{
    // Gaussian blur across frequency bins
    std::vector<float> blurred = magnitude;
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

void SpectralProcessor::applyFrequencyFilter(std::vector<float>& magnitude)
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

void SpectralProcessor::applySpectralStretch(std::vector<float>& magnitude)
{
    // Time-stretch by interpolating/repeating bins
    // Higher stretch = slower time evolution (more repetition)
    std::vector<float> stretched = magnitude;
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

void SpectralProcessor::applySpectralShift(std::vector<float>& magnitude)
{
    // Frequency shift by shifting bins (formant shifting)
    // Positive shift = higher frequencies, negative = lower frequencies
    std::vector<float> shifted(magnitude.size(), 0.0f);
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

void SpectralProcessor::applySpectralMorph(std::vector<float>& magnitude, const std::vector<float>& phase)
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

void SpectralProcessor::applySpectralGate(std::vector<float>& magnitude)
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

void SpectralProcessor::applySpectralRandomize(std::vector<float>& phase)
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
