/*
  ==============================================================================

    TruePeakDetector.cpp
    Created: 1 Sep 2026
    Author:  AudioForge Team

  ==============================================================================
*/

#include "TruePeakDetector.h"

namespace audioforge
{

TruePeakDetector::TruePeakDetector()
{
    // Create 4x oversampler with 2nd order filter
    // This provides sufficient quality for true peak detection
    // while maintaining low CPU usage
    oversampler = std::make_unique<juce::dsp::Oversampling<float>>(
        1,  // Single channel (we'll process L/R separately)
        2,  // 2nd order filter (96 dB/octave)
        juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR,
        true  // Use steep filter for better alias rejection
    );
}

void TruePeakDetector::prepare(double sampleRate, int maxBlockSize)
{
    currentSampleRate = sampleRate;
    maxSamplesPerBlock = maxBlockSize;

    // Prepare oversampler
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(maxBlockSize);
    spec.numChannels = 1;

    oversampler->initProcessing(maxBlockSize);
    oversampler->reset();

    // Allocate oversampled buffer (4x the input size)
    int oversampledSize = maxBlockSize * 4;
    oversampledBuffer.setSize(1, oversampledSize, false, true, false);
}

float TruePeakDetector::detectTruePeak(const float* samples, int numSamples)
{
    // Copy input samples to buffer
    oversampledBuffer.clear();
    auto* channelData = oversampledBuffer.getWritePointer(0);

    for (int i = 0; i < numSamples; ++i)
        channelData[i] = samples[i];

    // Create audio block for processing
    juce::dsp::AudioBlock<float> block(oversampledBuffer);
    auto subBlock = block.getSubBlock(0, static_cast<size_t>(numSamples));

    // Upsample to 4x sample rate
    auto oversampledBlock = oversampler->processSamplesUp(subBlock);

    // Find absolute maximum in oversampled signal
    float truePeak = 0.0f;
    auto* oversampledData = oversampledBlock.getChannelPointer(0);
    size_t oversampledSamples = oversampledBlock.getNumSamples();

    for (size_t i = 0; i < oversampledSamples; ++i)
    {
        float absSample = std::abs(oversampledData[i]);
        if (absSample > truePeak)
            truePeak = absSample;
    }

    return truePeak;
}

float TruePeakDetector::detectTruePeakStereo(const float* leftSamples,
                                               const float* rightSamples,
                                               int numSamples)
{
    // Detect true peak for left channel
    float leftPeak = detectTruePeak(leftSamples, numSamples);

    // Detect true peak for right channel
    float rightPeak = detectTruePeak(rightSamples, numSamples);

    // Return maximum of both channels
    return std::max(leftPeak, rightPeak);
}

void TruePeakDetector::reset()
{
    oversampler->reset();
    oversampledBuffer.clear();
}

} // namespace audioforge
