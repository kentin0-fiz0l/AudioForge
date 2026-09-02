/*
  ==============================================================================

    PluginEditor.h
    Created: 1 Sep 2026
    Author:  AudioForge Team

    UI for Multiband Compressor plugin.

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

//==============================================================================
class MultibandCompressorAudioProcessorEditor : public juce::AudioProcessorEditor,
                                                 private juce::Timer
{
public:
    MultibandCompressorAudioProcessorEditor(MultibandCompressorAudioProcessor&);
    ~MultibandCompressorAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    MultibandCompressorAudioProcessor& audioProcessor;

    // Crossover sliders (3)
    juce::Slider crossover1Slider, crossover2Slider, crossover3Slider;
    juce::Label crossover1Label, crossover2Label, crossover3Label;

    // Band 1 controls
    juce::Slider band1ThresholdSlider, band1RatioSlider, band1AttackSlider, band1ReleaseSlider, band1MakeupSlider;
    juce::ToggleButton band1SoloButton, band1BypassButton, band1MidSideButton;

    // Band 2 controls
    juce::Slider band2ThresholdSlider, band2RatioSlider, band2AttackSlider, band2ReleaseSlider, band2MakeupSlider;
    juce::ToggleButton band2SoloButton, band2BypassButton, band2MidSideButton;

    // Band 3 controls
    juce::Slider band3ThresholdSlider, band3RatioSlider, band3AttackSlider, band3ReleaseSlider, band3MakeupSlider;
    juce::ToggleButton band3SoloButton, band3BypassButton, band3MidSideButton;

    // Band 4 controls
    juce::Slider band4ThresholdSlider, band4RatioSlider, band4AttackSlider, band4ReleaseSlider, band4MakeupSlider;
    juce::ToggleButton band4SoloButton, band4BypassButton, band4MidSideButton;

    // Parameter attachments - Crossovers
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> crossover1Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> crossover2Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> crossover3Attachment;

    // Band 1 attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> band1ThresholdAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> band1RatioAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> band1AttackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> band1ReleaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> band1MakeupAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> band1SoloAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> band1BypassAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> band1MidSideAttachment;

    // Band 2 attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> band2ThresholdAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> band2RatioAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> band2AttackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> band2ReleaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> band2MakeupAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> band2SoloAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> band2BypassAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> band2MidSideAttachment;

    // Band 3 attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> band3ThresholdAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> band3RatioAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> band3AttackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> band3ReleaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> band3MakeupAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> band3SoloAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> band3BypassAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> band3MidSideAttachment;

    // Band 4 attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> band4ThresholdAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> band4RatioAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> band4AttackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> band4ReleaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> band4MakeupAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> band4SoloAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> band4BypassAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> band4MidSideAttachment;

    // Metering
    float band1GR = 0.0f, band2GR = 0.0f, band3GR = 0.0f, band4GR = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultibandCompressorAudioProcessorEditor)
};
