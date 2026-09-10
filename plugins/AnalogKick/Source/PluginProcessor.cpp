#include "PluginProcessor.h"
#include "PluginEditor.h"

static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>("pitch", "Pitch", juce::NormalisableRange<float>(20.0f, 200.0f, 0.1f), 55.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("pitchEnvAmount", "Pitch Env", juce::NormalisableRange<float>(0.0f, 4.0f, 0.01f), 2.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("pitchEnvDecay", "Pitch Decay", juce::NormalisableRange<float>(0.001f, 0.2f, 0.001f, 0.3f), 0.05f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("decay", "Decay", juce::NormalisableRange<float>(0.01f, 2.0f, 0.001f, 0.3f), 0.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("click", "Click", 0.0f, 1.0f, 0.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("drive", "Drive", juce::NormalisableRange<float>(1.0f, 10.0f, 0.1f), 1.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("tone", "Tone", 0.0f, 1.0f, 0.5f));

    return {params.begin(), params.end()};
}

KickProcessor::KickProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    for (int i = 0; i < 4; ++i)
        synth_.addVoice(new KickVoice());
    synth_.addSound(new KickSound());
}

KickProcessor::~KickProcessor() {}

void KickProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth_.setCurrentPlaybackSampleRate(sampleRate);
    for (int i = 0; i < synth_.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<KickVoice*>(synth_.getVoice(i)))
            voice->prepareToPlay(sampleRate, samplesPerBlock);
}

void KickProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();
    updateVoiceParameters();
    synth_.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void KickProcessor::updateVoiceParameters()
{
    float pitch = apvts_.getRawParameterValue("pitch")->load();
    float pitchEnvAmount = apvts_.getRawParameterValue("pitchEnvAmount")->load();
    float pitchEnvDecay = apvts_.getRawParameterValue("pitchEnvDecay")->load();
    float decay = apvts_.getRawParameterValue("decay")->load();
    float click = apvts_.getRawParameterValue("click")->load();
    float drive = apvts_.getRawParameterValue("drive")->load();
    float tone = apvts_.getRawParameterValue("tone")->load();

    for (int i = 0; i < synth_.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<KickVoice*>(synth_.getVoice(i)))
        {
            voice->setPitch(pitch);
            voice->setPitchEnvAmount(pitchEnvAmount);
            voice->setPitchEnvDecay(pitchEnvDecay);
            voice->setDecay(decay);
            voice->setClick(click);
            voice->setDrive(drive);
            voice->setTone(tone);
        }
    }
}

juce::AudioProcessorEditor* KickProcessor::createEditor()
{
    return new KickEditor(*this);
}

void KickProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts_.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void KickProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(apvts_.state.getType()))
        apvts_.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new KickProcessor();
}
