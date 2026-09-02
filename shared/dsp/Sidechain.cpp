/*
  ==============================================================================

    Sidechain.cpp
    Created: 1 Sep 2026
    Author:  AudioForge Team

  ==============================================================================
*/

#include "Sidechain.h"

namespace audioforge
{

Sidechain::Sidechain()
{
}

void Sidechain::prepare(double sampleRate, int maxBlockSize)
{
    currentSampleRate = sampleRate;
    maxSamplesPerBlock = maxBlockSize;

    // Prepare DSP filters
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(maxBlockSize);
    spec.numChannels = 1;  // Process mono (will duplicate for stereo)

    highPassFilter.prepare(spec);
    lowPassFilter.prepare(spec);

    // Set initial filter coefficients
    setHighPassFreq(highPassFreq);
    setLowPassFreq(lowPassFreq);

    reset();
}

void Sidechain::setHighPassFreq(float hz)
{
    highPassFreq = juce::jlimit(20.0f, 2000.0f, hz);

    // Create high-pass filter coefficients (2nd order Butterworth)
    *highPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(
        currentSampleRate,
        highPassFreq,
        0.707f  // Q factor (Butterworth response)
    );
}

void Sidechain::setLowPassFreq(float hz)
{
    lowPassFreq = juce::jlimit(200.0f, 20000.0f, hz);

    // Create low-pass filter coefficients (2nd order Butterworth)
    *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(
        currentSampleRate,
        lowPassFreq,
        0.707f  // Q factor (Butterworth response)
    );
}

void Sidechain::reset()
{
    highPassFilter.reset();
    lowPassFilter.reset();
}

void Sidechain::process(float* samples, int numSamples)
{
    if (!sidechainEnabled)
        return;  // Bypass if sidechain is disabled

    // Create audio block for DSP processing
    juce::dsp::AudioBlock<float> block(&samples, 1, static_cast<size_t>(numSamples));
    juce::dsp::ProcessContextReplacing<float> context(block);

    // Apply high-pass filter
    highPassFilter.process(context);

    // Apply low-pass filter
    lowPassFilter.process(context);
}

void Sidechain::processStereo(float* leftSamples, float* rightSamples, int numSamples)
{
    // Process each channel independently
    process(leftSamples, numSamples);
    process(rightSamples, numSamples);
}

float Sidechain::getLevel(const float* samples, int numSamples)
{
    // Find peak level in filtered sidechain signal
    float peak = 0.0f;
    for (int i = 0; i < numSamples; ++i)
    {
        float absValue = std::abs(samples[i]);
        if (absValue > peak)
            peak = absValue;
    }
    return peak;
}

} // namespace audioforge
