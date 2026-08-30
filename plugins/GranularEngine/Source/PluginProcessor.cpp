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

    // Grain Density (1-100 grains/second)
    addParameter(grainDensityParam = new juce::AudioParameterFloat(
        PARAM_GRAIN_DENSITY,
        "Grain Density",
        juce::NormalisableRange<float>(1.0f, 100.0f, 0.1f),
        10.0f));  // Default: 10 grains/sec

    // Time Stretch (0.25x - 4x)
    addParameter(timeStretchParam = new juce::AudioParameterFloat(
        PARAM_TIME_STRETCH,
        "Time Stretch",
        juce::NormalisableRange<float>(0.25f, 4.0f, 0.01f, 0.5f),
        1.0f));  // Default: 1x (normal speed)

    // Position (0-1: where in buffer to read)
    addParameter(positionParam = new juce::AudioParameterFloat(
        PARAM_POSITION,
        "Position",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f));  // Default: middle of buffer

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

    // Prepare grain scheduler
    grainScheduler.prepare(sampleRate, grainSizeSamples);
    grainScheduler.setGrainDensity(grainDensityParam->get());
    grainScheduler.setTimeStretch(timeStretchParam->get());
    grainScheduler.setReadPosition(positionParam->get());

    // Allocate output buffer
    granularOutput.resize(samplesPerBlock);
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

    // Update parameters
    int grainSizeSamples = (int)((grainSizeParam->get() / 1000.0f) * getSampleRate());
    if (grainSizeSamples != grainExtractor.getGrainSize())
    {
        grainExtractor.setGrainSize(grainSizeSamples);
    }

    grainScheduler.setGrainDensity(grainDensityParam->get());
    grainScheduler.setTimeStretch(timeStretchParam->get());
    grainScheduler.setReadPosition(positionParam->get());

    float dryWet = dryWetParam->get() / 100.0f;

    // Write input to grain buffer
    for (int ch = 0; ch < numChannels; ++ch)
    {
        const float* input = buffer.getReadPointer(ch);
        grainBuffer.writeBlock(input, numSamples);
    }

    // Process granular synthesis
    grainScheduler.processBlock(grainBuffer, grainExtractor,
                                 granularOutput.data(), numSamples);

    // Mix dry and wet signals
    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* channelData = buffer.getWritePointer(ch);

        for (int i = 0; i < numSamples; ++i)
        {
            float dry = channelData[i];
            float wet = granularOutput[i];
            channelData[i] = dry * (1.0f - dryWet) + wet * dryWet;
        }
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
    stream.writeFloat(grainDensityParam->get());
    stream.writeFloat(timeStretchParam->get());
    stream.writeFloat(positionParam->get());
    stream.writeFloat(dryWetParam->get());
}

void GranularEngineProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Restore plugin state
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
    grainSizeParam->setValueNotifyingHost(grainSizeParam->convertTo0to1(stream.readFloat()));
    grainDensityParam->setValueNotifyingHost(grainDensityParam->convertTo0to1(stream.readFloat()));
    timeStretchParam->setValueNotifyingHost(timeStretchParam->convertTo0to1(stream.readFloat()));
    positionParam->setValueNotifyingHost(positionParam->convertTo0to1(stream.readFloat()));
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
