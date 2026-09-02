/*
  ==============================================================================

    MultibandEngine.cpp
    Created: 1 Sep 2026
    Author:  AudioForge Team

  ==============================================================================
*/

#include "MultibandEngine.h"

namespace audioforge
{

MultibandEngine::MultibandEngine()
{
}

void MultibandEngine::prepare(double sampleRate, int maxBlockSize)
{
    currentSampleRate = sampleRate;
    maxSamplesPerBlock = maxBlockSize;

    // Prepare crossover
    crossover.prepare(sampleRate, maxBlockSize);

    // Prepare all band compressors
    for (int band = 0; band < NumBands; ++band)
    {
        bandCompressors[band].prepare(sampleRate, maxBlockSize);
    }

    // Allocate band buffers (stereo)
    for (int band = 0; band < NumBands; ++band)
    {
        bandBuffers[band].setSize(2, maxBlockSize);
    }

    reset();
}

void MultibandEngine::reset()
{
    crossover.reset();

    for (int band = 0; band < NumBands; ++band)
    {
        bandCompressors[band].reset();
        bandBuffers[band].clear();
    }
}

void MultibandEngine::setCrossoverFrequency(int index, float frequency)
{
    crossover.setCrossoverFrequency(index, frequency);
}

float MultibandEngine::getCrossoverFrequency(int index) const
{
    return crossover.getCrossoverFrequency(index);
}

BandCompressor& MultibandEngine::getBandCompressor(int bandIndex)
{
    jassert(bandIndex >= 0 && bandIndex < NumBands);
    return bandCompressors[bandIndex];
}

void MultibandEngine::setBandSolo(int bandIndex, bool enabled)
{
    jassert(bandIndex >= 0 && bandIndex < NumBands);
    bandSolo[bandIndex] = enabled;
}

bool MultibandEngine::isBandSoloed(int bandIndex) const
{
    jassert(bandIndex >= 0 && bandIndex < NumBands);
    return bandSolo[bandIndex];
}

void MultibandEngine::setBandBypass(int bandIndex, bool enabled)
{
    jassert(bandIndex >= 0 && bandIndex < NumBands);
    bandBypass[bandIndex] = enabled;
}

bool MultibandEngine::isBandBypassed(int bandIndex) const
{
    jassert(bandIndex >= 0 && bandIndex < NumBands);
    return bandBypass[bandIndex];
}

void MultibandEngine::setBandMidSideEnabled(int bandIndex, bool enabled)
{
    jassert(bandIndex >= 0 && bandIndex < NumBands);
    bandMidSideEnabled[bandIndex] = enabled;
}

bool MultibandEngine::isBandMidSideEnabled(int bandIndex) const
{
    jassert(bandIndex >= 0 && bandIndex < NumBands);
    return bandMidSideEnabled[bandIndex];
}

void MultibandEngine::process(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    if (numSamples == 0 || numChannels < 2)
        return;

    //==============================================================================
    // Multiband Processing Algorithm
    //
    // 1. Split stereo input into 4 frequency bands (per channel)
    // 2. Process each band:
    //    a. Optional Mid/Side encoding
    //    b. Compress (or bypass)
    //    c. Optional Mid/Side decoding
    //    d. Mute if not soloed (when any band is soloed)
    // 3. Sum bands back to stereo output (phase-coherent)
    //==============================================================================

    float* leftChannel = buffer.getWritePointer(0);
    float* rightChannel = buffer.getWritePointer(1);

    //==============================================================================
    // 1. Split stereo into 4 frequency bands

    // Prepare band pointers for left channel
    for (int band = 0; band < NumBands; ++band)
    {
        bandSplitPtrs[band] = bandBuffers[band].getWritePointer(0);
    }

    // Split left channel
    crossover.processSplit(leftChannel, numSamples, bandSplitPtrs.data());

    // Split right channel
    for (int band = 0; band < NumBands; ++band)
    {
        bandSplitPtrs[band] = bandBuffers[band].getWritePointer(1);
    }

    crossover.processSplit(rightChannel, numSamples, bandSplitPtrs.data());

    //==============================================================================
    // 2. Process each band

    bool anySoloed = isAnyBandSoloed();

    for (int band = 0; band < NumBands; ++band)
    {
        // Check if band should be muted (solo logic)
        bool shouldMute = anySoloed && !bandSolo[band];

        if (shouldMute)
        {
            // Mute this band (another band is soloed)
            bandBuffers[band].clear();
            continue;
        }

        // Process the band
        processBand(band, numSamples);
    }

    //==============================================================================
    // 3. Sum bands back to stereo output

    // Prepare band pointers for summing (left channel)
    for (int band = 0; band < NumBands; ++band)
    {
        bandSumPtrs[band] = bandBuffers[band].getReadPointer(0);
    }

    crossover.processSum(bandSumPtrs.data(), leftChannel, numSamples);

    // Sum bands for right channel
    for (int band = 0; band < NumBands; ++band)
    {
        bandSumPtrs[band] = bandBuffers[band].getReadPointer(1);
    }

    crossover.processSum(bandSumPtrs.data(), rightChannel, numSamples);
}

void MultibandEngine::processBand(int bandIndex, int numSamples)
{
    float* left = bandBuffers[bandIndex].getWritePointer(0);
    float* right = bandBuffers[bandIndex].getWritePointer(1);

    //==============================================================================
    // Per-band processing
    //
    // 1. Optional Mid/Side encoding
    // 2. Compress (or bypass if bypassed)
    // 3. Optional Mid/Side decoding
    //==============================================================================

    // Check if bypassed
    if (bandBypass[bandIndex])
    {
        // Bypass: pass uncompressed audio through
        return;
    }

    //==============================================================================
    // 1. Optional Mid/Side encoding

    if (bandMidSideEnabled[bandIndex])
    {
        // Encode: L/R → Mid/Side
        midSideProcessor.encode(left, right, numSamples);
    }

    //==============================================================================
    // 2. Compress both channels (or Mid + Side if M/S enabled)

    bandCompressors[bandIndex].process(left, numSamples);
    bandCompressors[bandIndex].process(right, numSamples);

    //==============================================================================
    // 3. Optional Mid/Side decoding

    if (bandMidSideEnabled[bandIndex])
    {
        // Decode: Mid/Side → L/R
        midSideProcessor.decode(left, right, numSamples);
    }
}

bool MultibandEngine::isAnyBandSoloed() const
{
    for (int band = 0; band < NumBands; ++band)
    {
        if (bandSolo[band])
            return true;
    }
    return false;
}

float MultibandEngine::getBandGainReductionDb(int bandIndex) const
{
    jassert(bandIndex >= 0 && bandIndex < NumBands);
    return bandCompressors[bandIndex].getGainReductionDb();
}

} // namespace audioforge
