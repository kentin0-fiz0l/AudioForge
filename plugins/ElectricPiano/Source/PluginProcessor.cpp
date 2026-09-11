#include "PluginProcessor.h"
#include "PluginEditor.h"

ElectricPianoProcessor::ElectricPianoProcessor()
    : AudioProcessor(BusesProperties()
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "Parameters", createParameterLayout()),
      midiLearnManager_(apvts_) {
      presetManager_(apvts_, "ElectricPiano") {

    // Scan for presets on startup
    presetManager_.scanPresets();
}
    // Add 16 voices for polyphony
    for (int i = 0; i < 16; ++i)
    {
        synth_.addVoice(new ElectricPianoVoice());
    }

    // Add the sound
    synth_.addSound(new ElectricPianoSound());
}

ElectricPianoProcessor::~ElectricPianoProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout ElectricPianoProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Model selection (0=Rhodes, 1=Wurlitzer, 2=Hybrid)
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        PARAM_MODEL, "Model", 0, 2, 0));

    // Tone controls
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_TONE, "Tone", 0.0f, 1.0f, 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_DECAY, "Decay", 0.0f, 1.0f, 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_MECHANICAL_NOISE, "Mechanical Noise", 0.0f, 1.0f, 0.2f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_VELOCITY_CURVE, "Velocity Curve", 0.0f, 1.0f, 0.5f));

    // Effects
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_TREMOLO_DEPTH, "Tremolo Depth", 0.0f, 1.0f, 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_TREMOLO_RATE, "Tremolo Rate", 0.1f, 20.0f, 5.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_CHORUS_DEPTH, "Chorus Depth", 0.0f, 1.0f, 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_REVERB_MIX, "Reverb Mix", 0.0f, 1.0f, 0.0f));

    return { params.begin(), params.end() };
}

void ElectricPianoProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth_.setCurrentPlaybackSampleRate(sampleRate);

    // Prepare all voices
    for (int i = 0; i < synth_.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<ElectricPianoVoice*>(synth_.getVoice(i)))
        {
            voice->prepareToPlay(sampleRate, samplesPerBlock);
        }
    }
}

void ElectricPianoProcessor::releaseResources()
{
}

bool ElectricPianoProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // Supports mono or stereo output
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

void ElectricPianoProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    for (const auto metadata : midiMessages)
        midiLearnManager_.processMidiMessage(metadata.getMessage());

    // Clear the buffer
    buffer.clear();

    // Update voice parameters
    updateVoiceParameters();

    // Render the synthesizer
    synth_.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void ElectricPianoProcessor::updateVoiceParameters()
{
    // Get parameter values
    int modelParam = *apvts_.getRawParameterValue(PARAM_MODEL);
    float tone = *apvts_.getRawParameterValue(PARAM_TONE);
    float decay = *apvts_.getRawParameterValue(PARAM_DECAY);
    float mechanicalNoise = *apvts_.getRawParameterValue(PARAM_MECHANICAL_NOISE);
    float velocityCurve = *apvts_.getRawParameterValue(PARAM_VELOCITY_CURVE);
    float tremoloDepth = *apvts_.getRawParameterValue(PARAM_TREMOLO_DEPTH);
    float tremoloRate = *apvts_.getRawParameterValue(PARAM_TREMOLO_RATE);
    float chorusDepth = *apvts_.getRawParameterValue(PARAM_CHORUS_DEPTH);
    float reverbMix = *apvts_.getRawParameterValue(PARAM_REVERB_MIX);

    // Convert model parameter to enum
    ElectricPianoEngine::EPModel model;
    switch (modelParam)
    {
        case 0: model = ElectricPianoEngine::EPModel::Rhodes; break;
        case 1: model = ElectricPianoEngine::EPModel::Wurlitzer; break;
        case 2: model = ElectricPianoEngine::EPModel::Hybrid; break;
        default: model = ElectricPianoEngine::EPModel::Rhodes; break;
    }

    // Update all voices
    for (int i = 0; i < synth_.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<ElectricPianoVoice*>(synth_.getVoice(i)))
        {
            voice->setModel(model);
            voice->setTone(tone);
            voice->setDecay(decay);
            voice->setMechanicalNoise(mechanicalNoise);
            voice->setVelocityCurve(velocityCurve);
            voice->setTremoloDepth(tremoloDepth);
            voice->setTremoloRate(tremoloRate);
            voice->setChorusDepth(chorusDepth);
            voice->setReverbMix(reverbMix);
        }
    }
}

bool ElectricPianoProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* ElectricPianoProcessor::createEditor()
{
    return new ElectricPianoEditor(*this);
}

const juce::String ElectricPianoProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ElectricPianoProcessor::acceptsMidi() const
{
    return true;
}

bool ElectricPianoProcessor::producesMidi() const
{
    return false;
}

bool ElectricPianoProcessor::isMidiEffect() const
{
    return false;
}

double ElectricPianoProcessor::getTailLengthSeconds() const
{
    return 2.0; // 2 second tail for reverb/decay
}

int ElectricPianoProcessor::getNumPrograms()
{
    return 1;
}

int ElectricPianoProcessor::getCurrentProgram()
{
    return 0;
}

void ElectricPianoProcessor::setCurrentProgram(int)
{
}

const juce::String ElectricPianoProcessor::getProgramName(int)
{
    return {};
}

void ElectricPianoProcessor::changeProgramName(int, const juce::String&)
{
}

void ElectricPianoProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts_.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    xml->addChildElement(midiLearnManager_.saveToXml().release());
    xml->addChildElement(presetManager_.saveToXml().release());
    copyXmlToBinary(*xml, destData);
}

void ElectricPianoProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts_.state.getType()))
            apvts_.replaceState(juce::ValueTree::fromXml(*xmlState));
        if (auto* midiXml = xmlState->getChildByName("MIDILearnMappings"))
            midiLearnManager_.loadFromXml(*midiXml);
        if (auto* presetXml = xml->getChildByName("PresetManagerState"))
            presetManager_.loadFromXml(*presetXml);
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ElectricPianoProcessor();
}
