#include "PluginProcessor.h"
#include "PluginEditor.h"

PolysynthProcessor::PolysynthProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "Parameters", createParameterLayout())
{
    for (int i = 0; i < 6; ++i) // 6-voice polyphony
        synth_.addVoice(new PolysynthVoice());
    synth_.addSound(new PolysynthSound());
}

PolysynthProcessor::~PolysynthProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout PolysynthProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_OSC_MIX, "Osc Mix", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_DETUNE, "Detune", -24.0f, 24.0f, -12.0f));
    params.push_back(std::make_unique<juce::AudioParameterInt>(PARAM_UNISON_VOICES, "Unison Voices", 1, 6, 1));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_UNISON_DETUNE, "Unison Detune", 0.0f, 50.0f, 10.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_FILTER_CUTOFF, "Filter Cutoff", 20.0f, 20000.0f, 2000.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_FILTER_RESONANCE, "Filter Resonance", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_FILTER_MIX, "LP/HP Mix", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_ATTACK, "Attack", 0.001f, 2.0f, 0.01f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_DECAY, "Decay", 0.001f, 2.0f, 0.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_SUSTAIN, "Sustain", 0.0f, 1.0f, 0.7f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_RELEASE, "Release", 0.001f, 5.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_CHORUS_DEPTH, "Chorus Depth", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_CHORUS_RATE, "Chorus Rate", 0.1f, 10.0f, 2.0f));

    return { params.begin(), params.end() };
}

void PolysynthProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth_.setCurrentPlaybackSampleRate(sampleRate);
    for (int i = 0; i < synth_.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<PolysynthVoice*>(synth_.getVoice(i)))
            voice->prepareToPlay(sampleRate, samplesPerBlock);
}

void PolysynthProcessor::releaseResources() {}

bool PolysynthProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void PolysynthProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    updateVoiceParameters();
    synth_.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void PolysynthProcessor::updateVoiceParameters()
{
    for (int i = 0; i < synth_.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<PolysynthVoice*>(synth_.getVoice(i)))
        {
            voice->setOscMix(*apvts_.getRawParameterValue(PARAM_OSC_MIX));
            voice->setDetune(*apvts_.getRawParameterValue(PARAM_DETUNE));
            voice->setUnisonVoices(*apvts_.getRawParameterValue(PARAM_UNISON_VOICES));
            voice->setUnisonDetune(*apvts_.getRawParameterValue(PARAM_UNISON_DETUNE));
            voice->setFilterCutoff(*apvts_.getRawParameterValue(PARAM_FILTER_CUTOFF));
            voice->setFilterResonance(*apvts_.getRawParameterValue(PARAM_FILTER_RESONANCE));
            voice->setFilterMix(*apvts_.getRawParameterValue(PARAM_FILTER_MIX));
            voice->setAttack(*apvts_.getRawParameterValue(PARAM_ATTACK));
            voice->setDecay(*apvts_.getRawParameterValue(PARAM_DECAY));
            voice->setSustain(*apvts_.getRawParameterValue(PARAM_SUSTAIN));
            voice->setRelease(*apvts_.getRawParameterValue(PARAM_RELEASE));
            voice->setChorusDepth(*apvts_.getRawParameterValue(PARAM_CHORUS_DEPTH));
            voice->setChorusRate(*apvts_.getRawParameterValue(PARAM_CHORUS_RATE));
        }
    }
}

bool PolysynthProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* PolysynthProcessor::createEditor() { return new PolysynthEditor(*this); }
const juce::String PolysynthProcessor::getName() const { return JucePlugin_Name; }
bool PolysynthProcessor::acceptsMidi() const { return true; }
bool PolysynthProcessor::producesMidi() const { return false; }
bool PolysynthProcessor::isMidiEffect() const { return false; }
double PolysynthProcessor::getTailLengthSeconds() const { return 5.0; }
int PolysynthProcessor::getNumPrograms() { return 1; }
int PolysynthProcessor::getCurrentProgram() { return 0; }
void PolysynthProcessor::setCurrentProgram(int) {}
const juce::String PolysynthProcessor::getProgramName(int) { return {}; }
void PolysynthProcessor::changeProgramName(int, const juce::String&) {}

void PolysynthProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts_.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PolysynthProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts_.state.getType()))
            apvts_.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PolysynthProcessor();
}
