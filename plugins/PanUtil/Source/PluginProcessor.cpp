#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
    // What the state was before Gain and Mono: pan, width and mode
    constexpr int legacyStateSize = 12;
    constexpr int stateSize = 20;
}

PanUtilProcessor::PanUtilProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    // Pan parameter: -1.0 (left) to +1.0 (right), default 0.0 (center)
    addParameter(panParam = new juce::AudioParameterFloat(
        "pan",
        "Pan",
        juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f),
        0.0f));

    // Width parameter: 0.0 (mono) to 2.0 (wide), default 1.0 (normal)
    addParameter(widthParam = new juce::AudioParameterFloat(
        "width",
        "Width",
        juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f),
        1.0f));

    // Mode parameter: Pan or Balance. Balance is the default because it
    // leaves a centred signal at its own level; the pan law takes 3 dB off.
    addParameter(modeParam = new juce::AudioParameterChoice(
        "mode",
        "Mode",
        juce::StringArray{"Pan", "Balance"},
        1));

    // Gain and Mono. Added last, so the parameters before them keep their
    // positions.
    addParameter(gainParam = new juce::AudioParameterFloat(
        "gain",
        "Gain",
        juce::NormalisableRange<float>(-36.0f, 24.0f, 0.1f),
        0.0f,
        "dB"));

    addParameter(monoParam = new juce::AudioParameterBool(
        "mono",
        "Mono",
        false));
}

PanUtilProcessor::~PanUtilProcessor()
{
}

void PanUtilProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Initialize smoothed values with 50ms ramp time
    smoothedPan.reset(sampleRate, 0.05);
    smoothedWidth.reset(sampleRate, 0.05);

    smoothedGain.reset(sampleRate, 0.05);

    smoothedPan.setCurrentAndTargetValue(panParam->get());
    smoothedWidth.setCurrentAndTargetValue(monoParam->get() ? 0.0f : widthParam->get());
    smoothedGain.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(gainParam->get()));
}

bool PanUtilProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // Pan, width and mono all need two channels
    return layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo()
        && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void PanUtilProcessor::releaseResources()
{
}

void PanUtilProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    // Ensure we have stereo input
    if (buffer.getNumChannels() < 2)
        return;

    // Get parameters
    float panValue = panParam->get();
    float widthValue = widthParam->get();
    int mode = modeParam->getIndex();

    // Mono is no width at all, whatever Width says. Going through the
    // smoother, switching it does not click.
    smoothedPan.setTargetValue(panValue);
    smoothedWidth.setTargetValue(monoParam->get() ? 0.0f : widthValue);
    smoothedGain.setTargetValue(juce::Decibels::decibelsToGain(gainParam->get()));

    const int numSamples = buffer.getNumSamples();
    float* leftChannel = buffer.getWritePointer(0);
    float* rightChannel = buffer.getWritePointer(1);

    float peakLeft = 0.0f;
    float peakRight = 0.0f;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float pan = smoothedPan.getNextValue();
        float width = smoothedWidth.getNextValue();
        const float gain = smoothedGain.getNextValue();

        float L = leftChannel[sample];
        float R = rightChannel[sample];
        float outL, outR;

        // Apply width control first
        AudioForge::DSP::PanningAlgorithms::applyStereoWidth(L, R, width, outL, outR);

        // Constant-power panning in Pan mode; Balance only turns one side down
        const auto gains = mode == 0 ? AudioForge::DSP::PanningAlgorithms::constantPowerPan(pan)
                                     : AudioForge::DSP::PanningAlgorithms::balance(pan);

        leftChannel[sample] = outL * gains.leftGain * gain;
        rightChannel[sample] = outR * gains.rightGain * gain;

        // Track peaks for metering
        peakLeft = juce::jmax(peakLeft, std::abs(leftChannel[sample]));
        peakRight = juce::jmax(peakRight, std::abs(rightChannel[sample]));
    }

    // Update meters (thread-safe)
    leftMeter.updateLevel(peakLeft);
    rightMeter.updateLevel(peakRight);
}

juce::AudioProcessorEditor* PanUtilProcessor::createEditor()
{
    return new PanUtilEditor(*this);
}

void PanUtilProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream(destData, true);
    stream.writeFloat(panParam->get());
    stream.writeFloat(widthParam->get());
    stream.writeInt(modeParam->getIndex());
    stream.writeFloat(gainParam->get());
    stream.writeInt(monoParam->get() ? 1 : 0);
}

void PanUtilProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Too short to be a state: reading it would give zeros, and a width of
    // zero is mono
    if (data == nullptr || sizeInBytes < legacyStateSize)
        return;

    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);

    // A value that is not a number is skipped: it would get into the
    // smoothing and stay there
    const auto restore = [&stream] (juce::AudioParameterFloat& param)
    {
        const float value = stream.readFloat();
        if (std::isfinite(value))
            param.setValueNotifyingHost(param.convertTo0to1(value));
    };

    restore(*panParam);
    restore(*widthParam);
    modeParam->setValueNotifyingHost(static_cast<float>(stream.readInt()) /
                                     (modeParam->choices.size() - 1));

    // A state from before Gain and Mono leaves them as they are
    if (sizeInBytes < stateSize)
        return;

    restore(*gainParam);
    monoParam->setValueNotifyingHost(stream.readInt() != 0 ? 1.0f : 0.0f);
}

//==============================================================================
#ifndef AUDIOFORGE_TESTS
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PanUtilProcessor();
}
#endif
