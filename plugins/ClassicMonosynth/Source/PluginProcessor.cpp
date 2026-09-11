#include "PluginProcessor.h"
#include "PluginEditor.h"

ClassicMonosynthProcessor::ClassicMonosynthProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "Parameters", createParameterLayout()),
      midiLearnManager_(apvts_) {
      presetManager_(apvts_, "ClassicMonosynth") {

    // Scan for presets on startup
    presetManager_.scanPresets();
}
    // Monosynth = only 1 voice!
    synth_.addVoice(new MonosynthVoice());
    synth_.addSound(new MonosynthSound());
}

ClassicMonosynthProcessor::~ClassicMonosynthProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout ClassicMonosynthProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Oscillators
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_OSC1_LEVEL, "Osc 1 Level", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_OSC2_LEVEL, "Osc 2 Level", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_OSC3_LEVEL, "Osc 3 Level", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_OSC2_DETUNE, "Osc 2 Detune", -24.0f, 24.0f, -12.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_OSC3_DETUNE, "Osc 3 Detune", -24.0f, 24.0f, 12.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_PULSE_WIDTH, "Pulse Width", 0.01f, 0.99f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterBool>(PARAM_OSC_SYNC, "Osc Sync", false));

    // Filter
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_FILTER_CUTOFF, "Filter Cutoff", 20.0f, 20000.0f, 1000.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_FILTER_RESONANCE, "Filter Resonance", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_FILTER_ENV_AMOUNT, "Filter Env Amount", -5000.0f, 5000.0f, 2000.0f));

    // Filter Envelope
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_FILTER_ATTACK, "Filter Attack", 0.001f, 2.0f, 0.01f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_FILTER_DECAY, "Filter Decay", 0.001f, 2.0f, 0.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_FILTER_SUSTAIN, "Filter Sustain", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_FILTER_RELEASE, "Filter Release", 0.001f, 5.0f, 0.5f));

    // Amp Envelope
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_AMP_ATTACK, "Amp Attack", 0.001f, 2.0f, 0.01f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_AMP_DECAY, "Amp Decay", 0.001f, 2.0f, 0.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_AMP_SUSTAIN, "Amp Sustain", 0.0f, 1.0f, 0.7f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_AMP_RELEASE, "Amp Release", 0.001f, 5.0f, 0.3f));

    // LFOs
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_LFO1_RATE, "LFO 1 Rate", 0.1f, 20.0f, 5.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_LFO1_AMOUNT, "LFO 1 Amount", 0.0f, 2.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_LFO2_RATE, "LFO 2 Rate", 0.1f, 20.0f, 3.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_LFO2_AMOUNT, "LFO 2 Amount", 0.0f, 1.0f, 0.0f));

    // Glide
    params.push_back(std::make_unique<juce::AudioParameterFloat>(PARAM_GLIDE_TIME, "Glide Time", 0.0f, 2.0f, 0.0f));

    return { params.begin(), params.end() };
}

void ClassicMonosynthProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth_.setCurrentPlaybackSampleRate(sampleRate);

    if (auto* voice = dynamic_cast<MonosynthVoice*>(synth_.getVoice(0)))
        voice->prepareToPlay(sampleRate, samplesPerBlock);
}

void ClassicMonosynthProcessor::releaseResources() {}

bool ClassicMonosynthProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void ClassicMonosynthProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    for (const auto metadata : midiMessages)
        midiLearnManager_.processMidiMessage(metadata.getMessage());
    buffer.clear();

    updateVoiceParameters();
    synth_.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void ClassicMonosynthProcessor::updateVoiceParameters()
{
    auto* voice = dynamic_cast<MonosynthVoice*>(synth_.getVoice(0));
    if (!voice) return;

    // Oscillators
    voice->setOsc1Level(*apvts_.getRawParameterValue(PARAM_OSC1_LEVEL));
    voice->setOsc2Level(*apvts_.getRawParameterValue(PARAM_OSC2_LEVEL));
    voice->setOsc3Level(*apvts_.getRawParameterValue(PARAM_OSC3_LEVEL));
    voice->setOsc2Detune(*apvts_.getRawParameterValue(PARAM_OSC2_DETUNE));
    voice->setOsc3Detune(*apvts_.getRawParameterValue(PARAM_OSC3_DETUNE));
    voice->setPulseWidth(*apvts_.getRawParameterValue(PARAM_PULSE_WIDTH));
    voice->setOscSync(*apvts_.getRawParameterValue(PARAM_OSC_SYNC) > 0.5f);

    // Filter
    voice->setFilterCutoff(*apvts_.getRawParameterValue(PARAM_FILTER_CUTOFF));
    voice->setFilterResonance(*apvts_.getRawParameterValue(PARAM_FILTER_RESONANCE));
    voice->setFilterEnvAmount(*apvts_.getRawParameterValue(PARAM_FILTER_ENV_AMOUNT));

    // Filter Envelope
    voice->setFilterAttack(*apvts_.getRawParameterValue(PARAM_FILTER_ATTACK));
    voice->setFilterDecay(*apvts_.getRawParameterValue(PARAM_FILTER_DECAY));
    voice->setFilterSustain(*apvts_.getRawParameterValue(PARAM_FILTER_SUSTAIN));
    voice->setFilterRelease(*apvts_.getRawParameterValue(PARAM_FILTER_RELEASE));

    // Amp Envelope
    voice->setAmpAttack(*apvts_.getRawParameterValue(PARAM_AMP_ATTACK));
    voice->setAmpDecay(*apvts_.getRawParameterValue(PARAM_AMP_DECAY));
    voice->setAmpSustain(*apvts_.getRawParameterValue(PARAM_AMP_SUSTAIN));
    voice->setAmpRelease(*apvts_.getRawParameterValue(PARAM_AMP_RELEASE));

    // LFOs
    voice->setLfo1Rate(*apvts_.getRawParameterValue(PARAM_LFO1_RATE));
    voice->setLfo1Amount(*apvts_.getRawParameterValue(PARAM_LFO1_AMOUNT));
    voice->setLfo2Rate(*apvts_.getRawParameterValue(PARAM_LFO2_RATE));
    voice->setLfo2Amount(*apvts_.getRawParameterValue(PARAM_LFO2_AMOUNT));

    // Glide
    voice->setGlideTime(*apvts_.getRawParameterValue(PARAM_GLIDE_TIME));
}

bool ClassicMonosynthProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* ClassicMonosynthProcessor::createEditor() { return new ClassicMonosynthEditor(*this); }
const juce::String ClassicMonosynthProcessor::getName() const { return JucePlugin_Name; }
bool ClassicMonosynthProcessor::acceptsMidi() const { return true; }
bool ClassicMonosynthProcessor::producesMidi() const { return false; }
bool ClassicMonosynthProcessor::isMidiEffect() const { return false; }
double ClassicMonosynthProcessor::getTailLengthSeconds() const { return 5.0; }
int ClassicMonosynthProcessor::getNumPrograms() { return 1; }
int ClassicMonosynthProcessor::getCurrentProgram() { return 0; }
void ClassicMonosynthProcessor::setCurrentProgram(int) {}
const juce::String ClassicMonosynthProcessor::getProgramName(int) { return {}; }
void ClassicMonosynthProcessor::changeProgramName(int, const juce::String&) {}

void ClassicMonosynthProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts_.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    xml->addChildElement(midiLearnManager_.saveToXml().release());
    xml->addChildElement(presetManager_.saveToXml().release());
    copyXmlToBinary(*xml, destData);
}

void ClassicMonosynthProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts_.state.getType()))
            apvts_.replaceState(juce::ValueTree::fromXml(*xmlState));
        if (auto* midiXml = xmlState->getChildByName("MIDILearnMappings"))
            midiLearnManager_.loadFromXml(*midiXml);
        if (auto* presetXml = xml->getChildByName("PresetManagerState"))
            presetManager_.loadFromXml(*presetXml);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ClassicMonosynthProcessor();
}
