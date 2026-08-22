#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "dsp/WaveShaping.h"
#include <cmath>

WaveShaperAudioProcessor::WaveShaperAudioProcessor()
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

    // Mix parameter (dry/wet, 0-100%)
    addParameter(mixParam = new juce::AudioParameterFloat(
        "mix",
        "Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        100.0f,
        "%"));

    // Tone parameter (low-pass filter cutoff, 0-100%)
    addParameter(toneParam = new juce::AudioParameterFloat(
        "tone",
        "Tone",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        100.0f,
        "%"));

    // Shape mode
    addParameter(shapeParam = new juce::AudioParameterChoice(
        "shape",
        "Shape",
        juce::StringArray{"Soft Clip", "Hard Clip", "Tanh", "Cubic", "Asymmetric"},
        0));

    // Output gain (-12 to +12 dB)
    addParameter(outputGainParam = new juce::AudioParameterFloat(
        "outputGain",
        "Output",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f,
        "dB"));
}

WaveShaperAudioProcessor::~WaveShaperAudioProcessor()
{
}

const juce::String WaveShaperAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool WaveShaperAudioProcessor::acceptsMidi() const { return false; }
bool WaveShaperAudioProcessor::producesMidi() const { return false; }
bool WaveShaperAudioProcessor::isMidiEffect() const { return false; }
double WaveShaperAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int WaveShaperAudioProcessor::getNumPrograms() { return 1; }
int WaveShaperAudioProcessor::getCurrentProgram() { return 0; }
void WaveShaperAudioProcessor::setCurrentProgram (int) {}
const juce::String WaveShaperAudioProcessor::getProgramName (int) { return {}; }
void WaveShaperAudioProcessor::changeProgramName (int, const juce::String&) {}

void WaveShaperAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    // Initialize tone filter (low-pass)
    toneFilter.prepare(spec);
    toneFilter.reset();

    // Initialize DC blocker (high-pass at 20 Hz)
    dcBlocker.prepare(spec);
    *dcBlocker.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 20.0);

    // Clear meters
    inputMeter.clear();
    outputMeter.clear();
}

void WaveShaperAudioProcessor::releaseResources()
{
}

bool WaveShaperAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void WaveShaperAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Get parameters
    const float drive = driveParam->get() / 100.0f;  // 0-1
    const float mix = mixParam->get() / 100.0f;      // 0-1
    const float tone = toneParam->get() / 100.0f;    // 0-1
    const int shapeMode = shapeParam->getIndex();
    const float outputGain = juce::Decibels::decibelsToGain(outputGainParam->get());

    // Update tone filter cutoff (map 0-100% to 500Hz-20kHz)
    const float cutoffFreq = 500.0f + tone * (20000.0f - 500.0f);
    *toneFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(
        currentSampleRate, cutoffFreq, 0.707f);

    // Process audio
    const int numSamples = buffer.getNumSamples();

    // Calculate input peak level
    float inputPeak = 0.0f;
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        const float* channelData = buffer.getReadPointer(channel);
        for (int sample = 0; sample < numSamples; ++sample)
            inputPeak = std::max(inputPeak, std::abs(channelData[sample]));
    }
    inputMeter.updateLevel(inputPeak);

    // Process each channel
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            const float input = channelData[sample];
            const float drySignal = input;

            // Apply drive (0-10x multiplier)
            float driven = input * (1.0f + drive * 10.0f);

            // Apply waveshaping based on selected mode
            float shaped = AudioForge::DSP::WaveShaping::shape(driven, shapeMode);

            // Mix dry/wet
            float output = drySignal * (1.0f - mix) + shaped * mix;

            // Apply output gain
            output *= outputGain;

            channelData[sample] = output;
        }
    }

    // Apply tone filter
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    toneFilter.process(context);

    // Apply DC blocker
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

bool WaveShaperAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* WaveShaperAudioProcessor::createEditor()
{
    return new WaveShaperAudioProcessorEditor (*this);
}

void WaveShaperAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream(destData, true);
    stream.writeFloat(*driveParam);
    stream.writeFloat(*mixParam);
    stream.writeFloat(*toneParam);
    stream.writeInt(shapeParam->getIndex());
    stream.writeFloat(*outputGainParam);
}

void WaveShaperAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
    *driveParam = stream.readFloat();
    *mixParam = stream.readFloat();
    *toneParam = stream.readFloat();
    *shapeParam = stream.readInt();
    *outputGainParam = stream.readFloat();
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WaveShaperAudioProcessor();
}
