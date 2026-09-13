#pragma once

#include "PluginProcessor.h"
#include <juce_gui_basics/juce_gui_basics.h>

class VocalDoublerEditor : public juce::AudioProcessorEditor {
public:
    VocalDoublerEditor(VocalDoublerProcessor&);
    ~VocalDoublerEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    VocalDoublerProcessor& processor_;

    // UI Components (9 parameters)
    juce::Slider voicesSlider_, widthSlider_, pitchVarSlider_;
    juce::Slider timingSlider_, humanizeSlider_;
    juce::Slider highPassSlider_, lowPassSlider_;
    juce::Slider mixSlider_, gainSlider_;

    juce::Label voicesLabel_, widthLabel_, pitchVarLabel_;
    juce::Label timingLabel_, humanizeLabel_;
    juce::Label highPassLabel_, lowPassLabel_;
    juce::Label mixLabel_, gainLabel_;

    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> voicesAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widthAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pitchVarAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> timingAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> humanizeAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highPassAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowPassAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VocalDoublerEditor)
};
