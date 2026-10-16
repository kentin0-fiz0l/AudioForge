#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "../../../shared/dsp/ReverbLevels.h"

static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterFloat>("size", "Size", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("damping", "Damping", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("width", "Width", 0.0f, 1.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("predelay", "PreDelay", juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("mix", "Mix", 0.0f, 1.0f, 0.3f));
    return {params.begin(), params.end()};
}

ReverbProcessor::ReverbProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()),
      midiLearnManager_(apvts_),
      presetManager_(apvts_, "PlateReverb") {

    // Scan for presets on startup
    presetManager_.scanPresets();
}

ReverbProcessor::~ReverbProcessor() {}

static constexpr float maxPreDelayMs = 100.0f;

void ReverbProcessor::prepareToPlay(double sr, int sb) {
    sampleRate_ = sr;
    juce::dsp::ProcessSpec spec{sr, static_cast<juce::uint32>(sb), 2};
    reverb_.prepare(spec);

    preDelay_.setMaximumDelayInSamples(static_cast<int>(std::ceil(sr * maxPreDelayMs / 1000.0)) + 1);
    preDelay_.prepare(spec);
    wetBuffer_.setSize(2, sb);

    preDelaySamples_.reset(sr, 0.05);
    preDelaySamples_.setCurrentAndTargetValue(preDelayInSamples());
    dryGain_.reset(sr, 0.05);
    dryGain_.setCurrentAndTargetValue(1.0f - apvts_.getRawParameterValue("mix")->load());
}

void ReverbProcessor::processBlock(juce::AudioBuffer<float>& buf, juce::MidiBuffer& midi) {
    juce::ScopedNoDenormals noDenormals;
    for (const auto metadata : midi)
        midiLearnManager_.processMidiMessage(metadata.getMessage());
    updateReverbParameters();

    const int numChannels = juce::jmin(buf.getNumChannels(), 2);
    const int numSamples = buf.getNumSamples();
    // Only reallocates if the host sends a bigger block than it promised
    wetBuffer_.setSize(numChannels, numSamples, false, false, true);

    for (int i = 0; i < numSamples; ++i) {
        const float delay = preDelaySamples_.getNextValue();
        for (int ch = 0; ch < numChannels; ++ch) {
            preDelay_.pushSample(ch, buf.getSample(ch, i));
            wetBuffer_.setSample(ch, i, preDelay_.popSample(ch, delay));
        }
    }

    juce::dsp::AudioBlock<float> wetBlock(wetBuffer_);
    juce::dsp::ProcessContextReplacing<float> context(wetBlock);
    reverb_.process(context);

    dryGain_.applyGain(buf, numSamples);
    for (int ch = 0; ch < numChannels; ++ch)
        buf.addFrom(ch, 0, wetBuffer_, ch, 0, numSamples);
}

float ReverbProcessor::preDelayInSamples() const {
    return apvts_.getRawParameterValue("predelay")->load() * static_cast<float>(sampleRate_) / 1000.0f;
}

void ReverbProcessor::updateReverbParameters() {
    juce::dsp::Reverb::Parameters params;
    params.roomSize = apvts_.getRawParameterValue("size")->load();
    params.damping = apvts_.getRawParameterValue("damping")->load();
    params.width = apvts_.getRawParameterValue("width")->load();
    // Mix at zero passes the signal through at its own level, and Mix at
    // one gives the reverb alone at about that level. The reverb runs
    // wet-only; the dry signal is added in processBlock
    const float mix = apvts_.getRawParameterValue("mix")->load();
    params.wetLevel = AudioForge::DSP::reverbLevelsForMix(mix).wet;
    params.dryLevel = 0.0f;
    reverb_.setParameters(params);
    dryGain_.setTargetValue(1.0f - mix);
    preDelaySamples_.setTargetValue(preDelayInSamples());
}

juce::AudioProcessorEditor* ReverbProcessor::createEditor() {
    return new ReverbEditor(*this);
}

void ReverbProcessor::getStateInformation(juce::MemoryBlock& d) {
    auto s = apvts_.copyState();
    std::unique_ptr<juce::XmlElement> x(s.createXml());
    x->addChildElement(midiLearnManager_.saveToXml().release());
    x->addChildElement(presetManager_.saveToXml().release());
    copyXmlToBinary(*x, d);
}

void ReverbProcessor::setStateInformation(const void* d, int sz) {
    std::unique_ptr<juce::XmlElement> x(getXmlFromBinary(d, sz));
    // A state that did not parse, or that belongs to something else, is ignored
    if (x == nullptr || ! x->hasTagName(apvts_.state.getType()))
        return;

    apvts_.replaceState(juce::ValueTree::fromXml(*x));

    if (auto* midiXml = x->getChildByName("MIDILearnMappings"))
        midiLearnManager_.loadFromXml(*midiXml);
    if (auto* presetXml = x->getChildByName("PresetManagerState"))
        presetManager_.loadFromXml(*presetXml);
}

#ifndef AUDIOFORGE_TESTS
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new ReverbProcessor();
}
#endif
