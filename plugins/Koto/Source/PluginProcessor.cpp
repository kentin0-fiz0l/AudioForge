#include "PluginProcessor.h"
#include "PluginEditor.h"

KotoProcessor::KotoProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "Parameters", createParameterLayout()),
      midiLearnManager_(apvts_),
      presetManager_(apvts_, "Koto") {

    // Scan for presets on startup
    presetManager_.scanPresets();
    for (int i = 0; i < 8; ++i)
        synth_.addVoice(new KotoVoice());

    synth_.addSound(new KotoSound());
}

KotoProcessor::~KotoProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout KotoProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterInt>(PARAM_SCALE, "Scale", 0, 4, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(PARAM_PLAY_STYLE, "Play Style", 0, 2, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_BODY_RESONANCE, "Body Resonance", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_ATTACK, "Attack", 0.001f, 0.1f, 0.005f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_DECAY, "Decay", 0.1f, 2.0f, 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_PITCH_BEND, "Pitch Bend", -2.0f, 2.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_TREMOLO_RATE, "Tremolo Rate", 2.0f, 20.0f, 10.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_TONE, "Tone", 0.0f, 1.0f, 0.6f));

    return { params.begin(), params.end() };
}

void KotoProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth_.setCurrentPlaybackSampleRate(sampleRate);

    for (int i = 0; i < synth_.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<KotoVoice*>(synth_.getVoice(i)))
            voice->prepareToPlay(sampleRate, samplesPerBlock);
    }
}

void KotoProcessor::releaseResources() {}

bool KotoProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void KotoProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    for (const auto metadata : midiMessages)
        midiLearnManager_.processMidiMessage(metadata.getMessage());
    buffer.clear();

    updateVoiceParameters();
    synth_.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void KotoProcessor::updateVoiceParameters()
{
    int scaleParam = *apvts_.getRawParameterValue(PARAM_SCALE);
    int playStyleParam = *apvts_.getRawParameterValue(PARAM_PLAY_STYLE);
    float bodyResonance = *apvts_.getRawParameterValue(PARAM_BODY_RESONANCE);
    float attack = *apvts_.getRawParameterValue(PARAM_ATTACK);
    float decay = *apvts_.getRawParameterValue(PARAM_DECAY);
    float pitchBend = *apvts_.getRawParameterValue(PARAM_PITCH_BEND);
    float tremoloRate = *apvts_.getRawParameterValue(PARAM_TREMOLO_RATE);
    float tone = *apvts_.getRawParameterValue(PARAM_TONE);

    KotoEngine::Scale scale;
    switch (scaleParam)
    {
        case 0: scale = KotoEngine::Scale::InSen; break;
        case 1: scale = KotoEngine::Scale::Hirajoshi; break;
        case 2: scale = KotoEngine::Scale::Iwato; break;
        case 3: scale = KotoEngine::Scale::Kumoi; break;
        case 4: scale = KotoEngine::Scale::Chromatic; break;
        default: scale = KotoEngine::Scale::InSen; break;
    }

    KotoEngine::PlayStyle playStyle;
    switch (playStyleParam)
    {
        case 0: playStyle = KotoEngine::PlayStyle::Pluck; break;
        case 1: playStyle = KotoEngine::PlayStyle::Tremolo; break;
        case 2: playStyle = KotoEngine::PlayStyle::Scrape; break;
        default: playStyle = KotoEngine::PlayStyle::Pluck; break;
    }

    for (int i = 0; i < synth_.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<KotoVoice*>(synth_.getVoice(i)))
        {
            voice->setScale(scale);
            voice->setPlayStyle(playStyle);
            voice->setBodyResonance(bodyResonance);
            voice->setAttack(attack);
            voice->setDecay(decay);
            voice->setPitchBend(pitchBend);
            voice->setTremoloRate(tremoloRate);
            voice->setTone(tone);
        }
    }
}

bool KotoProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* KotoProcessor::createEditor() { return new KotoEditor(*this); }
const juce::String KotoProcessor::getName() const { return JucePlugin_Name; }
bool KotoProcessor::acceptsMidi() const { return true; }
bool KotoProcessor::producesMidi() const { return false; }
bool KotoProcessor::isMidiEffect() const { return false; }
double KotoProcessor::getTailLengthSeconds() const { return 3.0; }
int KotoProcessor::getNumPrograms() { return 1; }
int KotoProcessor::getCurrentProgram() { return 0; }
void KotoProcessor::setCurrentProgram(int) {}
const juce::String KotoProcessor::getProgramName(int) { return {}; }
void KotoProcessor::changeProgramName(int, const juce::String&) {}

void KotoProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts_.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    xml->addChildElement(midiLearnManager_.saveToXml().release());
    xml->addChildElement(presetManager_.saveToXml().release());
    copyXmlToBinary(*xml, destData);
}

void KotoProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts_.state.getType()))
            apvts_.replaceState(juce::ValueTree::fromXml(*xmlState));
        if (auto* midiXml = xmlState->getChildByName("MIDILearnMappings"))
            midiLearnManager_.loadFromXml(*midiXml);
        if (auto* presetXml = xmlState->getChildByName("PresetManagerState"))
            presetManager_.loadFromXml(*presetXml);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new KotoProcessor();
}
