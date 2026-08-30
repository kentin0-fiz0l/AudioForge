#include "PluginProcessor.h"
#include "PluginEditor.h"

GranularEngineProcessor::GranularEngineProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    // Grain Size (10-500ms)
    addParameter(grainSizeParam = new juce::AudioParameterFloat(
        PARAM_GRAIN_SIZE,
        "Grain Size",
        juce::NormalisableRange<float>(10.0f, 500.0f, 1.0f),
        50.0f));  // Default: 50ms

    // Dry/Wet Mix
    addParameter(dryWetParam = new juce::AudioParameterFloat(
        PARAM_DRY_WET,
        "Dry/Wet",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        100.0f));  // Default: 100% wet
}

GranularEngineProcessor::~GranularEngineProcessor()
{
}

void GranularEngineProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Prepare grain buffer (10 seconds max)
    grainBuffer.prepare(sampleRate, (int)(sampleRate * 10.0));

    // Prepare grain extractor
    grainExtractor.prepare(sampleRate);

    // Set initial grain size
    int grainSizeSamples = (int)((grainSizeParam->get() / 1000.0f) * sampleRate);
    grainExtractor.setGrainSize(grainSizeSamples);

    // Allocate temp grain buffer
    grainTemp.resize(grainSizeSamples);
}

void GranularEngineProcessor::releaseResources()
{
    grainBuffer.reset();
}

bool GranularEngineProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // Supports mono and stereo
    return layouts.getMainOutputChannelSet() == layouts.getMainInputChannelSet()
        && !layouts.getMainInputChannelSet().isDisabled();
}

void GranularEngineProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Phase 1: Simple pass-through with grain extraction test
    // Write input to grain buffer
    for (int ch = 0; ch < numChannels; ++ch)
    {
        const float* input = buffer.getReadPointer(ch);

        // Write input samples to buffer
        grainBuffer.writeBlock(input, numSamples);
    }

    // For Phase 1, just pass through the input
    // (In Phase 2, we'll start extracting and playing grains)

    // Test: Extract a grain from 1 second ago and verify windowing works
    // This is just for verification - output is still pass-through
    int grainSizeSamples = grainExtractor.getGrainSize();
    int readPosition = grainBuffer.getWritePosition() - (int)getSampleRate();  // 1 second ago

    if (grainBuffer.getSamplesAvailable() >= grainSizeSamples)
    {
        grainExtractor.extractGrain(grainBuffer, readPosition, grainTemp.data());
        // Grain is extracted and windowed in grainTemp, but we don't play it yet
    }
}

juce::AudioProcessorEditor* GranularEngineProcessor::createEditor()
{
    return new GranularEngineEditor(*this);
}

void GranularEngineProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Save plugin state
    juce::MemoryOutputStream stream(destData, true);
    stream.writeFloat(grainSizeParam->get());
    stream.writeFloat(dryWetParam->get());
}

void GranularEngineProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Restore plugin state
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
    grainSizeParam->setValueNotifyingHost(grainSizeParam->convertTo0to1(stream.readFloat()));
    dryWetParam->setValueNotifyingHost(dryWetParam->convertTo0to1(stream.readFloat()));
}

//==============================================================================
// Plugin factory
#ifndef AUDIOFORGE_TESTS
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GranularEngineProcessor();
}
#endif
