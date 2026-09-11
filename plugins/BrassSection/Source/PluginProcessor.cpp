#include "PluginProcessor.h"
#include "PluginEditor.h"

BrassSectionProcessor::BrassSectionProcessor()
    : AudioProcessor(BusesProperties()
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "Parameters", createParameterLayout()),
      midiLearnManager_(apvts_) {
      presetManager_(apvts_, "BrassSection") {

    // Scan for presets on startup
    presetManager_.scanPresets();
}
    // Add 12 voices for polyphony
    for (int i = 0; i < 12; ++i)
    {
        synth_.addVoice(new BrassVoice());
    }

    synth_.addSound(new BrassSound());
}

BrassSectionProcessor::~BrassSectionProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout BrassSectionProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Instrument type (0=Trumpet, 1=Trombone, 2=Saxophone)
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        PARAM_INSTRUMENT, "Instrument", 0, 2, 0));

    // Articulation (0=Sustain, 1=Staccato, 2=Marcato, 3=Fall-off)
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        PARAM_ARTICULATION, "Articulation", 0, 3, 0));

    // Tone controls
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_BRIGHTNESS, "Brightness", 0.0f, 1.0f, 0.7f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_BREATH_NOISE, "Breath Noise", 0.0f, 1.0f, 0.2f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_VIBRATO_DEPTH, "Vibrato Depth", 0.0f, 1.0f, 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_VIBRATO_RATE, "Vibrato Rate", 0.0f, 1.0f, 0.5f));

    // Section size (1-4 voices)
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        PARAM_SECTION_SIZE, "Section Size", 1, 4, 1));

    return { params.begin(), params.end() };
}

void BrassSectionProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth_.setCurrentPlaybackSampleRate(sampleRate);

    for (int i = 0; i < synth_.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<BrassVoice*>(synth_.getVoice(i)))
        {
            voice->prepareToPlay(sampleRate, samplesPerBlock);
        }
    }
}

void BrassSectionProcessor::releaseResources()
{
}

bool BrassSectionProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

void BrassSectionProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    for (const auto metadata : midiMessages)
        midiLearnManager_.processMidiMessage(metadata.getMessage());
    buffer.clear();

    updateVoiceParameters();
    synth_.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void BrassSectionProcessor::updateVoiceParameters()
{
    int instrumentParam = *apvts_.getRawParameterValue(PARAM_INSTRUMENT);
    int articulationParam = *apvts_.getRawParameterValue(PARAM_ARTICULATION);
    float brightness = *apvts_.getRawParameterValue(PARAM_BRIGHTNESS);
    float breathNoise = *apvts_.getRawParameterValue(PARAM_BREATH_NOISE);
    float vibratoDepth = *apvts_.getRawParameterValue(PARAM_VIBRATO_DEPTH);
    float vibratoRate = *apvts_.getRawParameterValue(PARAM_VIBRATO_RATE);
    int sectionSize = *apvts_.getRawParameterValue(PARAM_SECTION_SIZE);

    BrassEngine::InstrumentType instrument;
    switch (instrumentParam)
    {
        case 0: instrument = BrassEngine::InstrumentType::Trumpet; break;
        case 1: instrument = BrassEngine::InstrumentType::Trombone; break;
        case 2: instrument = BrassEngine::InstrumentType::Saxophone; break;
        default: instrument = BrassEngine::InstrumentType::Trumpet; break;
    }

    BrassEngine::Articulation articulation;
    switch (articulationParam)
    {
        case 0: articulation = BrassEngine::Articulation::Sustain; break;
        case 1: articulation = BrassEngine::Articulation::Staccato; break;
        case 2: articulation = BrassEngine::Articulation::Marcato; break;
        case 3: articulation = BrassEngine::Articulation::FallOff; break;
        default: articulation = BrassEngine::Articulation::Sustain; break;
    }

    for (int i = 0; i < synth_.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<BrassVoice*>(synth_.getVoice(i)))
        {
            voice->setInstrumentType(instrument);
            voice->setArticulation(articulation);
            voice->setBrightness(brightness);
            voice->setBreathNoise(breathNoise);
            voice->setVibratoDepth(vibratoDepth);
            voice->setVibratoRate(vibratoRate);
            voice->setSectionSize(sectionSize);
        }
    }
}

bool BrassSectionProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* BrassSectionProcessor::createEditor()
{
    return new BrassSectionEditor(*this);
}

const juce::String BrassSectionProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BrassSectionProcessor::acceptsMidi() const { return true; }
bool BrassSectionProcessor::producesMidi() const { return false; }
bool BrassSectionProcessor::isMidiEffect() const { return false; }
double BrassSectionProcessor::getTailLengthSeconds() const { return 2.0; }
int BrassSectionProcessor::getNumPrograms() { return 1; }
int BrassSectionProcessor::getCurrentProgram() { return 0; }
void BrassSectionProcessor::setCurrentProgram(int) {}
const juce::String BrassSectionProcessor::getProgramName(int) { return {}; }
void BrassSectionProcessor::changeProgramName(int, const juce::String&) {}

void BrassSectionProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts_.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    xml->addChildElement(midiLearnManager_.saveToXml().release());
    xml->addChildElement(presetManager_.saveToXml().release());
    copyXmlToBinary(*xml, destData);
}

void BrassSectionProcessor::setStateInformation(const void* data, int sizeInBytes)
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
    return new BrassSectionProcessor();
}
