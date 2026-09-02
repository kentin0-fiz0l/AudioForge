/*
  ==============================================================================

    LinkwitzRiley.cpp
    Created: 1 Sep 2026
    Author:  AudioForge Team

  ==============================================================================
*/

#include "LinkwitzRiley.h"

namespace audioforge
{

LinkwitzRileyCrossover::LinkwitzRileyCrossover()
{
}

void LinkwitzRileyCrossover::prepare(double sampleRate, int maxBlockSize)
{
    currentSampleRate = sampleRate;
    maxSamplesPerBlock = maxBlockSize;

    // Prepare all filters
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(maxBlockSize);
    spec.numChannels = 1;  // Process mono (will call per channel)

    for (int i = 0; i < NumCrossovers; ++i)
    {
        crossovers[i].lowPass1.prepare(spec);
        crossovers[i].lowPass2.prepare(spec);
        crossovers[i].highPass1.prepare(spec);
        crossovers[i].highPass2.prepare(spec);

        // Set initial coefficients
        updateCrossoverCoefficients(i);
    }

    // Allocate temporary buffers
    tempBuffer1.setSize(1, maxBlockSize);
    tempBuffer2.setSize(1, maxBlockSize);

    reset();
}

void LinkwitzRileyCrossover::setCrossoverFrequency(int index, float frequency)
{
    if (index < 0 || index >= NumCrossovers)
        return;

    // Clamp to valid ranges with some overlap margin
    if (index == 0)
        frequency = juce::jlimit(20.0f, 500.0f, frequency);
    else if (index == 1)
        frequency = juce::jlimit(200.0f, 5000.0f, frequency);
    else if (index == 2)
        frequency = juce::jlimit(2000.0f, 16000.0f, frequency);

    // Ensure crossovers don't overlap
    if (index > 0 && frequency < crossoverFreqs[index - 1] * 1.5f)
        frequency = crossoverFreqs[index - 1] * 1.5f;
    if (index < NumCrossovers - 1 && frequency > crossoverFreqs[index + 1] * 0.67f)
        frequency = crossoverFreqs[index + 1] * 0.67f;

    crossoverFreqs[index] = frequency;
    updateCrossoverCoefficients(index);
}

float LinkwitzRileyCrossover::getCrossoverFrequency(int index) const
{
    if (index < 0 || index >= NumCrossovers)
        return 0.0f;
    return crossoverFreqs[index];
}

void LinkwitzRileyCrossover::reset()
{
    for (int i = 0; i < NumCrossovers; ++i)
    {
        crossovers[i].lowPass1.reset();
        crossovers[i].lowPass2.reset();
        crossovers[i].highPass1.reset();
        crossovers[i].highPass2.reset();
    }

    tempBuffer1.clear();
    tempBuffer2.clear();
}

void LinkwitzRileyCrossover::updateCrossoverCoefficients(int index)
{
    if (index < 0 || index >= NumCrossovers)
        return;

    float freq = crossoverFreqs[index];

    // Create Butterworth 2nd-order coefficients
    // Q = 0.5 for Butterworth (maximally flat response)
    const float Q = 0.5f;

    // Low-pass coefficients (2 stages for LR4)
    auto lpCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(
        currentSampleRate, freq, Q);

    *crossovers[index].lowPass1.coefficients = *lpCoeffs;
    *crossovers[index].lowPass2.coefficients = *lpCoeffs;

    // High-pass coefficients (2 stages for LR4)
    auto hpCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(
        currentSampleRate, freq, Q);

    *crossovers[index].highPass1.coefficients = *hpCoeffs;
    *crossovers[index].highPass2.coefficients = *hpCoeffs;
}

void LinkwitzRileyCrossover::processSplit(const float* input, int numSamples,
                                          float** bandOutputs)
{
    //==============================================================================
    // Linkwitz-Riley 4-band splitting algorithm
    //
    // Input signal is split into 4 bands using 3 crossover points:
    //
    //                      Input
    //                        ↓
    //              [Crossover 0: 250 Hz]
    //                ↓              ↓
    //          Band 0 (Low)    [Crossover 1: 2 kHz]
    //          20-250 Hz         ↓              ↓
    //                      Band 1 (Low-Mid) [Crossover 2: 8 kHz]
    //                      250 Hz - 2 kHz     ↓            ↓
    //                                    Band 2 (High-Mid) Band 3 (High)
    //                                    2 kHz - 8 kHz     8 kHz - 20 kHz
    //
    //==============================================================================

    float* temp1 = tempBuffer1.getWritePointer(0);
    float* temp2 = tempBuffer2.getWritePointer(0);

    //==============================================================================
    // Process Crossover 0 (Low / Low-Mid split at 250 Hz)

    // Copy input to temp buffer for processing
    juce::FloatVectorOperations::copy(temp1, input, numSamples);

    // Create processing context
    juce::dsp::AudioBlock<float> block1(&temp1, 1, static_cast<size_t>(numSamples));
    juce::dsp::ProcessContextReplacing<float> context1(block1);

    // Low-pass side: Band 0 (Low)
    crossovers[0].lowPass1.process(context1);
    crossovers[0].lowPass2.process(context1);
    juce::FloatVectorOperations::copy(bandOutputs[0], temp1, numSamples);

    // High-pass side: feeds into next crossover
    juce::FloatVectorOperations::copy(temp2, input, numSamples);
    juce::dsp::AudioBlock<float> block2(&temp2, 1, static_cast<size_t>(numSamples));
    juce::dsp::ProcessContextReplacing<float> context2(block2);

    crossovers[0].highPass1.process(context2);
    crossovers[0].highPass2.process(context2);

    //==============================================================================
    // Process Crossover 1 (Low-Mid / High-Mid split at 2 kHz)

    // Low-pass side: Band 1 (Low-Mid)
    juce::FloatVectorOperations::copy(temp1, temp2, numSamples);
    juce::dsp::AudioBlock<float> block3(&temp1, 1, static_cast<size_t>(numSamples));
    juce::dsp::ProcessContextReplacing<float> context3(block3);

    crossovers[1].lowPass1.process(context3);
    crossovers[1].lowPass2.process(context3);
    juce::FloatVectorOperations::copy(bandOutputs[1], temp1, numSamples);

    // High-pass side: feeds into next crossover
    juce::dsp::AudioBlock<float> block4(&temp2, 1, static_cast<size_t>(numSamples));
    juce::dsp::ProcessContextReplacing<float> context4(block4);

    crossovers[1].highPass1.process(context4);
    crossovers[1].highPass2.process(context4);

    //==============================================================================
    // Process Crossover 2 (High-Mid / High split at 8 kHz)

    // Low-pass side: Band 2 (High-Mid)
    juce::FloatVectorOperations::copy(temp1, temp2, numSamples);
    juce::dsp::AudioBlock<float> block5(&temp1, 1, static_cast<size_t>(numSamples));
    juce::dsp::ProcessContextReplacing<float> context5(block5);

    crossovers[2].lowPass1.process(context5);
    crossovers[2].lowPass2.process(context5);
    juce::FloatVectorOperations::copy(bandOutputs[2], temp1, numSamples);

    // High-pass side: Band 3 (High)
    juce::dsp::AudioBlock<float> block6(&temp2, 1, static_cast<size_t>(numSamples));
    juce::dsp::ProcessContextReplacing<float> context6(block6);

    crossovers[2].highPass1.process(context6);
    crossovers[2].highPass2.process(context6);
    juce::FloatVectorOperations::copy(bandOutputs[3], temp2, numSamples);
}

void LinkwitzRileyCrossover::processSum(const float** bandInputs, float* output,
                                        int numSamples)
{
    //==============================================================================
    // Phase-coherent summing
    //
    // Because Linkwitz-Riley crossovers are phase-coherent, we can simply
    // sum all bands together to reconstruct the original signal perfectly.
    //
    //==============================================================================

    // Clear output buffer
    juce::FloatVectorOperations::clear(output, numSamples);

    // Sum all 4 bands
    for (int band = 0; band < NumBands; ++band)
    {
        juce::FloatVectorOperations::add(output, bandInputs[band], numSamples);
    }
}

} // namespace audioforge
