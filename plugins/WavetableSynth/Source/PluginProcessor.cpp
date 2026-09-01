#include "PluginProcessor.h"
#include "PluginEditor.h"

PluginProcessor::PluginProcessor()
    : AudioProcessor(BusesProperties()
                    .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , apvts(*this, nullptr, "Parameters", createParameterLayout())
    , presetManager("WavetableSynth", *this)
{
    // TODO: Load factory presets when ready
    // presetManager.setFactoryPresets({ ... });
}

PluginProcessor::~PluginProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Wavetable Position
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "wtPosition", "Wavetable Position",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));

    // Filter Controls
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "filterCutoff", "Filter Cutoff",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f), 1000.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "filterRes", "Filter Resonance",
        juce::NormalisableRange<float>(0.5f, 10.0f, 0.1f), 0.707f));

    layout.add(std::make_unique<juce::AudioParameterInt>(
        "filterType", "Filter Type",
        0, 3, 0)); // 0=LP, 1=HP, 2=BP, 3=Notch

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "filterEnv", "Filter Envelope Amount",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    // Amplitude Envelope
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "ampAttack", "Amp Attack",
        juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f, 0.3f), 0.01f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "ampDecay", "Amp Decay",
        juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f, 0.3f), 0.1f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "ampSustain", "Amp Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.8f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "ampRelease", "Amp Release",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f), 0.3f));

    // Filter Envelope
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "filtAttack", "Filter Attack",
        juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f, 0.3f), 0.05f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "filtDecay", "Filter Decay",
        juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f, 0.3f), 0.2f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "filtSustain", "Filter Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "filtRelease", "Filter Release",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f), 0.3f));

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
        else if (message.isAllNotesOff() || message.isAllSoundOff())
        {
            voiceManager.allNotesOff();
        }
    }

    // Get parameters
    float wtPosition = *apvts.getRawParameterValue("wtPosition");
    float filterCutoff = *apvts.getRawParameterValue("filterCutoff");
    float filterRes = *apvts.getRawParameterValue("filterRes");
    int filterType = static_cast<int>(*apvts.getRawParameterValue("filterType"));
    float filterEnv = *apvts.getRawParameterValue("filterEnv");

    float ampAttack = *apvts.getRawParameterValue("ampAttack");
    float ampDecay = *apvts.getRawParameterValue("ampDecay");
    float ampSustain = *apvts.getRawParameterValue("ampSustain");
    float ampRelease = *apvts.getRawParameterValue("ampRelease");

    float filtAttack = *apvts.getRawParameterValue("filtAttack");
    float filtDecay = *apvts.getRawParameterValue("filtDecay");
    float filtSustain = *apvts.getRawParameterValue("filtSustain");
    float filtRelease = *apvts.getRawParameterValue("filtRelease");

    // Update envelopes for all voices
    for (auto& voice : voiceManager.getVoices())
    {
        voice.updateEnvelopes(ampAttack, ampDecay, ampSustain, ampRelease,
                            filtAttack, filtDecay, filtSustain, filtRelease);
    }

    // Process voices
    auto* channelDataL = buffer.getWritePointer(0);
    auto* channelDataR = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float outputSample = 0.0f;

        // Sum all voices
        for (auto& voice : voiceManager.getVoices())
        {
            outputSample += voice.processSample(getSampleRate(),
                                              wtPosition,
                                              filterCutoff,
                                              filterRes,
                                              filterType,
                                              filterEnv);
        }

        // Write to output (mono -> stereo if needed)
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
