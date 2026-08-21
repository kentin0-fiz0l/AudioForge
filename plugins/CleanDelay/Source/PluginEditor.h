#pragma once

#include "PluginProcessor.h"
#include <juce_audio_processors/juce_audio_processors.h>

/**
 * CleanDelay Plugin Editor
 *
 * User interface for the CleanDelay effect.
 */
class CleanDelayEditor : public juce::AudioProcessorEditor,
                         private juce::Timer
{
public:
    CleanDelayEditor(CleanDelayProcessor&);
    ~CleanDelayEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    // Reference to processor
    CleanDelayProcessor& audioProcessor;

    // UI Components
    juce::Slider delayTimeSlider;
    juce::Label delayTimeLabel;

    juce::Slider feedbackSlider;
    juce::Label feedbackLabel;

    juce::Slider mixSlider;
    juce::Label mixLabel;

    juce::ToggleButton pingPongButton;
    juce::Label pingPongLabel;

    // Level meter displays
    float displayInputLevel = 0.0f;
    float displayOutputLevel = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CleanDelayEditor)
};
