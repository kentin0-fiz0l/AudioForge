/*
  ==============================================================================

    PluginEditor.h
    Created: 1 Sep 2026
    Author:  AudioForge Team

    UI for Limiter plugin.

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

//==============================================================================
/**
 * @brief Limiter Plugin Editor
 *
 * Features:
 * - 6 parameter controls (rotary sliders)
 * - Input/output meters
 * - Gain reduction meter
 * - True peak indicator
 */
class LimiterAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                      private juce::Timer
{
public:
    LimiterAudioProcessorEditor (LimiterAudioProcessor&);
    ~LimiterAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // Timer callback for metering updates
    void timerCallback() override;

    // Reference to processor
    LimiterAudioProcessor& audioProcessor;

    // Parameter sliders
    juce::Slider ceilingSlider;
    juce::Slider thresholdSlider;
    juce::Slider releaseSlider;
    juce::Slider lookaheadSlider;
    juce::Slider outputTrimSlider;

    // Labels
    juce::Label ceilingLabel;
    juce::Label thresholdLabel;
    juce::Label releaseLabel;
    juce::Label lookaheadLabel;
    juce::Label outputTrimLabel;

    // Toggle button
    juce::ToggleButton autoMakeupButton;

    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ceilingAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lookaheadAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputTrimAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> autoMakeupAttachment;

    // Metering values (updated by timer)
    float inputLevel = 0.0f;
    float outputLevel = 0.0f;
    float gainReduction = 0.0f;
    bool truePeakClipping = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LimiterAudioProcessorEditor)
};
