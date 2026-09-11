#include "PluginProcessor.h"
#include "PluginEditor.h"

StringEnsembleProcessor::StringEnsembleProcessor()
    : AudioProcessor(BusesProperties()
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "Parameters", createParameterLayout()),
      midiLearnManager_(apvts_) {
      presetManager_(apvts_, "StringEnsemble") {

    // Scan for presets on startup
    presetManager_.scanPresets();
}
    for (int i = 0; i < 12; ++i)
        synth_.addVoice(new StringVoice());

    synth_.addSound(new StringSound());
}

StringEnsembleProcessor::~StringEnsembleProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout StringEnsembleProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        PARAM_STRING_TYPE, "String Type", 0, 3, 0));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_BRIGHTNESS, "Brightness", 0.0f, 1.0f, 0.7f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_ATTACK, "Attack", 0.0f, 1.0f, 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_DECAY, "Decay", 0.0f, 1.0f, 0.7f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_VIBRATO_DEPTH, "Vibrato Depth", 0.0f, 1.0f, 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_VIBRATO_RATE, "Vibrato Rate", 0.0f, 1.0f, 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        PARAM_ENSEMBLE_SIZE, "Ensemble Size", 1, 4, 1));

    return { params.begin(), params.end() };
}

void StringEnsembleProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth_.setCurrentPlaybackSampleRate(sampleRate);

    for (int i = 0; i < synth_.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<StringVoice*>(synth_.getVoice(i)))
            voice->prepareToPlay(sampleRate, samplesPerBlock);
    }
}

void StringEnsembleProcessor::releaseResources() {}

bool StringEnsembleProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void StringEnsembleProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    for (const auto metadata : midiMessages)
        midiLearnManager_.processMidiMessage(metadata.getMessage());
    buffer.clear();

    updateVoiceParameters();
    synth_.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void StringEnsembleProcessor::updateVoiceParameters()
{
    int stringTypeParam = *apvts_.getRawParameterValue(PARAM_STRING_TYPE);
    float brightness = *apvts_.getRawParameterValue(PARAM_BRIGHTNESS);
    float attack = *apvts_.getRawParameterValue(PARAM_ATTACK);
    float decay = *apvts_.getRawParameterValue(PARAM_DECAY);
    float vibratoDepth = *apvts_.getRawParameterValue(PARAM_VIBRATO_DEPTH);
    float vibratoRate = *apvts_.getRawParameterValue(PARAM_VIBRATO_RATE);
    int ensembleSize = *apvts_.getRawParameterValue(PARAM_ENSEMBLE_SIZE);

    StringEngine::StringType stringType;
    switch (stringTypeParam)
    {
        case 0: stringType = StringEngine::StringType::Violin; break;
        case 1: stringType = StringEngine::StringType::Viola; break;
        case 2: stringType = StringEngine::StringType::Cello; break;
        case 3: stringType = StringEngine::StringType::Bass; break;
        default: stringType = StringEngine::StringType::Violin; break;
    }

    for (int i = 0; i < synth_.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<StringVoice*>(synth_.getVoice(i)))
        {
            voice->setStringType(stringType);
            voice->setBrightness(brightness);
            voice->setAttack(attack);
            voice->setDecay(decay);
            voice->setVibratoDepth(vibratoDepth);
            voice->setVibratoRate(vibratoRate);
            voice->setEnsembleSize(ensembleSize);
        }
    }
}

bool StringEnsembleProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* StringEnsembleProcessor::createEditor()
{
    return new StringEnsembleEditor(*this);
}

const juce::String StringEnsembleProcessor::getName() const { return JucePlugin_Name; }
bool StringEnsembleProcessor::acceptsMidi() const { return true; }
bool StringEnsembleProcessor::producesMidi() const { return false; }
bool StringEnsembleProcessor::isMidiEffect() const { return false; }
double StringEnsembleProcessor::getTailLengthSeconds() const { return 3.0; }
int StringEnsembleProcessor::getNumPrograms() { return 1; }
int StringEnsembleProcessor::getCurrentProgram() { return 0; }
void StringEnsembleProcessor::setCurrentProgram(int) {}
const juce::String StringEnsembleProcessor::getProgramName(int) { return {}; }
void StringEnsembleProcessor::changeProgramName(int, const juce::String&) {}

void StringEnsembleProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts_.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    xml->addChildElement(midiLearnManager_.saveToXml().release());
    xml->addChildElement(presetManager_.saveToXml().release());
    copyXmlToBinary(*xml, destData);
}

void StringEnsembleProcessor::setStateInformation(const void* data, int sizeInBytes)
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
    return new StringEnsembleProcessor();
}
