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

    // Pitch Shift (-24 to +24 semitones)
    addParameter(pitchShiftParam = new juce::AudioParameterFloat(
        PARAM_PITCH_SHIFT,
        "Pitch Shift",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f),
        0.0f));  // Default: no shift

    // Spray Amount (0-100%)
    addParameter(sprayParam = new juce::AudioParameterFloat(
        PARAM_SPRAY,
        "Spray",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        0.0f));  // Default: no spray

    // Reverse Probability (0-100%)
    addParameter(reverseParam = new juce::AudioParameterFloat(
        PARAM_REVERSE,
        "Reverse",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        0.0f));  // Default: no reverse

    // Stereo Width (0-200%)
    addParameter(stereoWidthParam = new juce::AudioParameterFloat(
        PARAM_STEREO_WIDTH,
        "Stereo Width",
        juce::NormalisableRange<float>(0.0f, 200.0f, 1.0f),
        100.0f));  // Default: 100% (normal stereo)

    // Dry/Wet Mix
    addParameter(dryWetParam = new juce::AudioParameterFloat(
        PARAM_DRY_WET,
        "Dry/Wet",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        100.0f));  // Default: 100% wet

    // Window Type (0=Hann, 1=Gaussian, 2=Triangle, 3=Tukey)
    addParameter(windowTypeParam = new juce::AudioParameterInt(
        PARAM_WINDOW_TYPE,
        "Window Type",
        0, 3,
        0));  // Default: Hann

    // Window Shape (affects Gaussian width and Tukey taper)
    addParameter(windowShapeParam = new juce::AudioParameterFloat(
        PARAM_WINDOW_SHAPE,
        "Window Shape",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f));  // Default: 0.5
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
    grainExtractor.setWindowType(windowTypeParam->get());
    grainExtractor.setWindowShape(windowShapeParam->get());

    // Prepare grain scheduler
    grainScheduler.prepare(sampleRate, grainSizeSamples);
    grainScheduler.setGrainDensity(grainDensityParam->get());
    grainScheduler.setTimeStretch(timeStretchParam->get());
    grainScheduler.setReadPosition(positionParam->get());

    // Allocate output buffers (stereo)
    granularLeftOutput.resize(samplesPerBlock);
    granularRightOutput.resize(samplesPerBlock);
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

    if (windowTypeParam->get() != grainExtractor.getWindowType())
    {
        grainExtractor.setWindowType(windowTypeParam->get());
    }

    if (windowShapeParam->get() != grainExtractor.getWindowShape())
    {
        grainExtractor.setWindowShape(windowShapeParam->get());
    }

    grainScheduler.setGrainDensity(grainDensityParam->get());
    grainScheduler.setTimeStretch(timeStretchParam->get());
    grainScheduler.setReadPosition(positionParam->get());
    grainScheduler.setPitchShift(pitchShiftParam->get());
    grainScheduler.setSprayAmount(sprayParam->get() / 100.0f);
    grainScheduler.setReverseProbability(reverseParam->get() / 100.0f);
    grainScheduler.setStereoWidth(stereoWidthParam->get() / 100.0f);

    float dryWet = dryWetParam->get() / 100.0f;

    // Write input to grain buffer
    for (int ch = 0; ch < numChannels; ++ch)
    {
        const float* input = buffer.getReadPointer(ch);
        grainBuffer.writeBlock(input, numSamples);
    }

    // Process granular synthesis (stereo output)
    grainScheduler.processBlock(grainBuffer, grainExtractor,
                                 granularLeftOutput.data(),
                                 granularRightOutput.data(),
                                 numSamples);

    // Mix dry and wet signals
    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* channelData = buffer.getWritePointer(ch);
        const float* wetData = (ch == 0) ? granularLeftOutput.data() : granularRightOutput.data();

        for (int i = 0; i < numSamples; ++i)
        {
            float dry = channelData[i];
            float wet = wetData[i];
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
    stream.writeFloat(pitchShiftParam->get());
    stream.writeFloat(sprayParam->get());
    stream.writeFloat(reverseParam->get());
    stream.writeFloat(stereoWidthParam->get());
    stream.writeFloat(dryWetParam->get());
    stream.writeInt(windowTypeParam->get());
    stream.writeFloat(windowShapeParam->get());
}

void GranularEngineProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Restore plugin state
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
    grainSizeParam->setValueNotifyingHost(grainSizeParam->convertTo0to1(stream.readFloat()));
    grainDensityParam->setValueNotifyingHost(grainDensityParam->convertTo0to1(stream.readFloat()));
    timeStretchParam->setValueNotifyingHost(timeStretchParam->convertTo0to1(stream.readFloat()));
    positionParam->setValueNotifyingHost(positionParam->convertTo0to1(stream.readFloat()));
    pitchShiftParam->setValueNotifyingHost(pitchShiftParam->convertTo0to1(stream.readFloat()));
    sprayParam->setValueNotifyingHost(sprayParam->convertTo0to1(stream.readFloat()));
    reverseParam->setValueNotifyingHost(reverseParam->convertTo0to1(stream.readFloat()));
    stereoWidthParam->setValueNotifyingHost(stereoWidthParam->convertTo0to1(stream.readFloat()));
    dryWetParam->setValueNotifyingHost(dryWetParam->convertTo0to1(stream.readFloat()));
    windowTypeParam->setValueNotifyingHost(windowTypeParam->convertTo0to1(stream.readInt()));
    windowShapeParam->setValueNotifyingHost(windowShapeParam->convertTo0to1(stream.readFloat()));
}

//==============================================================================
// Plugin factory
#ifndef AUDIOFORGE_TESTS
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GranularEngineProcessor();
}
#endif
