#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "FactoryPresets.h"

PluginProcessor::PluginProcessor()
    : AudioProcessor(BusesProperties()
                    .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , apvts(*this, nullptr, "Parameters", createParameterLayout())
    , presetManager("OrganEmulator", *this)
{
    presetManager.setFactoryPresets(OrganEmulatorPresets::getFactoryPresets());
}

PluginProcessor::~PluginProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Drawbar Parameters (9) - 0.0 to 1.0
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "drawbar16", "16' Drawbar",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.8f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "drawbar5_3", "5⅓' Drawbar",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.6f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "drawbar8", "8' Drawbar",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "drawbar4", "4' Drawbar",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "drawbar2_3", "2⅔' Drawbar",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.4f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "drawbar2", "2' Drawbar",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.3f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "drawbar1_3_5", "1⅗' Drawbar",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.2f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "drawbar1_1_3", "1⅓' Drawbar",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.2f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "drawbar1", "1' Drawbar",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.1f));

    // Leslie Parameters (3)
    layout.add(std::make_unique<juce::AudioParameterBool>(
        "leslieSpeed", "Leslie Speed", false)); // false = slow, true = fast

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "leslieDepth", "Leslie Depth",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "leslieMix", "Leslie Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.7f));

    // Percussion Parameters (2)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "percussionClick", "Percussion Click",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.3f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "percussionDecay", "Percussion Decay",
        juce::NormalisableRange<float>(0.01f, 0.5f, 0.01f), 0.05f));

    return layout;
}

void PluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Prepare all voices
    for (auto& voice : voiceManager.getVoices())
    {
        voice.prepare(sampleRate, samplesPerBlock);
    }
}

void PluginProcessor::releaseResources()
{
}

bool PluginProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // Mono or stereo output
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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
        else if (message.isAllNotesOff())
        {
            voiceManager.allNotesOff();
        }
    }

    // Get parameters
    float drawbars[9];
    drawbars[0] = *apvts.getRawParameterValue("drawbar16");
    drawbars[1] = *apvts.getRawParameterValue("drawbar5_3");
    drawbars[2] = *apvts.getRawParameterValue("drawbar8");
    drawbars[3] = *apvts.getRawParameterValue("drawbar4");
    drawbars[4] = *apvts.getRawParameterValue("drawbar2_3");
    drawbars[5] = *apvts.getRawParameterValue("drawbar2");
    drawbars[6] = *apvts.getRawParameterValue("drawbar1_3_5");
    drawbars[7] = *apvts.getRawParameterValue("drawbar1_1_3");
    drawbars[8] = *apvts.getRawParameterValue("drawbar1");

    bool leslieSpeed = *apvts.getRawParameterValue("leslieSpeed") > 0.5f;
    float leslieDepth = *apvts.getRawParameterValue("leslieDepth");
    float leslieMix = *apvts.getRawParameterValue("leslieMix");

    float percussionClick = *apvts.getRawParameterValue("percussionClick");
    float percussionDecay = *apvts.getRawParameterValue("percussionDecay");

    // Process audio
    auto* channelDataL = buffer.getWritePointer(0);
    auto* channelDataR = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float outL = 0.0f;
        float outR = 0.0f;

        // Process all active voices
        for (auto& voice : voiceManager.getVoices())
        {
            if (voice.isActive())
            {
                float voiceL, voiceR;
                voice.processSample(getSampleRate(), drawbars, leslieSpeed,
                                   leslieDepth, leslieMix, percussionClick,
                                   percussionDecay, voiceL, voiceR);
                outL += voiceL;
                outR += voiceR;
            }
        }

        // Write to output
        channelDataL[sample] = outL;
        if (channelDataR)
            channelDataR[sample] = outR;
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
