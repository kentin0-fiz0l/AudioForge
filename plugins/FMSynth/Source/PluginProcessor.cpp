#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "FactoryPresets.h"

//==============================================================================
PluginProcessor::PluginProcessor()
    : AudioProcessor(BusesProperties()
                    .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , apvts(*this, nullptr, "Parameters", createParameterLayout())
    , presetManager("FMSynth", *this)
{
    // Load factory presets
    presetManager.setFactoryPresets(FMSynthPresets::getFactoryPresets());
    presetManager.scanPresets();
}

PluginProcessor::~PluginProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Modulator parameters
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "modRatio", "Mod Ratio",
        juce::NormalisableRange<float>(0.5f, 8.0f, 0.1f), 1.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "modDepth", "Mod Depth",
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.1f), 2.0f));

    // Carrier ADSR
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "carrierAttack", "Carrier Attack",
        juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f, 0.3f), 0.01f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "carrierDecay", "Carrier Decay",
        juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f, 0.3f), 0.1f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "carrierSustain", "Carrier Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.7f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "carrierRelease", "Carrier Release",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f), 0.3f));

    // Modulator ADSR
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "modAttack", "Mod Attack",
        juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f, 0.3f), 0.01f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "modDecay", "Mod Decay",
        juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f, 0.3f), 0.1f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "modSustain", "Mod Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.7f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "modRelease", "Mod Release",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f), 0.3f));

    return layout;
}

void PluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Nothing specific to prepare for FM synthesis
}

void PluginProcessor::releaseResources()
{
}

bool PluginProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

void PluginProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    // Get parameters
    float modRatio = apvts.getParameter("modRatio")->getValue() * 7.5f + 0.5f;
    float modDepth = apvts.getParameter("modDepth")->getValue() * 10.0f;

    float carrierAttack = apvts.getParameter("carrierAttack")->getValue() * 1.999f + 0.001f;
    float carrierDecay = apvts.getParameter("carrierDecay")->getValue() * 1.999f + 0.001f;
    float carrierSustain = apvts.getParameter("carrierSustain")->getValue();
    float carrierRelease = apvts.getParameter("carrierRelease")->getValue() * 4.999f + 0.001f;

    float modAttack = apvts.getParameter("modAttack")->getValue() * 1.999f + 0.001f;
    float modDecay = apvts.getParameter("modDecay")->getValue() * 1.999f + 0.001f;
    float modSustain = apvts.getParameter("modSustain")->getValue();
    float modRelease = apvts.getParameter("modRelease")->getValue() * 4.999f + 0.001f;

    // Process MIDI
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

    // Update voice envelopes
    for (auto& voice : voiceManager.getVoices())
    {
        voice.updateEnvelopes(carrierAttack, carrierDecay, carrierSustain, carrierRelease,
                            modAttack, modDecay, modSustain, modRelease);
    }

    // Process audio
    auto* channelDataL = buffer.getWritePointer(0);
    auto* channelDataR = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float outputSample = 0.0f;

        for (auto& voice : voiceManager.getVoices())
        {
            outputSample += voice.processSample(getSampleRate(), modRatio, modDepth);
        }

        channelDataL[sample] = outputSample;
        if (channelDataR)
            channelDataR[sample] = outputSample;
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
