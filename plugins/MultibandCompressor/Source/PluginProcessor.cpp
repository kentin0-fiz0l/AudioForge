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
MultibandCompressorAudioProcessor::MultibandCompressorAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor(BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
       parameters(*this, nullptr, juce::Identifier("MultibandCompressorParameters"),
                  {
                      //==================================================================
                      // Crossover Frequencies (3 parameters)
                      //==================================================================

                      std::make_unique<juce::AudioParameterFloat>(
                          "crossover1",
                          "Crossover 1",
                          juce::NormalisableRange<float>(20.0f, 500.0f, 1.0f, 0.3f),
                          250.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 0) + " Hz"; }
                      ),

                      std::make_unique<juce::AudioParameterFloat>(
                          "crossover2",
                          "Crossover 2",
                          juce::NormalisableRange<float>(200.0f, 5000.0f, 1.0f, 0.3f),
                          2000.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 0) + " Hz"; }
                      ),

                      std::make_unique<juce::AudioParameterFloat>(
                          "crossover3",
                          "Crossover 3",
                          juce::NormalisableRange<float>(2000.0f, 16000.0f, 1.0f, 0.3f),
                          8000.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 0) + " Hz"; }
                      ),

                      //==================================================================
                      // Band 1 (Low: 20-250 Hz) - 8 parameters
                      //==================================================================

                      std::make_unique<juce::AudioParameterFloat>(
                          "band1Threshold",
                          "Band 1 Threshold",
                          juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f),
                          -20.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 1) + " dB"; }
                      ),

                      std::make_unique<juce::AudioParameterFloat>(
                          "band1Ratio",
                          "Band 1 Ratio",
                          juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f),
                          4.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 1) + ":1"; }
                      ),

                      std::make_unique<juce::AudioParameterFloat>(
                          "band1Attack",
                          "Band 1 Attack",
                          juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f, 0.3f),
                          10.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 1) + " ms"; }
                      ),

                      std::make_unique<juce::AudioParameterFloat>(
                          "band1Release",
                          "Band 1 Release",
                          juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f, 0.3f),
                          100.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 0) + " ms"; }
                      ),

                      std::make_unique<juce::AudioParameterFloat>(
                          "band1Makeup",
                          "Band 1 Makeup",
                          juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
                          0.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 1) + " dB"; }
                      ),

                      std::make_unique<juce::AudioParameterBool>("band1Solo", "Band 1 Solo", false),
                      std::make_unique<juce::AudioParameterBool>("band1Bypass", "Band 1 Bypass", false),
                      std::make_unique<juce::AudioParameterBool>("band1MidSide", "Band 1 Mid/Side", false),

                      //==================================================================
                      // Band 2 (Low-Mid: 250Hz-2kHz) - 8 parameters
                      //==================================================================

                      std::make_unique<juce::AudioParameterFloat>(
                          "band2Threshold",
                          "Band 2 Threshold",
                          juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f),
                          -20.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 1) + " dB"; }
                      ),

                      std::make_unique<juce::AudioParameterFloat>(
                          "band2Ratio",
                          "Band 2 Ratio",
                          juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f),
                          4.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 1) + ":1"; }
                      ),

                      std::make_unique<juce::AudioParameterFloat>(
                          "band2Attack",
                          "Band 2 Attack",
                          juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f, 0.3f),
                          10.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 1) + " ms"; }
                      ),

                      std::make_unique<juce::AudioParameterFloat>(
                          "band2Release",
                          "Band 2 Release",
                          juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f, 0.3f),
                          100.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 0) + " ms"; }
                      ),

                      std::make_unique<juce::AudioParameterFloat>(
                          "band2Makeup",
                          "Band 2 Makeup",
                          juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
                          0.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 1) + " dB"; }
                      ),

                      std::make_unique<juce::AudioParameterBool>("band2Solo", "Band 2 Solo", false),
                      std::make_unique<juce::AudioParameterBool>("band2Bypass", "Band 2 Bypass", false),
                      std::make_unique<juce::AudioParameterBool>("band2MidSide", "Band 2 Mid/Side", false),

                      //==================================================================
                      // Band 3 (High-Mid: 2kHz-8kHz) - 8 parameters
                      //==================================================================

                      std::make_unique<juce::AudioParameterFloat>(
                          "band3Threshold",
                          "Band 3 Threshold",
                          juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f),
                          -20.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 1) + " dB"; }
                      ),

                      std::make_unique<juce::AudioParameterFloat>(
                          "band3Ratio",
                          "Band 3 Ratio",
                          juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f),
                          4.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 1) + ":1"; }
                      ),

                      std::make_unique<juce::AudioParameterFloat>(
                          "band3Attack",
                          "Band 3 Attack",
                          juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f, 0.3f),
                          10.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 1) + " ms"; }
                      ),

                      std::make_unique<juce::AudioParameterFloat>(
                          "band3Release",
                          "Band 3 Release",
                          juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f, 0.3f),
                          100.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 0) + " ms"; }
                      ),

                      std::make_unique<juce::AudioParameterFloat>(
                          "band3Makeup",
                          "Band 3 Makeup",
                          juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
                          0.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 1) + " dB"; }
                      ),

                      std::make_unique<juce::AudioParameterBool>("band3Solo", "Band 3 Solo", false),
                      std::make_unique<juce::AudioParameterBool>("band3Bypass", "Band 3 Bypass", false),
                      std::make_unique<juce::AudioParameterBool>("band3MidSide", "Band 3 Mid/Side", false),

                      //==================================================================
                      // Band 4 (High: 8kHz-20kHz) - 8 parameters
                      //==================================================================

                      std::make_unique<juce::AudioParameterFloat>(
                          "band4Threshold",
                          "Band 4 Threshold",
                          juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f),
                          -20.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 1) + " dB"; }
                      ),

                      std::make_unique<juce::AudioParameterFloat>(
                          "band4Ratio",
                          "Band 4 Ratio",
                          juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f),
                          4.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 1) + ":1"; }
                      ),

                      std::make_unique<juce::AudioParameterFloat>(
                          "band4Attack",
                          "Band 4 Attack",
                          juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f, 0.3f),
                          10.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 1) + " ms"; }
                      ),

                      std::make_unique<juce::AudioParameterFloat>(
                          "band4Release",
                          "Band 4 Release",
                          juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f, 0.3f),
                          100.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 0) + " ms"; }
                      ),

                      std::make_unique<juce::AudioParameterFloat>(
                          "band4Makeup",
                          "Band 4 Makeup",
                          juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
                          0.0f,
                          juce::String(),
                          juce::AudioProcessorParameter::genericParameter,
                          [](float value, int) { return juce::String(value, 1) + " dB"; }
                      ),

                      std::make_unique<juce::AudioParameterBool>("band4Solo", "Band 4 Solo", false),
                      std::make_unique<juce::AudioParameterBool>("band4Bypass", "Band 4 Bypass", false),
                      std::make_unique<juce::AudioParameterBool>("band4MidSide", "Band 4 Mid/Side", false)
                  })
{
    // Get parameter pointers - Crossovers
    crossover1Param = parameters.getRawParameterValue("crossover1");
    crossover2Param = parameters.getRawParameterValue("crossover2");
    crossover3Param = parameters.getRawParameterValue("crossover3");

    // Band 1 parameters
    band1ThresholdParam = parameters.getRawParameterValue("band1Threshold");
    band1RatioParam = parameters.getRawParameterValue("band1Ratio");
    band1AttackParam = parameters.getRawParameterValue("band1Attack");
    band1ReleaseParam = parameters.getRawParameterValue("band1Release");
    band1MakeupParam = parameters.getRawParameterValue("band1Makeup");
    band1SoloParam = parameters.getRawParameterValue("band1Solo");
    band1BypassParam = parameters.getRawParameterValue("band1Bypass");
    band1MidSideParam = parameters.getRawParameterValue("band1MidSide");

    // Band 2 parameters
    band2ThresholdParam = parameters.getRawParameterValue("band2Threshold");
    band2RatioParam = parameters.getRawParameterValue("band2Ratio");
    band2AttackParam = parameters.getRawParameterValue("band2Attack");
    band2ReleaseParam = parameters.getRawParameterValue("band2Release");
    band2MakeupParam = parameters.getRawParameterValue("band2Makeup");
    band2SoloParam = parameters.getRawParameterValue("band2Solo");
    band2BypassParam = parameters.getRawParameterValue("band2Bypass");
    band2MidSideParam = parameters.getRawParameterValue("band2MidSide");

    // Band 3 parameters
    band3ThresholdParam = parameters.getRawParameterValue("band3Threshold");
    band3RatioParam = parameters.getRawParameterValue("band3Ratio");
    band3AttackParam = parameters.getRawParameterValue("band3Attack");
    band3ReleaseParam = parameters.getRawParameterValue("band3Release");
    band3MakeupParam = parameters.getRawParameterValue("band3Makeup");
    band3SoloParam = parameters.getRawParameterValue("band3Solo");
    band3BypassParam = parameters.getRawParameterValue("band3Bypass");
    band3MidSideParam = parameters.getRawParameterValue("band3MidSide");

    // Band 4 parameters
    band4ThresholdParam = parameters.getRawParameterValue("band4Threshold");
    band4RatioParam = parameters.getRawParameterValue("band4Ratio");
    band4AttackParam = parameters.getRawParameterValue("band4Attack");
    band4ReleaseParam = parameters.getRawParameterValue("band4Release");
    band4MakeupParam = parameters.getRawParameterValue("band4Makeup");
    band4SoloParam = parameters.getRawParameterValue("band4Solo");
    band4BypassParam = parameters.getRawParameterValue("band4Bypass");
    band4MidSideParam = parameters.getRawParameterValue("band4MidSide");
}

MultibandCompressorAudioProcessor::~MultibandCompressorAudioProcessor()
{
}

//==============================================================================
const juce::String MultibandCompressorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MultibandCompressorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MultibandCompressorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MultibandCompressorAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MultibandCompressorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MultibandCompressorAudioProcessor::getNumPrograms()
{
    return 1;
}

int MultibandCompressorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MultibandCompressorAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String MultibandCompressorAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void MultibandCompressorAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void MultibandCompressorAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    multibandEngine.prepare(sampleRate, samplesPerBlock);
}

void MultibandCompressorAudioProcessor::releaseResources()
{
    multibandEngine.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MultibandCompressorAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MultibandCompressorAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                                      juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;

    // Update crossover frequencies
    multibandEngine.setCrossoverFrequency(0, *crossover1Param);
    multibandEngine.setCrossoverFrequency(1, *crossover2Param);
    multibandEngine.setCrossoverFrequency(2, *crossover3Param);

    // Update Band 1 parameters
    auto& band1 = multibandEngine.getBandCompressor(0);
    band1.setThreshold(*band1ThresholdParam);
    band1.setRatio(*band1RatioParam);
    band1.setAttack(*band1AttackParam);
    band1.setRelease(*band1ReleaseParam);
    band1.setMakeupGain(*band1MakeupParam);
    multibandEngine.setBandSolo(0, *band1SoloParam > 0.5f);
    multibandEngine.setBandBypass(0, *band1BypassParam > 0.5f);
    multibandEngine.setBandMidSideEnabled(0, *band1MidSideParam > 0.5f);

    // Update Band 2 parameters
    auto& band2 = multibandEngine.getBandCompressor(1);
    band2.setThreshold(*band2ThresholdParam);
    band2.setRatio(*band2RatioParam);
    band2.setAttack(*band2AttackParam);
    band2.setRelease(*band2ReleaseParam);
    band2.setMakeupGain(*band2MakeupParam);
    multibandEngine.setBandSolo(1, *band2SoloParam > 0.5f);
    multibandEngine.setBandBypass(1, *band2BypassParam > 0.5f);
    multibandEngine.setBandMidSideEnabled(1, *band2MidSideParam > 0.5f);

    // Update Band 3 parameters
    auto& band3 = multibandEngine.getBandCompressor(2);
    band3.setThreshold(*band3ThresholdParam);
    band3.setRatio(*band3RatioParam);
    band3.setAttack(*band3AttackParam);
    band3.setRelease(*band3ReleaseParam);
    band3.setMakeupGain(*band3MakeupParam);
    multibandEngine.setBandSolo(2, *band3SoloParam > 0.5f);
    multibandEngine.setBandBypass(2, *band3BypassParam > 0.5f);
    multibandEngine.setBandMidSideEnabled(2, *band3MidSideParam > 0.5f);

    // Update Band 4 parameters
    auto& band4 = multibandEngine.getBandCompressor(3);
    band4.setThreshold(*band4ThresholdParam);
    band4.setRatio(*band4RatioParam);
    band4.setAttack(*band4AttackParam);
    band4.setRelease(*band4ReleaseParam);
    band4.setMakeupGain(*band4MakeupParam);
    multibandEngine.setBandSolo(3, *band4SoloParam > 0.5f);
    multibandEngine.setBandBypass(3, *band4BypassParam > 0.5f);
    multibandEngine.setBandMidSideEnabled(3, *band4MidSideParam > 0.5f);

    // Process audio
    multibandEngine.process(buffer);
}

//==============================================================================
bool MultibandCompressorAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* MultibandCompressorAudioProcessor::createEditor()
{
    return new MultibandCompressorAudioProcessorEditor(*this);
}

//==============================================================================
void MultibandCompressorAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MultibandCompressorAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MultibandCompressorAudioProcessor();
}
