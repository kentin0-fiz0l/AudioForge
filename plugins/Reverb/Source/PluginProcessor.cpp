/**
 * Reverb Audio Processor Implementation
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ReverbAudioProcessor::ReverbAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    // Room Size parameter (0-100%)
    addParameter(roomSizeParam = new juce::AudioParameterFloat(
        "roomSize",
        "Room Size",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        50.0f,
        "%"));

    // Damping parameter (0-100%)
    addParameter(dampingParam = new juce::AudioParameterFloat(
        "damping",
        "Damping",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        50.0f,
        "%"));

    // Width parameter (0-100%)
    addParameter(widthParam = new juce::AudioParameterFloat(
        "width",
        "Width",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        100.0f,
        "%"));

    // Pre-Delay parameter (0-100ms)
    addParameter(preDelayParam = new juce::AudioParameterFloat(
        "preDelay",
        "Pre-Delay",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        0.0f,
        "ms"));

    // Mix parameter (0-100%)
    addParameter(mixParam = new juce::AudioParameterFloat(
        "mix",
        "Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        50.0f,
        "%"));

    // Freeze parameter (boolean)
    addParameter(freezeParam = new juce::AudioParameterBool(
        "freeze",
        "Freeze",
        false));
}

ReverbAudioProcessor::~ReverbAudioProcessor()
{
}

//==============================================================================
void ReverbAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Calculate sample rate scaling
    sampleRateScale = static_cast<float>(sampleRate / 44100.0);

    // Initialize comb filters
    for (int i = 0; i < numCombs; ++i)
    {
        combsLeft[i].setBufferSize(static_cast<int>(combTuningL[i] * sampleRateScale));
        combsRight[i].setBufferSize(static_cast<int>(combTuningR[i] * sampleRateScale));
    }

    // Initialize all-pass filters
    for (int i = 0; i < numAllPasses; ++i)
    {
        allPassesLeft[i].setBufferSize(static_cast<int>(allPassTuning[i] * sampleRateScale));
        allPassesRight[i].setBufferSize(static_cast<int>(allPassTuning[i] * sampleRateScale));
    }

    // Initialize pre-delay buffers
    maxPreDelaySamples = static_cast<int>(sampleRate * 0.1); // 100ms max
    preDelayBufferLeft.resize(maxPreDelaySamples, 0.0f);
    preDelayBufferRight.resize(maxPreDelaySamples, 0.0f);
    preDelayIndex = 0;

    updateReverb();
}

void ReverbAudioProcessor::releaseResources()
{
}

bool ReverbAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void ReverbAudioProcessor::updateReverb()
{
    // Get parameters (0-100 range converted to 0-1)
    float roomSize = roomSizeParam->get() / 100.0f;
    float damping = dampingParam->get() / 100.0f;
    bool freeze = freezeParam->get();

    // Update comb filter parameters
    float feedback = freeze ? 1.0f : (0.28f + roomSize * 0.56f);
    float damp1 = damping * 0.4f;
    float damp2 = 1.0f - damp1;

    for (int i = 0; i < numCombs; ++i)
    {
        combsLeft[i].feedback = feedback;
        combsLeft[i].damp1 = damp1;
        combsLeft[i].damp2 = damp2;

        combsRight[i].feedback = feedback;
        combsRight[i].damp1 = damp1;
        combsRight[i].damp2 = damp2;
    }
}

void ReverbAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    // Clear unused output channels
    for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    if (getTotalNumInputChannels() < 2)
        return;

    // Update reverb parameters
    updateReverb();

    // Get parameters
    float mix = mixParam->get() / 100.0f;
    float width = widthParam->get() / 100.0f;
    float preDelayMs = preDelayParam->get();
    bool freeze = freezeParam->get();
    int preDelaySamples = static_cast<int>((preDelayMs / 1000.0f) * getSampleRate());
    preDelaySamples = juce::jmin(preDelaySamples, maxPreDelaySamples - 1);

    // Process audio
    auto* leftChannel = buffer.getWritePointer(0);
    auto* rightChannel = buffer.getWritePointer(1);

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float inputL = leftChannel[sample];
        float inputR = rightChannel[sample];

        // Update meters
        inputMeter.updateLevel(std::max(std::abs(inputL), std::abs(inputR)));

        // Mute input in freeze mode
        if (freeze)
        {
            inputL = 0.0f;
            inputR = 0.0f;
        }

        // Pre-delay
        float delayedL = inputL;
        float delayedR = inputR;

        if (preDelaySamples > 0)
        {
            int readIndex = (preDelayIndex - preDelaySamples + maxPreDelaySamples) % maxPreDelaySamples;
            delayedL = preDelayBufferLeft[readIndex];
            delayedR = preDelayBufferRight[readIndex];

            preDelayBufferLeft[preDelayIndex] = inputL;
            preDelayBufferRight[preDelayIndex] = inputR;
            preDelayIndex = (preDelayIndex + 1) % maxPreDelaySamples;
        }

        // Mix to mono for reverb input
        float monoInput = (delayedL + delayedR) * 0.5f;

        // Process through comb filters (parallel)
        float combOutL = 0.0f;
        float combOutR = 0.0f;

        for (int i = 0; i < numCombs; ++i)
        {
            combOutL += combsLeft[i].process(monoInput);
            combOutR += combsRight[i].process(monoInput);
        }

        // Process through all-pass filters (series)
        float wetL = combOutL;
        float wetR = combOutR;

        for (int i = 0; i < numAllPasses; ++i)
        {
            wetL = allPassesLeft[i].process(wetL);
            wetR = allPassesRight[i].process(wetR);
        }

        // Apply width (stereo spread)
        float mono = (wetL + wetR) * 0.5f;
        float side = (wetL - wetR) * 0.5f * width;
        wetL = mono + side;
        wetR = mono - side;

        // Mix dry and wet
        float dryL = leftChannel[sample];
        float dryR = rightChannel[sample];
        float outputL = dryL * (1.0f - mix) + wetL * mix;
        float outputR = dryR * (1.0f - mix) + wetR * mix;

        leftChannel[sample] = outputL;
        rightChannel[sample] = outputR;

        // Update output meter
        outputMeter.updateLevel(std::max(std::abs(outputL), std::abs(outputR)));
    }
}

//==============================================================================
juce::AudioProcessorEditor* ReverbAudioProcessor::createEditor()
{
    return new ReverbAudioProcessorEditor(*this);
}

//==============================================================================
void ReverbAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream(destData, true);

    stream.writeFloat(*roomSizeParam);
    stream.writeFloat(*dampingParam);
    stream.writeFloat(*widthParam);
    stream.writeFloat(*preDelayParam);
    stream.writeFloat(*mixParam);
    stream.writeBool(*freezeParam);
}

void ReverbAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);

    roomSizeParam->setValueNotifyingHost(stream.readFloat());
    dampingParam->setValueNotifyingHost(stream.readFloat());
    widthParam->setValueNotifyingHost(stream.readFloat());
    preDelayParam->setValueNotifyingHost(stream.readFloat());
    mixParam->setValueNotifyingHost(stream.readFloat());
    freezeParam->setValueNotifyingHost(stream.readBool() ? 1.0f : 0.0f);
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ReverbAudioProcessor();
}
