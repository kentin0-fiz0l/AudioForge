#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "dsp/SaturationAlgorithms.h"
#include <cmath>

SaturationAudioProcessor::SaturationAudioProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    // Drive parameter (0-100%)
    addParameter(driveParam = new juce::AudioParameterFloat(
        "drive",
        "Drive",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        0.0f,
        "%"));

    // Mode parameter (5 saturation types)
    addParameter(modeParam = new juce::AudioParameterChoice(
        "mode",
        "Mode",
        juce::StringArray{"Tape", "Tube", "Transistor", "Transformer", "Diode"},
        0));

    // Tone Low parameter (low shelf EQ, -12 to +12 dB)
    addParameter(toneLowParam = new juce::AudioParameterFloat(
        "toneLow",
        "Tone Low",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f,
        "dB"));

    // Tone High parameter (high shelf EQ, -12 to +12 dB)
    addParameter(toneHighParam = new juce::AudioParameterFloat(
        "toneHigh",
        "Tone High",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f,
        "dB"));

    // Mix parameter (dry/wet, 0-100%)
    addParameter(mixParam = new juce::AudioParameterFloat(
        "mix",
        "Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        100.0f,
        "%"));

    // Oversampling parameter
    addParameter(oversamplingParam = new juce::AudioParameterChoice(
        "oversampling",
        "Oversampling",
        juce::StringArray{"Off", "2x", "4x"},
        0));
}

SaturationAudioProcessor::~SaturationAudioProcessor()
{
}

const juce::String SaturationAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SaturationAudioProcessor::acceptsMidi() const { return false; }
bool SaturationAudioProcessor::producesMidi() const { return false; }
bool SaturationAudioProcessor::isMidiEffect() const { return false; }
double SaturationAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int SaturationAudioProcessor::getNumPrograms() { return 1; }
int SaturationAudioProcessor::getCurrentProgram() { return 0; }
void SaturationAudioProcessor::setCurrentProgram (int) {}
const juce::String SaturationAudioProcessor::getProgramName (int) { return {}; }
void SaturationAudioProcessor::changeProgramName (int, const juce::String&) {}

void SaturationAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    preparedBlockSize = juce::jmax(1, samplesPerBlock);

    // Determine oversampling factor
    int oversamplingIndex = oversamplingParam->getIndex();
    int newFactor = (oversamplingIndex == 0) ? 1 : (oversamplingIndex == 1) ? 2 : 4;

    if (newFactor != currentOversamplingFactor || (newFactor > 1 && oversampler == nullptr))
    {
        currentOversamplingFactor = newFactor;

        if (currentOversamplingFactor > 1)
        {
            // Create oversampler (order 5 = 48 dB/octave filter)
            oversampler = std::make_unique<juce::dsp::Oversampling<float>>(
                getTotalNumOutputChannels(),
                static_cast<size_t>(std::log2(currentOversamplingFactor)),
                juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR,
                true,  // Use steep filter
                false  // Don't normalize gain
            );
        }
        else
        {
            oversampler.reset();
        }
    }

    // Size the oversampler for this block size on every prepare: a host may
    // prepare again with a bigger block and the same factor
    if (oversampler != nullptr)
    {
        oversampler->initProcessing(static_cast<size_t>(preparedBlockSize));
        oversampler->reset();
        setLatencySamples(juce::roundToInt(oversampler->getLatencyInSamples()));
    }
    else
    {
        setLatencySamples(0);
    }

    dryBuffer.setSize(getTotalNumOutputChannels(), preparedBlockSize, false, false, true);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate * currentOversamplingFactor;
    spec.maximumBlockSize = static_cast<juce::uint32>(preparedBlockSize * currentOversamplingFactor);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    // Initialize tone filters
    lowShelf.prepare(spec);
    lowShelf.reset();

    highShelf.prepare(spec);
    highShelf.reset();

    // Initialize DC blocker (at original sample rate)
    juce::dsp::ProcessSpec dcSpec;
    dcSpec.sampleRate = sampleRate;
    dcSpec.maximumBlockSize = static_cast<juce::uint32>(preparedBlockSize);
    dcSpec.numChannels = spec.numChannels;

    dcBlocker.prepare(dcSpec);
    *dcBlocker.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 20.0);

    // Clear meters
    inputMeter.clear();
    outputMeter.clear();
}

void SaturationAudioProcessor::releaseResources()
{
}

bool SaturationAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void SaturationAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Get parameters
    const float drive = driveParam->get() / 100.0f;  // 0-1
    const float mix = mixParam->get() / 100.0f;      // 0-1
    const int mode = modeParam->getIndex();
    const float toneLowGain = toneLowParam->get();
    const float toneHighGain = toneHighParam->get();

    // Update oversampling if changed (or if the host never prepared us)
    int oversamplingIndex = oversamplingParam->getIndex();
    int newFactor = (oversamplingIndex == 0) ? 1 : (oversamplingIndex == 1) ? 2 : 4;
    if (newFactor != currentOversamplingFactor || preparedBlockSize == 0)
    {
        prepareToPlay(currentSampleRate,
                      preparedBlockSize > 0 ? preparedBlockSize : buffer.getNumSamples());
    }

    // Calculate input peak level
    float inputPeak = 0.0f;
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        const float* channelData = buffer.getReadPointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            inputPeak = std::max(inputPeak, std::abs(channelData[sample]));
    }
    inputMeter.updateLevel(inputPeak);

    // Update tone filter coefficients (at oversampled rate)
    const double oversampledSampleRate = currentSampleRate * currentOversamplingFactor;

    *lowShelf.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(
        oversampledSampleRate, 200.0, 0.707f, juce::Decibels::decibelsToGain(toneLowGain));

    *highShelf.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
        oversampledSampleRate, 5000.0, 0.707f, juce::Decibels::decibelsToGain(toneHighGain));

    // The oversampler and dry buffer are sized for the prepared block size,
    // so a bigger block from the host is processed in pieces of that size
    const int numChannels = juce::jmin(totalNumInputChannels, dryBuffer.getNumChannels());

    for (int start = 0; start < buffer.getNumSamples(); start += preparedBlockSize)
    {
        const int numSamples = juce::jmin(preparedBlockSize, buffer.getNumSamples() - start);
        processChunk(buffer, start, numSamples, numChannels, drive, mix, mode);
    }

    // Calculate output peak level
    float outputPeak = 0.0f;
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        const float* channelData = buffer.getReadPointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            outputPeak = std::max(outputPeak, std::abs(channelData[sample]));
    }
    outputMeter.updateLevel(outputPeak);
}

void SaturationAudioProcessor::processChunk (juce::AudioBuffer<float>& buffer, int startSample,
                                             int numSamples, int numChannels,
                                             float drive, float mix, int mode)
{
    // Store dry signal for mixing
    for (int ch = 0; ch < numChannels; ++ch)
        dryBuffer.copyFrom(ch, 0, buffer, ch, startSample, numSamples);

    auto block = juce::dsp::AudioBlock<float>(buffer)
                     .getSubsetChannelBlock(0, static_cast<size_t>(numChannels))
                     .getSubBlock(static_cast<size_t>(startSample), static_cast<size_t>(numSamples));

    // Saturate and shape the oversampled block in place, so that the
    // downsampler reads the processed signal back
    auto processingBlock = (oversampler != nullptr) ? oversampler->processSamplesUp(block) : block;

    for (size_t channel = 0; channel < processingBlock.getNumChannels(); ++channel)
    {
        auto* channelData = processingBlock.getChannelPointer(channel);

        for (size_t sample = 0; sample < processingBlock.getNumSamples(); ++sample)
        {
            // Apply drive (0-10x multiplier)
            float driven = channelData[sample] * (1.0f + drive * 9.0f);

            // Apply saturation based on selected mode
            channelData[sample] = AudioForge::DSP::SaturationAlgorithms::saturate(driven, mode);
        }
    }

    // Apply tone filters
    juce::dsp::ProcessContextReplacing<float> context(processingBlock);
    lowShelf.process(context);
    highShelf.process(context);

    // Downsample if we used oversampling
    if (oversampler != nullptr)
        oversampler->processSamplesDown(block);

    // Mix dry/wet
    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* wet = buffer.getWritePointer(ch, startSample);
        const auto* dry = dryBuffer.getReadPointer(ch);

        for (int sample = 0; sample < numSamples; ++sample)
            wet[sample] = dry[sample] * (1.0f - mix) + wet[sample] * mix;
    }

    // Apply DC blocker
    juce::dsp::ProcessContextReplacing<float> dcContext(block);
    dcBlocker.process(dcContext);
}

bool SaturationAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* SaturationAudioProcessor::createEditor()
{
    return new SaturationAudioProcessorEditor (*this);
}

void SaturationAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream(destData, true);
    stream.writeFloat(*driveParam);
    stream.writeInt(modeParam->getIndex());
    stream.writeFloat(*toneLowParam);
    stream.writeFloat(*toneHighParam);
    stream.writeFloat(*mixParam);
    stream.writeInt(oversamplingParam->getIndex());
}

void SaturationAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
    *driveParam = stream.readFloat();
    *modeParam = stream.readInt();
    *toneLowParam = stream.readFloat();
    *toneHighParam = stream.readFloat();
    *mixParam = stream.readFloat();
    *oversamplingParam = stream.readInt();
}

#ifndef AUDIOFORGE_TESTS
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SaturationAudioProcessor();
}
#endif
