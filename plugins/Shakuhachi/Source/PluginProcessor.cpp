#include "PluginProcessor.h"
#include "PluginEditor.h"

ShakuhachiProcessor::ShakuhachiProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "Parameters", createParameterLayout()),
      midiLearnManager_(apvts_) {
      presetManager_(apvts_, "Shakuhachi") {

    // Scan for presets on startup
    presetManager_.scanPresets();
}
    for (int i = 0; i < 4; ++i)
        synth_.addVoice(new ShakuhachiVoice());

    synth_.addSound(new ShakuhachiSound());
}

ShakuhachiProcessor::~ShakuhachiProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout ShakuhachiProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_BREATH_PRESSURE, "Breath Pressure", 0.0f, 1.0f, 0.7f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_EMBOUCHURE, "Embouchure (Meri/Kari)", -1.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_AIR_NOISE, "Air Noise", 0.0f, 1.0f, 0.4f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_ATTACK, "Attack", 0.01f, 0.5f, 0.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_RELEASE, "Release", 0.1f, 2.0f, 0.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_VIBRATO_RATE, "Vibrato Rate", 0.0f, 10.0f, 5.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_VIBRATO_DEPTH, "Vibrato Depth", 0.0f, 0.05f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_BRIGHTNESS, "Brightness", 0.0f, 1.0f, 0.5f));

    return { params.begin(), params.end() };
}

void ShakuhachiProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth_.setCurrentPlaybackSampleRate(sampleRate);

    for (int i = 0; i < synth_.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<ShakuhachiVoice*>(synth_.getVoice(i)))
            voice->prepareToPlay(sampleRate, samplesPerBlock);
    }
}

void ShakuhachiProcessor::releaseResources() {}

bool ShakuhachiProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void ShakuhachiProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    for (const auto metadata : midiMessages)
        midiLearnManager_.processMidiMessage(metadata.getMessage());
    buffer.clear();

    updateVoiceParameters();
    synth_.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void ShakuhachiProcessor::updateVoiceParameters()
{
    float breathPressure = *apvts_.getRawParameterValue(PARAM_BREATH_PRESSURE);
    float embouchure = *apvts_.getRawParameterValue(PARAM_EMBOUCHURE);
    float airNoise = *apvts_.getRawParameterValue(PARAM_AIR_NOISE);
    float attack = *apvts_.getRawParameterValue(PARAM_ATTACK);
    float release = *apvts_.getRawParameterValue(PARAM_RELEASE);
    float vibratoRate = *apvts_.getRawParameterValue(PARAM_VIBRATO_RATE);
    float vibratoDepth = *apvts_.getRawParameterValue(PARAM_VIBRATO_DEPTH);
    float brightness = *apvts_.getRawParameterValue(PARAM_BRIGHTNESS);

    for (int i = 0; i < synth_.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<ShakuhachiVoice*>(synth_.getVoice(i)))
        {
            voice->setBreathPressure(breathPressure);
            voice->setEmbouchure(embouchure);
            voice->setAirNoise(airNoise);
            voice->setAttack(attack);
            voice->setRelease(release);
            voice->setVibratoRate(vibratoRate);
            voice->setVibratoDepth(vibratoDepth);
            voice->setBrightness(brightness);
        }
    }
}

bool ShakuhachiProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* ShakuhachiProcessor::createEditor() { return new ShakuhachiEditor(*this); }
const juce::String ShakuhachiProcessor::getName() const { return JucePlugin_Name; }
bool ShakuhachiProcessor::acceptsMidi() const { return true; }
bool ShakuhachiProcessor::producesMidi() const { return false; }
bool ShakuhachiProcessor::isMidiEffect() const { return false; }
double ShakuhachiProcessor::getTailLengthSeconds() const { return 2.0; }
int ShakuhachiProcessor::getNumPrograms() { return 1; }
int ShakuhachiProcessor::getCurrentProgram() { return 0; }
void ShakuhachiProcessor::setCurrentProgram(int) {}
const juce::String ShakuhachiProcessor::getProgramName(int) { return {}; }
void ShakuhachiProcessor::changeProgramName(int, const juce::String&) {}

void ShakuhachiProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts_.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    xml->addChildElement(midiLearnManager_.saveToXml().release());
    xml->addChildElement(presetManager_.saveToXml().release());
    copyXmlToBinary(*xml, destData);
}

void ShakuhachiProcessor::setStateInformation(const void* data, int sizeInBytes)
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
    return new ShakuhachiProcessor();
}
