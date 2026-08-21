#pragma once

#include "PluginProcessor.h"
#include <juce_audio_processors/juce_audio_processors.h>

/**
 * SimpleComp Plugin Editor
 *
 * User interface for the basic compressor.
 */
class SimpleCompEditor : public juce::AudioProcessorEditor,
                         private juce::Timer
{
public:
    SimpleCompEditor(SimpleCompProcessor&);
    ~SimpleCompEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    // Reference to processor
    SimpleCompProcessor& audioProcessor;

    // UI Components
    juce::Slider thresholdSlider;
    juce::Label thresholdLabel;

    juce::Slider ratioSlider;
    juce::Label ratioLabel;

    juce::Slider attackSlider;
    juce::Label attackLabel;

    juce::Slider releaseSlider;
    juce::Label releaseLabel;

    juce::ComboBox kneeComboBox;
    juce::Label kneeLabel;

    // Gain reduction meter display
    float displayGainReduction = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleCompEditor)
};
