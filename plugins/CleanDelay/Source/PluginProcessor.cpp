#include "PluginProcessor.h"
#include "PluginEditor.h"

CleanDelayProcessor::CleanDelayProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    // Delay Time parameter (1ms to 2000ms)
    addParameter(delayTimeParam = new juce::AudioParameterFloat(
        PARAM_DELAY_TIME,
        "Delay Time",
        juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f, 0.3f), // Skew toward shorter delays
        0.25f,  // Default: 250ms
        "s"));

    // Feedback parameter (0% to 99%)
    addParameter(feedbackParam = new juce::AudioParameterFloat(
        PARAM_FEEDBACK,
        "Feedback",
        juce::NormalisableRange<float>(0.0f, 0.99f, 0.01f),
        0.3f,   // Default: 30%
        ""));

    // Mix parameter (0% to 100% wet)
    addParameter(mixParam = new juce::AudioParameterFloat(
        PARAM_MIX,
        "Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f,   // Default: 50% wet
        ""));

    // Ping-Pong mode toggle
    addParameter(pingPongParam = new juce::AudioParameterBool(
        PARAM_PINGPONG,
        "Ping-Pong",
        false)); // Default: off

    // Initialize delay buffers to zero
    delayBufferLeft.fill(0.0f);
    delayBufferRight.fill(0.0f);
}

CleanDelayProcessor::~CleanDelayProcessor()
{
}

void CleanDelayProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Inform base class of sample rate and buffer size
    setRateAndBufferSizeDetails(sampleRate, samplesPerBlock);

    // Store sample rate for delay calculations
    currentSampleRate = sampleRate;

    // Clear delay buffers
    delayBufferLeft.fill(0.0f);
    delayBufferRight.fill(0.0f);
    writePosition = 0;
}

void CleanDelayProcessor::releaseResources()
{
}

void CleanDelayProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Get parameters
    float delayTime = delayTimeParam->get();
    float feedback = feedbackParam->get();
    float mix = mixParam->get();
    bool pingPong = pingPongParam->get();

    // Calculate delay in samples
    int delaySamples = static_cast<int>(delayTime * currentSampleRate);
    delaySamples = juce::jmin(delaySamples, MAX_DELAY_SAMPLES - 1);

    // Measure input level
    inputLevelMeter.updateLevel(buffer.getMagnitude(0, numSamples));

    // Process each channel
    for (int channel = 0; channel < numChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        float* delayBuffer = (channel == 0) ? delayBufferLeft.data() : delayBufferRight.data();
        float* otherDelayBuffer = (channel == 0) ? delayBufferRight.data() : delayBufferLeft.data();

        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Read delayed sample
            int readPosition = writePosition - delaySamples;
            if (readPosition < 0)
                readPosition += MAX_DELAY_SAMPLES;

            float delayedSample = delayBuffer[readPosition];

            // In ping-pong mode, alternate channels get feedback from opposite channel
            float feedbackSource = delayedSample;
            if (pingPong && channel == 1)
                feedbackSource = otherDelayBuffer[readPosition];

            // Write to delay buffer (input + feedback)
            delayBuffer[writePosition] = channelData[sample] + (feedbackSource * feedback);

            // Mix dry and wet signals
            float wet = delayedSample;
            float dry = channelData[sample];
            channelData[sample] = dry * (1.0f - mix) + wet * mix;

            // Advance write position
            writePosition = (writePosition + 1) % MAX_DELAY_SAMPLES;
        }

        // Reset write position for second channel
        if (channel == 0 && numChannels > 1)
            writePosition = (writePosition - numSamples + MAX_DELAY_SAMPLES) % MAX_DELAY_SAMPLES;
    }

    // Measure output level
    outputLevelMeter.updateLevel(buffer.getMagnitude(0, numSamples));
}

juce::AudioProcessorEditor* CleanDelayProcessor::createEditor()
{
    return new CleanDelayEditor(*this);
}

void CleanDelayProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Save plugin state
    juce::MemoryOutputStream stream(destData, true);

    stream.writeFloat(delayTimeParam->get());
    stream.writeFloat(feedbackParam->get());
    stream.writeFloat(mixParam->get());
    stream.writeBool(pingPongParam->get());
}

void CleanDelayProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Restore plugin state
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);

    delayTimeParam->setValueNotifyingHost(delayTimeParam->convertTo0to1(stream.readFloat()));
    feedbackParam->setValueNotifyingHost(feedbackParam->convertTo0to1(stream.readFloat()));
    mixParam->setValueNotifyingHost(mixParam->convertTo0to1(stream.readFloat()));
    pingPongParam->setValueNotifyingHost(stream.readBool() ? 1.0f : 0.0f);
}

//==============================================================================
// Plugin factory
#ifndef AUDIOFORGE_TESTS
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CleanDelayProcessor();
}
#endif
