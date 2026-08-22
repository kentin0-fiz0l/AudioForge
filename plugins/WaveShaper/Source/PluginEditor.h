#pragma once

#include "PluginProcessor.h"

class WaveShaperAudioProcessorEditor : public juce::AudioProcessorEditor,
                                       private juce::Timer
{
public:
    WaveShaperAudioProcessorEditor (WaveShaperAudioProcessor&);
    ~WaveShaperAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    WaveShaperAudioProcessor& audioProcessor;

    // Parameter controls
    juce::Slider driveSlider;
    juce::Label driveLabel;

    juce::Slider mixSlider;
    juce::Label mixLabel;

    juce::Slider toneSlider;
    juce::Label toneLabel;

    juce::ComboBox shapeSelector;
    juce::Label shapeLabel;

    juce::Slider outputGainSlider;
    juce::Label outputGainLabel;

    // Metering
    juce::Label inputMeterLabel;
    juce::Label outputMeterLabel;

    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> shapeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputGainAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveShaperAudioProcessorEditor)
};
