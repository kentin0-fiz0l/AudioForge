#pragma once

#include "PluginProcessor.h"
#include <juce_gui_basics/juce_gui_basics.h>

/**
 * PanUtil Editor (GUI)
 *
 * Features:
 * - Pan slider with center detent
 * - Width slider
 * - Pan/Balance mode selector
 * - Dual L/R level meters
 * - Visual stereo field display
 */
class PanUtilEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    PanUtilEditor(PanUtilProcessor&);
    ~PanUtilEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // Timer callback for meter updates
    void timerCallback() override;

    // Reference to processor
    PanUtilProcessor& processor;

    // UI Components
    juce::Slider panSlider;
    juce::Label panLabel;

    juce::Slider widthSlider;
    juce::Label widthLabel;

    juce::ComboBox modeSelector;
    juce::Label modeLabel;

    // Meter values
    float leftMeter = 0.0f;
    float rightMeter = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PanUtilEditor)
};
