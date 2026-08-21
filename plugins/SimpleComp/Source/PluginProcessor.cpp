#include "PluginProcessor.h"
#include "PluginEditor.h"

SimpleCompProcessor::SimpleCompProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    // Threshold parameter (-60dB to 0dB)
    addParameter(thresholdParam = new juce::AudioParameterFloat(
        PARAM_THRESHOLD,
        "Threshold",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f),
        -20.0f,  // Default: -20 dB
        "dB"));

    // Ratio parameter (1:1 to 20:1)
    addParameter(ratioParam = new juce::AudioParameterFloat(
        PARAM_RATIO,
        "Ratio",
        juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f, 0.5f),  // Slight curve for fine control at low ratios
        4.0f,  // Default: 4:1
        ":1"));

    // Attack parameter (0.1ms to 100ms, log scale)
    addParameter(attackParam = new juce::AudioParameterFloat(
        PARAM_ATTACK,
        "Attack",
        juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f, 0.3f),  // Log scale
        10.0f,  // Default: 10 ms
        "ms"));

    // Release parameter (10ms to 1000ms, log scale)
    addParameter(releaseParam = new juce::AudioParameterFloat(
        PARAM_RELEASE,
        "Release",
        juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f, 0.3f),  // Log scale
        100.0f,  // Default: 100 ms
        "ms"));

    // Knee parameter (Hard / Soft)
    juce::StringArray kneeOptions = { "Hard", "Soft" };
    addParameter(kneeParam = new juce::AudioParameterChoice(
        PARAM_KNEE,
        "Knee",
        kneeOptions,
        0));  // Default: Hard knee
}

SimpleCompProcessor::~SimpleCompProcessor()
{
}

void SimpleCompProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    setRateAndBufferSizeDetails(sampleRate, samplesPerBlock);
    currentSampleRate = sampleRate;

    // Reset envelope followers
    for (auto& state : channelState)
        state.envelopeFollower = 0.0f;

    currentGainReductionDB.store(0.0f);
}

void SimpleCompProcessor::releaseResources()
{
}

void SimpleCompProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Get parameters
    float threshold = thresholdParam->get();
    float ratio = ratioParam->get();
    float attackMs = attackParam->get();
    float releaseMs = releaseParam->get();
    int kneeMode = kneeParam->getIndex();
    float kneeWidth = (kneeMode == 1) ? 6.0f : 0.0f;  // Soft knee = 6dB width

    // Calculate envelope time constants (in samples)
    float attackCoeff = std::exp(-1.0f / (attackMs * 0.001f * static_cast<float>(currentSampleRate)));
    float releaseCoeff = std::exp(-1.0f / (releaseMs * 0.001f * static_cast<float>(currentSampleRate)));

    // Calculate automatic makeup gain
    float makeupGainDB = calculateMakeupGain(threshold, ratio);
    float makeupGainLinear = AudioForge::DSP::Utilities::dbToLinear(makeupGainDB);

    // Track maximum gain reduction for metering
    float maxGainReduction = 0.0f;

    // Process each channel
    for (int channel = 0; channel < numChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        auto& state = channelState[channel];

        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Get input sample
            float input = channelData[sample];

            // Level detection (convert to dB, with floor at -96dB)
            float inputLevel = std::abs(input);
            float inputLevelDB = (inputLevel > 0.0f)
                ? AudioForge::DSP::Utilities::linearToDb(inputLevel)
                : -96.0f;

            // Envelope follower (smooth level detector)
            float coeff = (inputLevelDB > AudioForge::DSP::Utilities::linearToDb(state.envelopeFollower))
                ? attackCoeff   // Attack: level rising
                : releaseCoeff; // Release: level falling

            state.envelopeFollower = coeff * state.envelopeFollower + (1.0f - coeff) * inputLevel;

            // Convert envelope to dB
            float envelopeDB = (state.envelopeFollower > 0.0f)
                ? AudioForge::DSP::Utilities::linearToDb(state.envelopeFollower)
                : -96.0f;

            // Calculate gain reduction
            float gainReductionDB = calculateGainReduction(envelopeDB, threshold, ratio, kneeWidth);

            // Track maximum gain reduction across all channels
            maxGainReduction = std::max(maxGainReduction, gainReductionDB);

            // Convert gain reduction to linear gain
            float gain = AudioForge::DSP::Utilities::dbToLinear(-gainReductionDB);

            // Apply compression and makeup gain
            channelData[sample] = input * gain * makeupGainLinear;
        }
    }

    // Store gain reduction for metering (thread-safe)
    currentGainReductionDB.store(maxGainReduction);
}

float SimpleCompProcessor::calculateGainReduction(float inputLevelDB, float threshold, float ratio, float knee)
{
    if (knee <= 0.0f)
    {
        // Hard knee
        if (inputLevelDB <= threshold)
            return 0.0f;  // Below threshold, no compression

        float overDB = inputLevelDB - threshold;
        return overDB - (overDB / ratio);  // Gain reduction
    }
    else
    {
        // Soft knee
        float kneeStart = threshold - knee / 2.0f;
        float kneeEnd = threshold + knee / 2.0f;

        if (inputLevelDB <= kneeStart)
        {
            // Below knee, no compression
            return 0.0f;
        }
        else if (inputLevelDB >= kneeEnd)
        {
            // Above knee, full compression
            float overDB = inputLevelDB - threshold;
            return overDB - (overDB / ratio);
        }
        else
        {
            // In knee region, gradual compression
            float kneeInput = inputLevelDB - kneeStart;
            float kneeRatio = kneeInput / knee;
            float overDB = inputLevelDB - threshold;
            float gainReduction = overDB - (overDB / ratio);
            return gainReduction * kneeRatio * kneeRatio;  // Quadratic curve
        }
    }
}

float SimpleCompProcessor::calculateMakeupGain(float threshold, float ratio)
{
    // Estimate average gain reduction and compensate
    // Assume average signal level is around -12dB
    float averageLevel = -12.0f;

    if (averageLevel <= threshold)
        return 0.0f;  // No compression on average, no makeup needed

    float overDB = averageLevel - threshold;
    float gainReduction = overDB - (overDB / ratio);

    // Apply 50% of the estimated gain reduction as makeup
    // (conservative to avoid over-compensation)
    return gainReduction * 0.5f;
}

juce::AudioProcessorEditor* SimpleCompProcessor::createEditor()
{
    return new SimpleCompEditor(*this);
}

void SimpleCompProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream(destData, true);

    stream.writeFloat(thresholdParam->get());
    stream.writeFloat(ratioParam->get());
    stream.writeFloat(attackParam->get());
    stream.writeFloat(releaseParam->get());
    stream.writeInt(kneeParam->getIndex());
}

void SimpleCompProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);

    thresholdParam->setValueNotifyingHost(thresholdParam->convertTo0to1(stream.readFloat()));
    ratioParam->setValueNotifyingHost(ratioParam->convertTo0to1(stream.readFloat()));
    attackParam->setValueNotifyingHost(attackParam->convertTo0to1(stream.readFloat()));
    releaseParam->setValueNotifyingHost(releaseParam->convertTo0to1(stream.readFloat()));

    int kneeIndex = stream.readInt();
    kneeParam->setValueNotifyingHost(static_cast<float>(kneeIndex) / static_cast<float>(kneeParam->choices.size() - 1));
}

//==============================================================================
// Plugin factory
#ifndef AUDIOFORGE_TESTS
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleCompProcessor();
}
#endif
