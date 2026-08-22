/**
 * Reverb Audio Processor Editor
 *
 * UI with controls for room size, damping, width, pre-delay, mix, and freeze
 */

#pragma once

#include "PluginProcessor.h"

class ReverbAudioProcessorEditor : public juce::AudioProcessorEditor,
                                    private juce::Timer
{
public:
    ReverbAudioProcessorEditor(ReverbAudioProcessor&);
    ~ReverbAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    ReverbAudioProcessor& audioProcessor;

    // Parameter controls
    juce::Slider roomSizeSlider;
    juce::Label roomSizeLabel;

    juce::Slider dampingSlider;
    juce::Label dampingLabel;

    juce::Slider widthSlider;
    juce::Label widthLabel;

    juce::Slider preDelaySlider;
    juce::Label preDelayLabel;

    juce::Slider mixSlider;
    juce::Label mixLabel;

    juce::ToggleButton freezeButton;

    // Level meters
    juce::Rectangle<int> inputMeterBounds;
    juce::Rectangle<int> outputMeterBounds;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverbAudioProcessorEditor)
};
