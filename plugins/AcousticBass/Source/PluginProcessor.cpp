#include "PluginProcessor.h"
#include "PluginEditor.h"

AcousticBassProcessor::AcousticBassProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "Parameters", createParameterLayout())
{
    for (int i = 0; i < 8; ++i)
        synth_.addVoice(new BassVoice());

    synth_.addSound(new BassSound());
}

AcousticBassProcessor::~AcousticBassProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout AcousticBassProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterInt>(PARAM_BASS_TYPE, "Bass Type", 0, 1, 1));
    params.push_back(std::make_unique<juce::AudioParameterInt>(PARAM_PLAY_STYLE, "Play Style", 0, 2, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_TONE, "Tone", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_ATTACK, "Attack", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_DECAY, "Decay", 0.0f, 1.0f, 0.7f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_FRET_NOISE, "Fret Noise", 0.0f, 1.0f, 0.1f));

    return { params.begin(), params.end() };
}

void AcousticBassProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth_.setCurrentPlaybackSampleRate(sampleRate);

    for (int i = 0; i < synth_.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<BassVoice*>(synth_.getVoice(i)))
            voice->prepareToPlay(sampleRate, samplesPerBlock);
    }
}

void AcousticBassProcessor::releaseResources() {}

bool AcousticBassProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void AcousticBassProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    updateVoiceParameters();
    synth_.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void AcousticBassProcessor::updateVoiceParameters()
{
    int bassTypeParam = *apvts_.getRawParameterValue(PARAM_BASS_TYPE);
    int playStyleParam = *apvts_.getRawParameterValue(PARAM_PLAY_STYLE);
    float tone = *apvts_.getRawParameterValue(PARAM_TONE);
    float attack = *apvts_.getRawParameterValue(PARAM_ATTACK);
    float decay = *apvts_.getRawParameterValue(PARAM_DECAY);
    float fretNoise = *apvts_.getRawParameterValue(PARAM_FRET_NOISE);

    BassEngine::BassType bassType = bassTypeParam == 0 ? BassEngine::BassType::Upright : BassEngine::BassType::Electric;
    BassEngine::PlayStyle playStyle;
    switch (playStyleParam)
    {
        case 0: playStyle = BassEngine::PlayStyle::Fingered; break;
        case 1: playStyle = BassEngine::PlayStyle::Picked; break;
        case 2: playStyle = BassEngine::PlayStyle::Slapped; break;
        default: playStyle = BassEngine::PlayStyle::Fingered; break;
    }

    for (int i = 0; i < synth_.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<BassVoice*>(synth_.getVoice(i)))
        {
            voice->setBassType(bassType);
            voice->setPlayStyle(playStyle);
            voice->setTone(tone);
            voice->setAttack(attack);
            voice->setDecay(decay);
            voice->setFretNoise(fretNoise);
        }
    }
}

bool AcousticBassProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* AcousticBassProcessor::createEditor() { return new AcousticBassEditor(*this); }
const juce::String AcousticBassProcessor::getName() const { return JucePlugin_Name; }
bool AcousticBassProcessor::acceptsMidi() const { return true; }
bool AcousticBassProcessor::producesMidi() const { return false; }
bool AcousticBassProcessor::isMidiEffect() const { return false; }
double AcousticBassProcessor::getTailLengthSeconds() const { return 3.0; }
int AcousticBassProcessor::getNumPrograms() { return 1; }
int AcousticBassProcessor::getCurrentProgram() { return 0; }
void AcousticBassProcessor::setCurrentProgram(int) {}
const juce::String AcousticBassProcessor::getProgramName(int) { return {}; }
void AcousticBassProcessor::changeProgramName(int, const juce::String&) {}

void AcousticBassProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts_.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void AcousticBassProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts_.state.getType()))
            apvts_.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AcousticBassProcessor();
}
