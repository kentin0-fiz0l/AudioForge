#include "PluginProcessor.h"
#include "PluginEditor.h"

SimpleEQProcessor::SimpleEQProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    // Low shelf parameters
    addParameter(lowFreqParam = new juce::AudioParameterFloat(
        PARAM_LOW_FREQ,
        "Low Frequency",
        juce::NormalisableRange<float>(20.0f, 500.0f, 1.0f, 0.3f),  // Log scale
        80.0f,  // Default: 80 Hz
        "Hz"));

    addParameter(lowGainParam = new juce::AudioParameterFloat(
        PARAM_LOW_GAIN,
        "Low Gain",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f,  // Default: 0 dB (unity)
        "dB"));

    // Mid band parameters
    addParameter(midFreqParam = new juce::AudioParameterFloat(
        PARAM_MID_FREQ,
        "Mid Frequency",
        juce::NormalisableRange<float>(200.0f, 5000.0f, 1.0f, 0.3f),  // Log scale
        1000.0f,  // Default: 1 kHz
        "Hz"));

    addParameter(midGainParam = new juce::AudioParameterFloat(
        PARAM_MID_GAIN,
        "Mid Gain",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f,  // Default: 0 dB (unity)
        "dB"));

    // High shelf parameters
    addParameter(highFreqParam = new juce::AudioParameterFloat(
        PARAM_HIGH_FREQ,
        "High Frequency",
        juce::NormalisableRange<float>(2000.0f, 20000.0f, 1.0f, 0.3f),  // Log scale
        8000.0f,  // Default: 8 kHz
        "Hz"));

    addParameter(highGainParam = new juce::AudioParameterFloat(
        PARAM_HIGH_GAIN,
        "High Gain",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f,  // Default: 0 dB (unity)
        "dB"));

    // High-pass filter parameters
    addParameter(hpfEnabledParam = new juce::AudioParameterBool(
        PARAM_HPF_ENABLED,
        "HPF Enabled",
        false));  // Default: off

    addParameter(hpfFreqParam = new juce::AudioParameterFloat(
        PARAM_HPF_FREQ,
        "HPF Frequency",
        juce::NormalisableRange<float>(20.0f, 500.0f, 1.0f, 0.3f),  // Log scale
        40.0f,  // Default: 40 Hz
        "Hz"));

    // Low-pass filter parameters
    addParameter(lpfEnabledParam = new juce::AudioParameterBool(
        PARAM_LPF_ENABLED,
        "LPF Enabled",
        false));  // Default: off

    addParameter(lpfFreqParam = new juce::AudioParameterFloat(
        PARAM_LPF_FREQ,
        "LPF Frequency",
        juce::NormalisableRange<float>(2000.0f, 20000.0f, 1.0f, 0.3f),  // Log scale
        12000.0f,  // Default: 12 kHz
        "Hz"));
}

SimpleEQProcessor::~SimpleEQProcessor()
{
}

void SimpleEQProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    setRateAndBufferSizeDetails(sampleRate, samplesPerBlock);
    currentSampleRate = sampleRate;

    // Reset all filters
    for (auto& filters : channelFilters)
        filters.reset();

    // Invalidate filter cache to force recalculation
    filterCache = FilterCache();
}

void SimpleEQProcessor::releaseResources()
{
}

void SimpleEQProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Get parameters
    float lowFreq = lowFreqParam->get();
    float lowGain = lowGainParam->get();
    float midFreq = midFreqParam->get();
    float midGain = midGainParam->get();
    float highFreq = highFreqParam->get();
    float highGain = highGainParam->get();
    bool hpfEnabled = hpfEnabledParam->get();
    float hpfFreq = hpfFreqParam->get();
    bool lpfEnabled = lpfEnabledParam->get();
    float lpfFreq = lpfFreqParam->get();

    // Measure input level
    inputLevelMeter.updateLevel(buffer.getMagnitude(0, numSamples));

    // Update filter coefficients if parameters changed
    bool lowShelfChanged = (lowFreq != filterCache.lowFreq || lowGain != filterCache.lowGain);
    bool midBandChanged = (midFreq != filterCache.midFreq || midGain != filterCache.midGain);
    bool highShelfChanged = (highFreq != filterCache.highFreq || highGain != filterCache.highGain);
    bool hpfChanged = (hpfFreq != filterCache.hpfFreq);
    bool lpfChanged = (lpfFreq != filterCache.lpfFreq);

    if (lowShelfChanged)
    {
        auto coeffs = AudioForge::DSP::FilterDesign::makeLowShelf(
            lowFreq,
            static_cast<float>(currentSampleRate),
            lowGain,
            0.707f);

        for (auto& filters : channelFilters)
            filters.lowShelf.setCoefficients(coeffs);

        filterCache.lowFreq = lowFreq;
        filterCache.lowGain = lowGain;
    }

    if (midBandChanged)
    {
        auto coeffs = AudioForge::DSP::FilterDesign::makePeakingEQ(
            midFreq,
            static_cast<float>(currentSampleRate),
            midGain,
            1.0f);

        for (auto& filters : channelFilters)
            filters.midBand.setCoefficients(coeffs);

        filterCache.midFreq = midFreq;
        filterCache.midGain = midGain;
    }

    if (highShelfChanged)
    {
        auto coeffs = AudioForge::DSP::FilterDesign::makeHighShelf(
            highFreq,
            static_cast<float>(currentSampleRate),
            highGain,
            0.707f);

        for (auto& filters : channelFilters)
            filters.highShelf.setCoefficients(coeffs);

        filterCache.highFreq = highFreq;
        filterCache.highGain = highGain;
    }

    if (hpfChanged)
    {
        auto coeffs = AudioForge::DSP::FilterDesign::makeHighPass(
            hpfFreq,
            static_cast<float>(currentSampleRate),
            0.707f);  // Q (Butterworth)

        for (auto& filters : channelFilters)
            filters.highPass.setCoefficients(coeffs);

        filterCache.hpfFreq = hpfFreq;
    }

    if (lpfChanged)
    {
        auto coeffs = AudioForge::DSP::FilterDesign::makeLowPass(
            lpfFreq,
            static_cast<float>(currentSampleRate),
            0.707f);  // Q (Butterworth)

        for (auto& filters : channelFilters)
            filters.lowPass.setCoefficients(coeffs);

        filterCache.lpfFreq = lpfFreq;
    }

    // Process each channel
    for (int channel = 0; channel < numChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        auto& filters = channelFilters[channel];

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float output = channelData[sample];

            // Apply high-pass filter first (if enabled)
            if (hpfEnabled)
                output = filters.highPass.processSample(output);

            // Apply EQ bands
            output = filters.lowShelf.processSample(output);
            output = filters.midBand.processSample(output);
            output = filters.highShelf.processSample(output);

            // Apply low-pass filter last (if enabled)
            if (lpfEnabled)
                output = filters.lowPass.processSample(output);

            channelData[sample] = output;
        }
    }

    // Measure output level
    outputLevelMeter.updateLevel(buffer.getMagnitude(0, numSamples));
}

juce::AudioProcessorEditor* SimpleEQProcessor::createEditor()
{
    return new SimpleEQEditor(*this);
}

void SimpleEQProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream(destData, true);

    stream.writeFloat(lowFreqParam->get());
    stream.writeFloat(lowGainParam->get());
    stream.writeFloat(midFreqParam->get());
    stream.writeFloat(midGainParam->get());
    stream.writeFloat(highFreqParam->get());
    stream.writeFloat(highGainParam->get());
    stream.writeBool(hpfEnabledParam->get());
    stream.writeFloat(hpfFreqParam->get());
    stream.writeBool(lpfEnabledParam->get());
    stream.writeFloat(lpfFreqParam->get());
}

void SimpleEQProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);

    lowFreqParam->setValueNotifyingHost(lowFreqParam->convertTo0to1(stream.readFloat()));
    lowGainParam->setValueNotifyingHost(lowGainParam->convertTo0to1(stream.readFloat()));
    midFreqParam->setValueNotifyingHost(midFreqParam->convertTo0to1(stream.readFloat()));
    midGainParam->setValueNotifyingHost(midGainParam->convertTo0to1(stream.readFloat()));
    highFreqParam->setValueNotifyingHost(highFreqParam->convertTo0to1(stream.readFloat()));
    highGainParam->setValueNotifyingHost(highGainParam->convertTo0to1(stream.readFloat()));
    hpfEnabledParam->setValueNotifyingHost(stream.readBool() ? 1.0f : 0.0f);
    hpfFreqParam->setValueNotifyingHost(hpfFreqParam->convertTo0to1(stream.readFloat()));
    lpfEnabledParam->setValueNotifyingHost(stream.readBool() ? 1.0f : 0.0f);
    lpfFreqParam->setValueNotifyingHost(lpfFreqParam->convertTo0to1(stream.readFloat()));
}

//==============================================================================
// Plugin factory
#ifndef AUDIOFORGE_TESTS
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleEQProcessor();
}
#endif
