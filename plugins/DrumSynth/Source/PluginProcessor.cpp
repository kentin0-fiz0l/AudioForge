#include "PluginProcessor.h"
#include "PluginEditor.h"

PluginProcessor::PluginProcessor()
    : AudioProcessor(BusesProperties()
                    .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , apvts(*this, nullptr, "Parameters", createParameterLayout())
    , midiLearnManager_(apvts)
    , presetManager_(apvts, "DrumSynth")
{
    // Scan for presets on startup
    presetManager_.scanPresets();
}

PluginProcessor::~PluginProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Kick Parameters (5)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "kickPitch", "Kick Pitch",
        juce::NormalisableRange<float>(30.0f, 100.0f, 0.1f), 50.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "kickDecay", "Kick Decay",
        juce::NormalisableRange<float>(0.05f, 2.0f, 0.01f), 0.4f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "kickClick", "Kick Click",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.3f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "kickTone", "Kick Tone",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.8f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "kickDrive", "Kick Drive",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.2f));

    // Snare Parameters (5)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "snareTune", "Snare Tune",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "snareSnap", "Snare Snap",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.4f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "snareTone", "Snare Tone",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.6f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "snareDecay", "Snare Decay",
        juce::NormalisableRange<float>(0.05f, 1.0f, 0.01f), 0.2f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "snareMix", "Snare Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.7f));

    // Hi-Hat Parameters (4)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "hihatTune", "HiHat Tune",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "hihatDecay", "HiHat Decay",
        juce::NormalisableRange<float>(0.01f, 0.5f, 0.01f), 0.1f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "hihatTone", "HiHat Tone",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.7f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "hihatClick", "HiHat Click",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    // Clap parameters. Added last, so the parameters before them keep
    // their positions.
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "clapTone", "Clap Tone",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "clapDecay", "Clap Decay",
        juce::NormalisableRange<float>(0.05f, 0.6f, 0.01f), 0.2f));

    return layout;
}

void PluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Drums are ready to be triggered
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

// Output trim: puts the loudest single full-velocity note at the default settings at
// about -9 dBFS (it was -1.4). Recalibrate with StressHost --levels; see tools/stress-host/README.md.
static constexpr float outputTrim = 0.417f;  // -7.6 dB

void PluginProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // Clear output buffer
    buffer.clear();

    // Get parameters
    float kickPitch = *apvts.getRawParameterValue("kickPitch");
    float kickDecay = *apvts.getRawParameterValue("kickDecay");
    float kickClick = *apvts.getRawParameterValue("kickClick");
    float kickTone = *apvts.getRawParameterValue("kickTone");
    float kickDrive = *apvts.getRawParameterValue("kickDrive");

    float snareTune = *apvts.getRawParameterValue("snareTune");
    float snareSnap = *apvts.getRawParameterValue("snareSnap");
    float snareTone = *apvts.getRawParameterValue("snareTone");
    float snareDecay = *apvts.getRawParameterValue("snareDecay");
    float snareMix = *apvts.getRawParameterValue("snareMix");

    float hihatTune = *apvts.getRawParameterValue("hihatTune");
    float hihatDecay = *apvts.getRawParameterValue("hihatDecay");
    float hihatTone = *apvts.getRawParameterValue("hihatTone");
    float hihatClick = *apvts.getRawParameterValue("hihatClick");

    float clapTone = *apvts.getRawParameterValue("clapTone");
    float clapDecay = *apvts.getRawParameterValue("clapDecay");

    // Process audio
    auto* channelDataL = buffer.getWritePointer(0);
    auto* channelDataR = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;

    // Notes are played at their own positions in the block. Triggering
    // them all at its start put every hit up to a block early, which at
    // 512 samples is 10 ms of unsteady timing.
    auto nextNote = midiMessages.cbegin();
    const auto lastNote = midiMessages.cend();

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        for (; nextNote != lastNote && (*nextNote).samplePosition <= sample; ++nextNote)
            triggerDrum((*nextNote).getMessage());

        float outputSample = 0.0f;

        // Process each drum module
        outputSample += kickLevel * kickModule.processSample(getSampleRate(), kickPitch, kickDecay,
                                                             kickClick, kickTone, kickDrive);

        outputSample += snareLevel * snareModule.processSample(getSampleRate(), snareTune, snareSnap,
                                                               snareTone, snareDecay, snareMix);

        outputSample += hihatModule.processSample(getSampleRate(), hihatTune, hihatDecay,
                                                  hihatTone, hihatClick);

        outputSample += clapLevel * clapModule.processSample(getSampleRate(), clapTone, clapDecay);

        // Write to output (mono -> stereo if needed)
        channelDataL[sample] = outputSample;
        if (channelDataR)
            channelDataR[sample] = outputSample;
    }

    // A note placed past the end of the block is still played, a moment early
    for (; nextNote != lastNote; ++nextNote)
        triggerDrum((*nextNote).getMessage());

    buffer.applyGain(outputTrim);
}

void PluginProcessor::triggerDrum(const juce::MidiMessage& message)
{
    if (! message.isNoteOn())
        return;

    // A hit plays as hard as it is struck: half velocity, half the level
    const float level = message.getFloatVelocity();

    switch (drumForNote(message.getNoteNumber()))
    {
        case Drum::kick:      kickLevel = level;  kickModule.trigger(); break;
        case Drum::snare:     snareLevel = level; snareModule.trigger(); break;
        case Drum::closedHat: hihatModule.trigger(false, level); break;
        case Drum::openHat:   hihatModule.trigger(true, level); break;
        case Drum::clap:      clapLevel = level;  clapModule.trigger(); break;
        case Drum::none:      break;
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

#ifndef AUDIOFORGE_TESTS
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}
#endif
