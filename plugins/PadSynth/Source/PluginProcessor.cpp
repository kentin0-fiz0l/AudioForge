#include "PluginProcessor.h"
#include "PluginEditor.h"

PluginProcessor::PluginProcessor()
    : AudioProcessor(BusesProperties()
                    .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , apvts(*this, nullptr, "Parameters", createParameterLayout())
    , presetManager("PadSynth", *this)
{
    // TODO: Load factory presets when ready
    // presetManager.setFactoryPresets(PadSynthPresets::getFactoryPresets());
}

PluginProcessor::~PluginProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Oscillator Waveform
    layout.add(std::make_unique<juce::AudioParameterInt>(
        "waveform", "Waveform",
        0, 3, 1)); // 0=Sine, 1=Saw, 2=Square, 3=Triangle

    // Unison Controls
    layout.add(std::make_unique<juce::AudioParameterInt>(
        "unisonVoices", "Unison Voices",
        3, 9, 7)); // 3-9 voices

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "detuneAmount", "Detune",
        juce::NormalisableRange<float>(0.0f, 50.0f, 0.1f), 10.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "stereoWidth", "Stereo Width",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.8f));

    // Filter Controls
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "filterCutoff", "Filter Cutoff",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f), 2000.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "filterRes", "Filter Resonance",
        juce::NormalisableRange<float>(0.5f, 10.0f, 0.1f), 0.8f));

    layout.add(std::make_unique<juce::AudioParameterInt>(
        "filterType", "Filter Type",
        0, 3, 0)); // 0=LP, 1=HP, 2=BP, 3=Notch

    // Envelope (ADSR)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "attack", "Attack",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f), 0.8f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "decay", "Decay",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f), 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "sustain", "Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.9f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "release", "Release",
        juce::NormalisableRange<float>(0.001f, 10.0f, 0.001f, 0.3f), 1.5f));

    return layout;
}

void PluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Voices are prepared when notes are triggered
}

void PluginProcessor::releaseResources()
{
}

bool PluginProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // Stereo output only
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

void PluginProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // Clear output buffer
    buffer.clear();

    // Process MIDI events
    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();

        if (message.isNoteOn())
        {
            voiceManager.noteOn(message.getNoteNumber(),
                              message.getFloatVelocity(),
                              getSampleRate());
        }
        else if (message.isNoteOff())
        {
            voiceManager.noteOff(message.getNoteNumber());
        }
        else if (message.isAllNotesOff() || message.isAllSoundOff())
        {
            voiceManager.allNotesOff();
        }
    }

    // Get parameters
    int waveform = static_cast<int>(*apvts.getRawParameterValue("waveform"));
    int unisonVoices = static_cast<int>(*apvts.getRawParameterValue("unisonVoices"));
    float detuneAmount = *apvts.getRawParameterValue("detuneAmount");
    float stereoWidth = *apvts.getRawParameterValue("stereoWidth");
    float filterCutoff = *apvts.getRawParameterValue("filterCutoff");
    float filterRes = *apvts.getRawParameterValue("filterRes");
    int filterType = static_cast<int>(*apvts.getRawParameterValue("filterType"));
    float attack = *apvts.getRawParameterValue("attack");
    float decay = *apvts.getRawParameterValue("decay");
    float sustain = *apvts.getRawParameterValue("sustain");
    float release = *apvts.getRawParameterValue("release");

    // Update envelopes for all voices
    for (auto& voice : voiceManager.getVoices())
    {
        voice.updateEnvelope(attack, decay, sustain, release);
    }

    // Process voices (stereo output)
    auto* channelDataL = buffer.getWritePointer(0);
    auto* channelDataR = buffer.getWritePointer(1);

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float outputLeft = 0.0f;
        float outputRight = 0.0f;

        // Sum all voices
        for (auto& voice : voiceManager.getVoices())
        {
            float voiceLeft, voiceRight;
            voice.processSample(getSampleRate(),
                              waveform,
                              unisonVoices,
                              detuneAmount,
                              stereoWidth,
                              filterCutoff,
                              filterRes,
                              filterType,
                              voiceLeft,
                              voiceRight);

            outputLeft += voiceLeft;
            outputRight += voiceRight;
        }

        // Write to output
        channelDataL[sample] = outputLeft;
        channelDataR[sample] = outputRight;
    }
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor(*this);
}

bool PluginProcessor::hasEditor() const
{
    return true;
}

const juce::String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
    return true;
}

bool PluginProcessor::producesMidi() const
{
    return false;
}

bool PluginProcessor::isMidiEffect() const
{
    return false;
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 1;
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

void PluginProcessor::setCurrentProgram(int index)
{
}

const juce::String PluginProcessor::getProgramName(int index)
{
    return {};
}

void PluginProcessor::changeProgramName(int index, const juce::String& newName)
{
}

void PluginProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PluginProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}
