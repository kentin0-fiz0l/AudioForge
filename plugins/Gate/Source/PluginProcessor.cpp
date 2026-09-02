/*
  ==============================================================================

    PluginProcessor.cpp
    Created: 1 Sep 2026
    Author:  AudioForge Team

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GateAudioProcessor::GateAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor(BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       .withInput  ("Sidechain", juce::AudioChannelSet::stereo(), true)
                       ),
#endif
       parameters(*this, nullptr, juce::Identifier("GateParameters"),
                  {
                      // Core parameters
                      std::make_unique<juce::AudioParameterFloat>(
                          "threshold",
                          "Threshold",
                          juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f),
                          -30.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 1) + " dB"; }
                      ),
                      std::make_unique<juce::AudioParameterFloat>(
                          "ratio",
                          "Ratio",
                          juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f),
                          10.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 1) + ":1"; }
                      ),
                      std::make_unique<juce::AudioParameterFloat>(
                          "range",
                          "Range",
                          juce::NormalisableRange<float>(-96.0f, 0.0f, 0.1f),
                          -60.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 1) + " dB"; }
                      ),
                      std::make_unique<juce::AudioParameterFloat>(
                          "attack",
                          "Attack",
                          juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f, 0.3f),
                          1.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 1) + " ms"; }
                      ),
                      std::make_unique<juce::AudioParameterFloat>(
                          "hold",
                          "Hold",
                          juce::NormalisableRange<float>(0.0f, 1000.0f, 1.0f, 0.3f),
                          50.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 0) + " ms"; }
                      ),
                      std::make_unique<juce::AudioParameterFloat>(
                          "release",
                          "Release",
                          juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f, 0.3f),
                          100.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 0) + " ms"; }
                      ),
                      std::make_unique<juce::AudioParameterFloat>(
                          "lookahead",
                          "Lookahead",
                          juce::NormalisableRange<float>(0.0f, 10.0f, 0.1f),
                          2.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 1) + " ms"; }
                      ),
                      std::make_unique<juce::AudioParameterChoice>(
                          "mode",
                          "Mode",
                          juce::StringArray{"Gate", "Expander"},
                          0
                      ),

                      // Sidechain parameters
                      std::make_unique<juce::AudioParameterBool>(
                          "sidechainEnable",
                          "Sidechain Enable",
                          false
                      ),
                      std::make_unique<juce::AudioParameterFloat>(
                          "scHighPass",
                          "SC High-Pass",
                          juce::NormalisableRange<float>(20.0f, 2000.0f, 1.0f, 0.3f),
                          80.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 0) + " Hz"; }
                      ),
                      std::make_unique<juce::AudioParameterFloat>(
                          "scLowPass",
                          "SC Low-Pass",
                          juce::NormalisableRange<float>(200.0f, 20000.0f, 1.0f, 0.3f),
                          8000.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 0) + " Hz"; }
                      ),
                      std::make_unique<juce::AudioParameterBool>(
                          "scListen",
                          "SC Listen",
                          false
                      ),

                      // Detection parameters
                      std::make_unique<juce::AudioParameterChoice>(
                          "detectionMode",
                          "Detection Mode",
                          juce::StringArray{"RMS", "Peak"},
                          0
                      ),
                      std::make_unique<juce::AudioParameterFloat>(
                          "rmsWindow",
                          "RMS Window",
                          juce::NormalisableRange<float>(1.0f, 50.0f, 0.1f),
                          10.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 1) + " ms"; }
                      )
                  })
{
    // Get parameter pointers
    thresholdParam = parameters.getRawParameterValue("threshold");
    ratioParam = parameters.getRawParameterValue("ratio");
    rangeParam = parameters.getRawParameterValue("range");
    attackParam = parameters.getRawParameterValue("attack");
    holdParam = parameters.getRawParameterValue("hold");
    releaseParam = parameters.getRawParameterValue("release");
    lookaheadParam = parameters.getRawParameterValue("lookahead");
    modeParam = parameters.getRawParameterValue("mode");

    sidechainEnableParam = parameters.getRawParameterValue("sidechainEnable");
    scHighPassParam = parameters.getRawParameterValue("scHighPass");
    scLowPassParam = parameters.getRawParameterValue("scLowPass");
    scListenParam = parameters.getRawParameterValue("scListen");

    detectionModeParam = parameters.getRawParameterValue("detectionMode");
    rmsWindowParam = parameters.getRawParameterValue("rmsWindow");
}

GateAudioProcessor::~GateAudioProcessor()
{
}

//==============================================================================
const juce::String GateAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GateAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GateAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GateAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GateAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GateAudioProcessor::getNumPrograms()
{
    return 1;
}

int GateAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GateAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String GateAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void GateAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void GateAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    gateEngine.prepare(sampleRate, samplesPerBlock);
}

void GateAudioProcessor::releaseResources()
{
    gateEngine.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GateAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
  #else
    // Main input/output: stereo
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // Sidechain input: stereo or disabled
    if (layouts.getNumChannels(true, 1) > 0 &&
        layouts.getChannelSet(true, 1) != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void GateAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;

    // Update parameters
    gateEngine.setThreshold(*thresholdParam);
    gateEngine.setRatio(*ratioParam);
    gateEngine.setRange(*rangeParam);
    gateEngine.setAttack(*attackParam);
    gateEngine.setHold(*holdParam);
    gateEngine.setRelease(*releaseParam);
    gateEngine.setLookahead(*lookaheadParam);
    gateEngine.setMode(*modeParam < 0.5f ? audioforge::GateEngine::Gate
                                          : audioforge::GateEngine::Expander);

    // Update sidechain parameters
    auto& sidechain = gateEngine.getSidechain();
    sidechain.setEnabled(*sidechainEnableParam > 0.5f);
    sidechain.setHighPassFreq(*scHighPassParam);
    sidechain.setLowPassFreq(*scLowPassParam);
    sidechain.setListenMode(*scListenParam > 0.5f);

    // Update detection parameters
    auto& envelopeFollower = gateEngine.getEnvelopeFollower();
    envelopeFollower.setDetectionMode(*detectionModeParam < 0.5f
        ? audioforge::EnvelopeFollower::RMS
        : audioforge::EnvelopeFollower::Peak);
    envelopeFollower.setRMSWindow(*rmsWindowParam);

    // Get sidechain buffer (if available)
    juce::AudioBuffer<float>* sidechainBuffer = nullptr;
    if (getBusCount(true) > 1)
    {
        auto sidechainBus = getBusBuffer(buffer, true, 1);
        if (sidechainBus.getNumChannels() > 0 && sidechainBus.getNumSamples() > 0)
        {
            // Create a temporary buffer for sidechain
            static juce::AudioBuffer<float> tempSidechain;
            tempSidechain.makeCopyOf(sidechainBus);
            sidechainBuffer = &tempSidechain;
        }
    }

    // Process audio
    gateEngine.process(buffer, sidechainBuffer);
}

//==============================================================================
bool GateAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* GateAudioProcessor::createEditor()
{
    return new GateAudioProcessorEditor(*this);
}

//==============================================================================
void GateAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void GateAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GateAudioProcessor();
}
