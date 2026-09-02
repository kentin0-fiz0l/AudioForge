/*
  ==============================================================================

    PluginEditor.h
    Created: 1 Sep 2026
    Author:  AudioForge Team

    UI for Gate plugin.

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

//==============================================================================
/**
 * @brief Gate Plugin Editor
 *
 * Features:
 * - 8 rotary sliders (threshold, ratio, range, attack, hold, release, lookahead, RMS window)
 * - 2 toggle buttons (mode, sidechain enable, sidechain listen)
 * - 2 combo boxes (detection mode)
 * - 2 frequency sliders (SC HP, SC LP)
 * - 3 level meters (input, GR, output)
 */
class GateAudioProcessorEditor : public juce::AudioProcessorEditor,
                                  private juce::Timer
{
public:
    GateAudioProcessorEditor(GateAudioProcessor&);
    ~GateAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // Timer callback for metering updates
    void timerCallback() override;

    // Reference to processor
    GateAudioProcessor& audioProcessor;

    // Parameter sliders
    juce::Slider thresholdSlider;
    juce::Slider ratioSlider;
    juce::Slider rangeSlider;
    juce::Slider attackSlider;
    juce::Slider holdSlider;
    juce::Slider releaseSlider;
    juce::Slider lookaheadSlider;
    juce::Slider rmsWindowSlider;

    // Sidechain sliders
    juce::Slider scHighPassSlider;
    juce::Slider scLowPassSlider;

    // Labels
    juce::Label thresholdLabel;
    juce::Label ratioLabel;
    juce::Label rangeLabel;
    juce::Label attackLabel;
    juce::Label holdLabel;
    juce::Label releaseLabel;
    juce::Label lookaheadLabel;
    juce::Label rmsWindowLabel;
    juce::Label scHighPassLabel;
    juce::Label scLowPassLabel;

    // Toggle buttons
    juce::ToggleButton modeButton;
    juce::ToggleButton sidechainEnableButton;
    juce::ToggleButton scListenButton;

    // Combo boxes
    juce::ComboBox detectionModeCombo;
    juce::Label detectionModeLabel;

    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ratioAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rangeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> holdAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lookaheadAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rmsWindowAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> scHighPassAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> scLowPassAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> modeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> sidechainEnableAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> scListenAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> detectionModeAttachment;

    // Metering values (updated by timer)
    float inputLevel = 0.0f;
    float outputLevel = 0.0f;
    float gainReduction = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GateAudioProcessorEditor)
};
