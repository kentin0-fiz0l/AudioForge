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
LimiterAudioProcessor::LimiterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
    parameters(*this, nullptr, juce::Identifier("LimiterParameters"),
    {
        std::make_unique<juce::AudioParameterFloat>(
            "ceiling",
            "Ceiling",
            juce::NormalisableRange<float>(-12.0f, 0.0f, 0.1f),
            -0.3f,
            "dB"
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "threshold",
            "Threshold",
            juce::NormalisableRange<float>(-24.0f, 0.0f, 0.1f),
            -6.0f,
            "dB"
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "release",
            "Release",
            juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f, 0.5f),
            100.0f,
            "ms"
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "lookahead",
            "Lookahead",
            juce::NormalisableRange<float>(0.0f, 10.0f, 0.1f),
            5.0f,
            "ms"
        ),
        std::make_unique<juce::AudioParameterBool>(
            "autoMakeup",
            "Auto Makeup",
            true
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "outputTrim",
            "Output Trim",
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
            0.0f,
            "dB"
        )
    })
{
    // Get parameter pointers
    ceilingParam = parameters.getRawParameterValue("ceiling");
    thresholdParam = parameters.getRawParameterValue("threshold");
    releaseParam = parameters.getRawParameterValue("release");
    lookaheadParam = parameters.getRawParameterValue("lookahead");
    autoMakeupParam = parameters.getRawParameterValue("autoMakeup");
    outputTrimParam = parameters.getRawParameterValue("outputTrim");
}

LimiterAudioProcessor::~LimiterAudioProcessor()
{
}

//==============================================================================
const juce::String LimiterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool LimiterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool LimiterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool LimiterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double LimiterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int LimiterAudioProcessor::getNumPrograms()
{
    return 1;
}

int LimiterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void LimiterAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused(index);
}

const juce::String LimiterAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused(index);
    return {};
}

void LimiterAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void LimiterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    limiterEngine.prepare(sampleRate, samplesPerBlock);
}

void LimiterAudioProcessor::releaseResources()
{
    limiterEngine.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool LimiterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // Support stereo and mono
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // Input and output layout must match
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void LimiterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;

    // Update parameters from UI
    limiterEngine.setCeiling(ceilingParam->load());
    limiterEngine.setThreshold(thresholdParam->load());
    limiterEngine.setRelease(releaseParam->load());
    limiterEngine.setLookahead(lookaheadParam->load());
    limiterEngine.setAutoMakeupEnabled(autoMakeupParam->load() > 0.5f);
    limiterEngine.setOutputTrim(outputTrimParam->load());

    // Process audio
    limiterEngine.process(buffer);
}

//==============================================================================
bool LimiterAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* LimiterAudioProcessor::createEditor()
{
    return new LimiterAudioProcessorEditor (*this);
}

//==============================================================================
void LimiterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void LimiterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LimiterAudioProcessor();
}
