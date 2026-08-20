#include "PluginProcessor.h"
#include "PluginEditor.h"

SimpleGainProcessor::SimpleGainProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    // Add gain parameter (-60 to +12 dB)
    addParameter(gainParam = new juce::AudioParameterFloat(
        "gain",                              // Parameter ID
        "Gain",                              // Parameter name
        juce::NormalisableRange<float>(-60.0f, 12.0f, 0.1f), // Range
        0.0f,                                // Default value (0 dB = unity gain)
        "dB"));                              // Unit label
}

SimpleGainProcessor::~SimpleGainProcessor()
{
}

void SimpleGainProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Initialize smoothed gain with 50ms ramp time to avoid clicks
    smoothedGain.reset(sampleRate, 0.05);

    // Set initial gain from parameter
    float gainDb = gainParam->get();
    float gainLinear = juce::Decibels::decibelsToGain(gainDb);
    smoothedGain.setCurrentAndTargetValue(gainLinear);
}

void SimpleGainProcessor::releaseResources()
{
    // Clean up when playback stops
}

void SimpleGainProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    // Get parameter value and convert from dB to linear gain
    float gainDb = gainParam->get();
    float targetGain = juce::Decibels::decibelsToGain(gainDb);
    smoothedGain.setTargetValue(targetGain);

    // Get number of channels and samples
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    // Track peak level for metering
    float peakLevel = 0.0f;

    // Process each channel
    for (int channel = 0; channel < numChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);

        // Process each sample
        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Get smoothed gain value (changes gradually to avoid clicks)
            float gain = smoothedGain.getNextValue();

            // Apply gain
            channelData[sample] *= gain;

            // Track peak for metering
            peakLevel = juce::jmax(peakLevel, std::abs(channelData[sample]));
        }
    }

    // Update level meter (thread-safe atomic)
    currentLevel.store(peakLevel);
}

juce::AudioProcessorEditor* SimpleGainProcessor::createEditor()
{
    return new SimpleGainEditor(*this);
}

void SimpleGainProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Save plugin state
    juce::MemoryOutputStream stream(destData, true);
    stream.writeFloat(gainParam->get());
}

void SimpleGainProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Restore plugin state
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
    gainParam->setValueNotifyingHost(gainParam->convertTo0to1(stream.readFloat()));
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleGainProcessor();
}
