#pragma once

#include "PluginProcessor.h"
#include <JuceHeader.h>

/**
 * SimpleGain Editor (GUI)
 *
 * Simple, clean interface with:
 * - Gain slider (-60 to +12 dB)
 * - Level meter
 * - Value display
 */
class SimpleGainEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    SimpleGainEditor(SimpleGainProcessor&);
    ~SimpleGainEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // Timer callback for meter updates (30 fps)
    void timerCallback() override;

    // Reference to processor
    SimpleGainProcessor& processor;

    // UI Components
    juce::Slider gainSlider;
    juce::Label gainLabel;
    juce::Label meterLabel;

    // Parameter attachment (links slider to parameter)
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;

    // Meter value
    float meterLevel = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleGainEditor)
};
