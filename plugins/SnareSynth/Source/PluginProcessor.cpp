#include "PluginProcessor.h"
#include "PluginEditor.h"

static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>("tone", "Tone", juce::NormalisableRange<float>(100.0f, 400.0f, 1.0f), 180.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("snappy", "Snappy", 0.0f, 1.0f, 0.7f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("decay", "Decay", juce::NormalisableRange<float>(0.01f, 1.0f, 0.001f, 0.3f), 0.15f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("snareDecay", "Snare Decay", juce::NormalisableRange<float>(0.01f, 0.5f, 0.001f, 0.3f), 0.12f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("pitchEnv", "Pitch Env", juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("snap", "Snap", 0.0f, 1.0f, 0.5f));

    return {params.begin(), params.end()};
}

SnareProcessor::SnareProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()),
      midiLearnManager_(apvts_) {
    for (int i = 0; i < 4; ++i)
        synth_.addVoice(new SnareVoice());
    synth_.addSound(new SnareSound());
}

SnareProcessor::~SnareProcessor() {}

void SnareProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth_.setCurrentPlaybackSampleRate(sampleRate);
    for (int i = 0; i < synth_.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<SnareVoice*>(synth_.getVoice(i)))
            voice->prepareToPlay(sampleRate, samplesPerBlock);
}

void SnareProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    for (const auto metadata : midiMessages)
        midiLearnManager_.processMidiMessage(metadata.getMessage());
    buffer.clear();
    updateVoiceParameters();
    synth_.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void SnareProcessor::updateVoiceParameters()
{
    float tone = apvts_.getRawParameterValue("tone")->load();
    float snappy = apvts_.getRawParameterValue("snappy")->load();
    float decay = apvts_.getRawParameterValue("decay")->load();
    float snareDecay = apvts_.getRawParameterValue("snareDecay")->load();
    float pitchEnv = apvts_.getRawParameterValue("pitchEnv")->load();
    float snap = apvts_.getRawParameterValue("snap")->load();

    for (int i = 0; i < synth_.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SnareVoice*>(synth_.getVoice(i)))
        {
            voice->setTone(tone);
            voice->setSnappy(snappy);
            voice->setDecay(decay);
            voice->setSnareDecay(snareDecay);
            voice->setPitchEnv(pitchEnv);
            voice->setSnap(snap);
        }
    }
}

juce::AudioProcessorEditor* SnareProcessor::createEditor()
{
    return new SnareEditor(*this);
}

void SnareProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts_.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    xml->addChildElement(midiLearnManager_.saveToXml().release());
    copyXmlToBinary(*xml, destData);
}

void SnareProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(apvts_.state.getType()))
        apvts_.replaceState(juce::ValueTree::fromXml(*xml));
        if (auto* midiXml = xml->getChildByName("MIDILearnMappings"))
            midiLearnManager_.loadFromXml(*midiXml);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SnareProcessor();
}
