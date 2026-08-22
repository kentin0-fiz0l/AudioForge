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

    // Determine oversampling factor
    int oversamplingIndex = oversamplingParam->getIndex();
    int newFactor = (oversamplingIndex == 0) ? 1 : (oversamplingIndex == 1) ? 2 : 4;

    if (newFactor != currentOversamplingFactor || oversampler == nullptr)
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

            oversampler->initProcessing(static_cast<size_t>(samplesPerBlock));
        }
        else
        {
            oversampler.reset();
        }
    }

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate * currentOversamplingFactor;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock * currentOversamplingFactor);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    // Initialize tone filters
    lowShelf.prepare(spec);
    lowShelf.reset();

    highShelf.prepare(spec);
    highShelf.reset();

    // Initialize DC blocker (at original sample rate)
    juce::dsp::ProcessSpec dcSpec;
    dcSpec.sampleRate = sampleRate;
    dcSpec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
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

    // Update oversampling if changed
    int oversamplingIndex = oversamplingParam->getIndex();
    int newFactor = (oversamplingIndex == 0) ? 1 : (oversamplingIndex == 1) ? 2 : 4;
    if (newFactor != currentOversamplingFactor)
    {
        prepareToPlay(currentSampleRate, buffer.getNumSamples());
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

    // Store dry signal for mixing
    juce::AudioBuffer<float> dryBuffer(totalNumInputChannels, buffer.getNumSamples());
    for (int ch = 0; ch < totalNumInputChannels; ++ch)
        dryBuffer.copyFrom(ch, 0, buffer, ch, 0, buffer.getNumSamples());

    // Apply oversampling if enabled
    juce::AudioBuffer<float>* processingBuffer = &buffer;
    std::unique_ptr<juce::AudioBuffer<float>> oversampledBuffer;

    if (oversampler != nullptr)
    {
        auto oversampledBlock = oversampler->processSamplesUp(juce::dsp::AudioBlock<float>(buffer));
        oversampledBuffer = std::make_unique<juce::AudioBuffer<float>>(
            oversampledBlock.getNumChannels(),
            oversampledBlock.getNumSamples()
        );

        for (size_t ch = 0; ch < oversampledBlock.getNumChannels(); ++ch)
            oversampledBuffer->copyFrom(static_cast<int>(ch), 0,
                                       oversampledBlock.getChannelPointer(ch),
                                       static_cast<int>(oversampledBlock.getNumSamples()));

        processingBuffer = oversampledBuffer.get();
    }

    // Update tone filter coefficients (at oversampled rate)
    const double oversampledSampleRate = currentSampleRate * currentOversamplingFactor;

    *lowShelf.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(
        oversampledSampleRate, 200.0, 0.707f, juce::Decibels::decibelsToGain(toneLowGain));

    *highShelf.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
        oversampledSampleRate, 5000.0, 0.707f, juce::Decibels::decibelsToGain(toneHighGain));

    // Process audio with saturation
    const int numSamples = processingBuffer->getNumSamples();

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = processingBuffer->getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float input = channelData[sample];

            // Apply drive (0-10x multiplier)
            float driven = input * (1.0f + drive * 9.0f);

            // Apply saturation based on selected mode
            float saturated = AudioForge::DSP::SaturationAlgorithms::saturate(driven, mode);

            channelData[sample] = saturated;
        }
    }

    // Apply tone filters
    juce::dsp::AudioBlock<float> block(*processingBuffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    lowShelf.process(context);
    highShelf.process(context);

    // Downsample if we used oversampling
    if (oversampler != nullptr)
    {
        auto oversampledBlock = juce::dsp::AudioBlock<float>(*processingBuffer);
        auto outputBlock = juce::dsp::AudioBlock<float>(buffer);
        oversampler->processSamplesDown(outputBlock);
    }

    // Mix dry/wet
    for (int ch = 0; ch < totalNumInputChannels; ++ch)
    {
        auto* wet = buffer.getWritePointer(ch);
        const auto* dry = dryBuffer.getReadPointer(ch);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            wet[sample] = dry[sample] * (1.0f - mix) + wet[sample] * mix;
    }

    // Apply DC blocker
    juce::dsp::AudioBlock<float> finalBlock(buffer);
    juce::dsp::ProcessContextReplacing<float> dcContext(finalBlock);
    dcBlocker.process(dcContext);

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
