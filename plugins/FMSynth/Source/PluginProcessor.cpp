#include "PluginProcessor.h"
#include "PluginEditor.h"

static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterInt>("algorithm", "Algorithm", 0, 7, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("feedback", "Feedback", 0.0f, 1.0f, 0.0f));

    for (int op = 0; op < 4; ++op)
    {
        juce::String prefix = "op" + juce::String(op + 1);
        params.push_back(std::make_unique<juce::AudioParameterFloat>(prefix + "Level", "Op" + juce::String(op + 1) + " Level", 0.0f, 1.0f, (op < 2) ? 1.0f : 0.5f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(prefix + "Ratio", "Op" + juce::String(op + 1) + " Ratio", juce::NormalisableRange<float>(0.25f, 16.0f, 0.25f), (op == 0) ? 1.0f : static_cast<float>(op + 1)));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(prefix + "Attack", "Op" + juce::String(op + 1) + " A", juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f, 0.3f), 0.01f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(prefix + "Decay", "Op" + juce::String(op + 1) + " D", juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f, 0.3f), 0.2f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(prefix + "Sustain", "Op" + juce::String(op + 1) + " S", 0.0f, 1.0f, 0.7f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(prefix + "Release", "Op" + juce::String(op + 1) + " R", juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f), 0.5f));
    }

    return {params.begin(), params.end()};
}

FMProcessor::FMProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()),
      midiLearnManager_(apvts_),
      presetManager_(apvts_, "FMSynth") {

    // Scan for presets on startup
    presetManager_.scanPresets();

    for (int i = 0; i < 6; ++i)
        synth_.addVoice(new FMVoice());
    synth_.addSound(new FMSound());
}

FMProcessor::~FMProcessor() {}

void FMProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth_.setCurrentPlaybackSampleRate(sampleRate);
    for (int i = 0; i < synth_.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<FMVoice*>(synth_.getVoice(i)))
            voice->prepareToPlay(sampleRate, samplesPerBlock);
}

void FMProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // Process MIDI learn
    for (const auto metadata : midiMessages)
        midiLearnManager_.processMidiMessage(metadata.getMessage());

    buffer.clear();
    updateVoiceParameters();
    synth_.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void FMProcessor::updateVoiceParameters()
{
    int algo = apvts_.getRawParameterValue("algorithm")->load();
    float fb = apvts_.getRawParameterValue("feedback")->load();

    for (int i = 0; i < synth_.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<FMVoice*>(synth_.getVoice(i)))
        {
            voice->setAlgorithm(algo);
            voice->setFeedback(fb);

            for (int op = 0; op < 4; ++op)
            {
                juce::String prefix = "op" + juce::String(op + 1);
                voice->setOpLevel(op, apvts_.getRawParameterValue(prefix + "Level")->load());
                voice->setOpRatio(op, apvts_.getRawParameterValue(prefix + "Ratio")->load());
                voice->setOpAttack(op, apvts_.getRawParameterValue(prefix + "Attack")->load());
                voice->setOpDecay(op, apvts_.getRawParameterValue(prefix + "Decay")->load());
                voice->setOpSustain(op, apvts_.getRawParameterValue(prefix + "Sustain")->load());
                voice->setOpRelease(op, apvts_.getRawParameterValue(prefix + "Release")->load());
            }
        }
    }
}

juce::AudioProcessorEditor* FMProcessor::createEditor()
{
    return new FMEditor(*this);
}

void FMProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts_.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    xml->addChildElement(midiLearnManager_.saveToXml().release());
    xml->addChildElement(presetManager_.saveToXml().release());
    copyXmlToBinary(*xml, destData);
}

void FMProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(apvts_.state.getType())) {
        apvts_.replaceState(juce::ValueTree::fromXml(*xml));
        if (auto* midiMappingsXml = xml->getChildByName("MIDILearnMappings"))
            midiLearnManager_.loadFromXml(*midiMappingsXml);
        if (auto* presetXml = xml->getChildByName("PresetManagerState"))
            presetManager_.loadFromXml(*presetXml);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FMProcessor();
}
