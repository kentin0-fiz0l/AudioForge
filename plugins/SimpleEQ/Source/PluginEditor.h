#pragma once

#include "PluginProcessor.h"
#include <juce_audio_processors/juce_audio_processors.h>

/**
 * SimpleEQ Plugin Editor
 *
 * User interface for the 3-band parametric EQ.
 */
class SimpleEQEditor : public juce::AudioProcessorEditor,
                       private juce::Timer
{
public:
    SimpleEQEditor(SimpleEQProcessor&);
    ~SimpleEQEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    // Reference to processor
    SimpleEQProcessor& audioProcessor;

    // UI Components - EQ bands
    juce::Slider lowFreqSlider;
    juce::Label lowFreqLabel;
    juce::Slider lowGainSlider;
    juce::Label lowGainLabel;

    juce::Slider midFreqSlider;
    juce::Label midFreqLabel;
    juce::Slider midGainSlider;
    juce::Label midGainLabel;

    juce::Slider highFreqSlider;
    juce::Label highFreqLabel;
    juce::Slider highGainSlider;
    juce::Label highGainLabel;

    // UI Components - Filters
    juce::ToggleButton hpfEnabledButton;
    juce::Label hpfLabel;
    juce::Slider hpfFreqSlider;

    juce::ToggleButton lpfEnabledButton;
    juce::Label lpfLabel;
    juce::Slider lpfFreqSlider;

    // Level meter displays
    float displayInputLevel = 0.0f;
    float displayOutputLevel = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleEQEditor)
};
