#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

ChorusFlangerAudioProcessor::ChorusFlangerAudioProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    // Rate parameter (0.1-10 Hz, log scale)
    addParameter(rateParam = new juce::AudioParameterFloat(
        "rate",
        "Rate",
        juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, 0.3f),
        0.5f,
        "Hz"));

    // Depth parameter (0-100%)
    addParameter(depthParam = new juce::AudioParameterFloat(
        "depth",
        "Depth",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        50.0f,
        "%"));

    // Mode parameter (Chorus/Flanger)
    addParameter(modeParam = new juce::AudioParameterChoice(
        "mode",
        "Mode",
        juce::StringArray{"Chorus", "Flanger"},
        0));

    // Feedback parameter (0-95%)
    addParameter(feedbackParam = new juce::AudioParameterFloat(
        "feedback",
        "Feedback",
        juce::NormalisableRange<float>(0.0f, 95.0f, 0.1f),
        20.0f,
        "%"));

    // Mix parameter (0-100%)
    addParameter(mixParam = new juce::AudioParameterFloat(
        "mix",
        "Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        50.0f,
        "%"));

    // Waveform parameter (Sine/Triangle)
    addParameter(waveformParam = new juce::AudioParameterChoice(
        "waveform",
        "Waveform",
        juce::StringArray{"Sine", "Triangle"},
        0));

    // Stereo Width parameter (0-100%)
    addParameter(stereoWidthParam = new juce::AudioParameterFloat(
        "stereoWidth",
        "Stereo Width",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        50.0f,
        "%"));
}

ChorusFlangerAudioProcessor::~ChorusFlangerAudioProcessor()
{
}

const juce::String ChorusFlangerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ChorusFlangerAudioProcessor::acceptsMidi() const { return false; }
bool ChorusFlangerAudioProcessor::producesMidi() const { return false; }
bool ChorusFlangerAudioProcessor::isMidiEffect() const { return false; }
double ChorusFlangerAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int ChorusFlangerAudioProcessor::getNumPrograms() { return 1; }
int ChorusFlangerAudioProcessor::getCurrentProgram() { return 0; }
void ChorusFlangerAudioProcessor::setCurrentProgram (int) {}
const juce::String ChorusFlangerAudioProcessor::getProgramName (int) { return {}; }
void ChorusFlangerAudioProcessor::changeProgramName (int, const juce::String&) {}

void ChorusFlangerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;

    // Allocate delay buffers (2 seconds max)
    delayBufferLeft.resize(maxDelayBufferSize, 0.0f);
    delayBufferRight.resize(maxDelayBufferSize, 0.0f);
    writePosition = 0;

    // Reset LFO phases
    lfoPhaseLeft = 0.0f;
    lfoPhaseRight = 0.0f;

    // Initialize DC blocker (high-pass at 20 Hz)
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 2;

    dcBlocker.prepare(spec);
    *dcBlocker.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 20.0);

    // Clear meters
    inputMeter.clear();
    outputMeter.clear();
}

void ChorusFlangerAudioProcessor::releaseResources()
{
}

bool ChorusFlangerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

float ChorusFlangerAudioProcessor::getLFOSample(float phase, int waveform)
{
    if (waveform == 0) // Sine
    {
        return std::sin(phase * 2.0f * juce::MathConstants<float>::pi);
    }
    else // Triangle
    {
        float tri = 4.0f * std::abs(phase - 0.5f) - 1.0f;
        return tri;
    }
}

float ChorusFlangerAudioProcessor::getInterpolatedDelaySample(
    const std::vector<float>& buffer, float delayInSamples)
{
    // Calculate read position (fractional)
    float readPos = static_cast<float>(writePosition) - delayInSamples;
    while (readPos < 0.0f)
        readPos += maxDelayBufferSize;

    // Get integer and fractional parts
    int readPosInt = static_cast<int>(readPos);
    float frac = readPos - readPosInt;

    // Linear interpolation between two samples
    int readPosNext = (readPosInt + 1) % maxDelayBufferSize;
    float sample1 = buffer[readPosInt];
    float sample2 = buffer[readPosNext];

    return sample1 + frac * (sample2 - sample1);
}

void ChorusFlangerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Get parameters
    const float rate = rateParam->get();
    const float depth = depthParam->get() / 100.0f;  // 0-1
    const int mode = modeParam->getIndex();  // 0=Chorus, 1=Flanger
    const float feedback = feedbackParam->get() / 100.0f;  // 0-0.95
    const float mix = mixParam->get() / 100.0f;  // 0-1
    const int waveform = waveformParam->getIndex();  // 0=Sine, 1=Triangle
    const float stereoWidth = stereoWidthParam->get() / 100.0f;  // 0-1

    // Calculate delay range based on mode
    float baseDelayMs, modulationMs;
    if (mode == 0) // Chorus
    {
        baseDelayMs = 15.0f;  // 15ms base delay
        modulationMs = 10.0f; // ±10ms modulation
    }
    else // Flanger
    {
        baseDelayMs = 3.0f;   // 3ms base delay
        modulationMs = 2.5f;  // ±2.5ms modulation
    }

    // Convert to samples
    const float baseDelaySamples = baseDelayMs * 0.001f * static_cast<float>(currentSampleRate);
    const float modulationSamples = modulationMs * 0.001f * static_cast<float>(currentSampleRate);

    // Calculate LFO phase increment per sample
    const float phaseIncrement = rate / static_cast<float>(currentSampleRate);

    // Calculate input peak level
    float inputPeak = 0.0f;
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        const float* channelData = buffer.getReadPointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            inputPeak = std::max(inputPeak, std::abs(channelData[sample]));
    }
    inputMeter.updateLevel(inputPeak);

    // Process audio
    const int numSamples = buffer.getNumSamples();
    auto* leftChannel = buffer.getWritePointer(0);
    auto* rightChannel = totalNumInputChannels > 1 ? buffer.getWritePointer(1) : nullptr;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Store dry signals
        const float dryLeft = leftChannel[sample];
        const float dryRight = rightChannel ? rightChannel[sample] : dryLeft;

        // Calculate LFO values for left and right channels
        const float lfoLeft = getLFOSample(lfoPhaseLeft, waveform);
        const float lfoRight = getLFOSample(lfoPhaseRight, waveform);

        // Calculate modulated delay times
        const float delayLeft = baseDelaySamples + lfoLeft * modulationSamples * depth;
        const float delayRight = baseDelaySamples + lfoRight * modulationSamples * depth;

        // Read from delay buffers with interpolation
        const float delayedLeft = getInterpolatedDelaySample(delayBufferLeft, delayLeft);
        const float delayedRight = getInterpolatedDelaySample(delayBufferRight, delayRight);

        // Mix with feedback
        const float wetLeft = delayedLeft;
        const float wetRight = delayedRight;

        // Write to delay buffers (input + feedback)
        delayBufferLeft[writePosition] = dryLeft + wetLeft * feedback;
        delayBufferRight[writePosition] = dryRight + wetRight * feedback;

        // Mix dry/wet
        leftChannel[sample] = dryLeft * (1.0f - mix) + wetLeft * mix;
        if (rightChannel)
            rightChannel[sample] = dryRight * (1.0f - mix) + wetRight * mix;

        // Advance write position
        writePosition = (writePosition + 1) % maxDelayBufferSize;

        // Advance LFO phases
        lfoPhaseLeft += phaseIncrement;
        if (lfoPhaseLeft >= 1.0f)
            lfoPhaseLeft -= 1.0f;

        // Right channel LFO with phase offset based on stereo width
        const float phaseOffset = stereoWidth * 0.5f;  // 0-0.5 (0-180 degrees)
        lfoPhaseRight = lfoPhaseLeft + phaseOffset;
        if (lfoPhaseRight >= 1.0f)
            lfoPhaseRight -= 1.0f;
    }

    // Apply DC blocker
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    dcBlocker.process(context);

    // Calculate output peak level
    float outputPeak = 0.0f;
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        const float* channelData = buffer.getReadPointer(channel);
        for (int sample = 0; sample < numSamples; ++sample)
            outputPeak = std::max(outputPeak, std::abs(channelData[sample]));
    }
    outputMeter.updateLevel(outputPeak);
}

bool ChorusFlangerAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* ChorusFlangerAudioProcessor::createEditor()
{
    return new ChorusFlangerAudioProcessorEditor (*this);
}

void ChorusFlangerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream(destData, true);
    stream.writeFloat(*rateParam);
    stream.writeFloat(*depthParam);
    stream.writeInt(modeParam->getIndex());
    stream.writeFloat(*feedbackParam);
    stream.writeFloat(*mixParam);
    stream.writeInt(waveformParam->getIndex());
    stream.writeFloat(*stereoWidthParam);
}

void ChorusFlangerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
    *rateParam = stream.readFloat();
    *depthParam = stream.readFloat();
    *modeParam = stream.readInt();
    *feedbackParam = stream.readFloat();
    *mixParam = stream.readFloat();
    *waveformParam = stream.readInt();
    *stereoWidthParam = stream.readFloat();
}

#ifndef AUDIOFORGE_TESTS
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChorusFlangerAudioProcessor();
}
#endif
