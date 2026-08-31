#include "PluginProcessor.h"
#include "PluginEditor.h"

// ============================================================================
// Voice Implementation
// ============================================================================

Voice::Voice()
{
}

void Voice::noteOn(int midiNote, float velocity_, double sampleRate)
{
    currentNote = midiNote;
    velocity = velocity_;
    active = true;
    level = velocity;

    // TODO: Trigger envelopes, set oscillator frequency, etc.
}

void Voice::noteOff()
{
    // TODO: Trigger envelope release
    active = false; // For now, just stop immediately
}

void Voice::reset()
{
    active = false;
    currentNote = -1;
    velocity = 0.0f;
    level = 0.0f;
}

float Voice::processSample(double sampleRate)
{
    if (!active)
        return 0.0f;

    // TODO: Generate synthesis output
    // Example: return oscillator.processSample() * envelope.getNextValue();
    return 0.0f;
}

// ============================================================================
// Processor Implementation
// ============================================================================

PluginProcessor::PluginProcessor()
    : AudioProcessor(BusesProperties()
                    .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , apvts(*this, nullptr, "Parameters", {})
    , presetManager("FMSynth", *this)
{
    createParameters();

    // TODO: Load factory presets
    // presetManager.setFactoryPresets({ ... });
    presetManager.scanPresets();
}

PluginProcessor::~PluginProcessor()
{
}

void PluginProcessor::createParameters()
{
    // TODO: Define parameters
    // Example:
    // apvts.createAndAddParameter("cutoff", "Cutoff", ..., defaultValue);
}

void PluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Prepare voices for playback
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

    // Process voices
    auto* channelDataL = buffer.getWritePointer(0);
    auto* channelDataR = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float outputSample = 0.0f;

        // Sum all voices
        for (auto& voice : voiceManager.getVoices())
        {
            outputSample += voice.processSample(getSampleRate());
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
