#include "PluginProcessor.h"
#include "PluginEditor.h"

SitarProcessor::SitarProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "Parameters", createParameterLayout()),
      midiLearnManager_(apvts_) {
    for (int i = 0; i < 6; ++i)
        synth_.addVoice(new SitarVoice());

    synth_.addSound(new SitarSound());
}

SitarProcessor::~SitarProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout SitarProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterInt>(PARAM_RAGA_SCALE, "Raga Scale", 0, 3, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_JAWARI, "Jawari", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_SYMPATHETIC, "Sympathetic", 0.0f, 1.0f, 0.6f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_ATTACK, "Attack", 0.001f, 0.1f, 0.01f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_DECAY, "Decay", 0.1f, 2.0f, 0.7f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_PITCH_BEND, "Pitch Bend", -2.0f, 2.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_VIBRATO_RATE, "Vibrato Rate", 0.0f, 10.0f, 5.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_VIBRATO_DEPTH, "Vibrato Depth", 0.0f, 0.05f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_DRONE_LEVEL, "Drone Level", 0.0f, 1.0f, 0.2f));

    return { params.begin(), params.end() };
}

void SitarProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth_.setCurrentPlaybackSampleRate(sampleRate);

    for (int i = 0; i < synth_.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SitarVoice*>(synth_.getVoice(i)))
            voice->prepareToPlay(sampleRate, samplesPerBlock);
    }
}

void SitarProcessor::releaseResources() {}

bool SitarProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void SitarProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    for (const auto metadata : midiMessages)
        midiLearnManager_.processMidiMessage(metadata.getMessage());
    buffer.clear();

    updateVoiceParameters();
    synth_.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void SitarProcessor::updateVoiceParameters()
{
    int scaleParam = *apvts_.getRawParameterValue(PARAM_RAGA_SCALE);
    float jawari = *apvts_.getRawParameterValue(PARAM_JAWARI);
    float sympathetic = *apvts_.getRawParameterValue(PARAM_SYMPATHETIC);
    float attack = *apvts_.getRawParameterValue(PARAM_ATTACK);
    float decay = *apvts_.getRawParameterValue(PARAM_DECAY);
    float pitchBend = *apvts_.getRawParameterValue(PARAM_PITCH_BEND);
    float vibratoRate = *apvts_.getRawParameterValue(PARAM_VIBRATO_RATE);
    float vibratoDepth = *apvts_.getRawParameterValue(PARAM_VIBRATO_DEPTH);
    float droneLevel = *apvts_.getRawParameterValue(PARAM_DRONE_LEVEL);

    SitarEngine::RagaScale scale;
    switch (scaleParam)
    {
        case 0: scale = SitarEngine::RagaScale::Chromatic; break;
        case 1: scale = SitarEngine::RagaScale::Bhairavi; break;
        case 2: scale = SitarEngine::RagaScale::Yaman; break;
        case 3: scale = SitarEngine::RagaScale::Kafi; break;
        default: scale = SitarEngine::RagaScale::Chromatic; break;
    }

    for (int i = 0; i < synth_.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SitarVoice*>(synth_.getVoice(i)))
        {
            voice->setRagaScale(scale);
            voice->setJawariAmount(jawari);
            voice->setSympatheticResonance(sympathetic);
            voice->setAttack(attack);
            voice->setDecay(decay);
            voice->setPitchBend(pitchBend);
            voice->setVibratoRate(vibratoRate);
            voice->setVibratoDepth(vibratoDepth);
            voice->setDroneLevel(droneLevel);
        }
    }
}

bool SitarProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* SitarProcessor::createEditor() { return new SitarEditor(*this); }
const juce::String SitarProcessor::getName() const { return JucePlugin_Name; }
bool SitarProcessor::acceptsMidi() const { return true; }
bool SitarProcessor::producesMidi() const { return false; }
bool SitarProcessor::isMidiEffect() const { return false; }
double SitarProcessor::getTailLengthSeconds() const { return 5.0; }
int SitarProcessor::getNumPrograms() { return 1; }
int SitarProcessor::getCurrentProgram() { return 0; }
void SitarProcessor::setCurrentProgram(int) {}
const juce::String SitarProcessor::getProgramName(int) { return {}; }
void SitarProcessor::changeProgramName(int, const juce::String&) {}

void SitarProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts_.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    xml->addChildElement(midiLearnManager_.saveToXml().release());
    copyXmlToBinary(*xml, destData);
}

void SitarProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts_.state.getType()))
            apvts_.replaceState(juce::ValueTree::fromXml(*xmlState));
        if (auto* midiXml = xmlState->getChildByName("MIDILearnMappings"))
            midiLearnManager_.loadFromXml(*midiXml);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SitarProcessor();
}
