#include "PluginProcessor.h"
#include "PluginEditor.h"

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

    // Mode parameter: Pan or Balance
    addParameter(modeParam = new juce::AudioParameterChoice(
        "mode",
        "Mode",
        juce::StringArray{"Pan", "Balance"},
        0));
}

PanUtilProcessor::~PanUtilProcessor()
{
}

void PanUtilProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Initialize smoothed values with 50ms ramp time
    smoothedPan.reset(sampleRate, 0.05);
    smoothedWidth.reset(sampleRate, 0.05);

    smoothedPan.setCurrentAndTargetValue(panParam->get());
    smoothedWidth.setCurrentAndTargetValue(widthParam->get());
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

    smoothedPan.setTargetValue(panValue);
    smoothedWidth.setTargetValue(widthValue);

    const int numSamples = buffer.getNumSamples();
    float* leftChannel = buffer.getWritePointer(0);
    float* rightChannel = buffer.getWritePointer(1);

    float peakLeft = 0.0f;
    float peakRight = 0.0f;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float pan = smoothedPan.getNextValue();
        float width = smoothedWidth.getNextValue();

        float L = leftChannel[sample];
        float R = rightChannel[sample];

        // Apply width control first (M/S processing)
        float mid = (L + R) * 0.5f;   // Sum to mono
        float side = (L - R) * 0.5f;  // Difference (stereo info)

        // Scale side signal by width
        side *= width;

        // Convert back to L/R
        L = mid + side;
        R = mid - side;

        // Apply panning
        if (mode == 0)  // Pan mode
        {
            // Constant power panning
            // pan = -1.0 (left), 0.0 (center), +1.0 (right)
            float angle = (pan + 1.0f) * 0.25f * juce::MathConstants<float>::pi;
            float leftGain = std::cos(angle);
            float rightGain = std::sin(angle);

            leftChannel[sample] = L * leftGain;
            rightChannel[sample] = R * rightGain;
        }
        else  // Balance mode
        {
            // Balance: attenuate one side, not the other
            if (pan < 0.0f)  // Pan left = reduce right
            {
                leftChannel[sample] = L;
                rightChannel[sample] = R * (1.0f + pan);
            }
            else  // Pan right = reduce left
            {
                leftChannel[sample] = L * (1.0f - pan);
                rightChannel[sample] = R;
            }
        }

        // Track peaks for metering
        peakLeft = juce::jmax(peakLeft, std::abs(leftChannel[sample]));
        peakRight = juce::jmax(peakRight, std::abs(rightChannel[sample]));
    }

    // Update meters
    leftLevel.store(peakLeft);
    rightLevel.store(peakRight);
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
}

void PanUtilProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
    panParam->setValueNotifyingHost(panParam->convertTo0to1(stream.readFloat()));
    widthParam->setValueNotifyingHost(widthParam->convertTo0to1(stream.readFloat()));
    modeParam->setValueNotifyingHost(static_cast<float>(stream.readInt()) /
                                     (modeParam->choices.size() - 1));
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PanUtilProcessor();
}
