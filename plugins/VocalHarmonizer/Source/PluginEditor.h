#pragma once

#include "PluginProcessor.h"
#include <juce_gui_basics/juce_gui_basics.h>

class VocalHarmonizerEditor : public juce::AudioProcessorEditor {
public:
    VocalHarmonizerEditor(VocalHarmonizerProcessor&);
    ~VocalHarmonizerEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    VocalHarmonizerProcessor& processor_;

    // UI Components (15 parameters total)
    juce::Slider voicesSlider_;
    juce::Slider voice1PitchSlider_, voice2PitchSlider_, voice3PitchSlider_, voice4PitchSlider_;
    juce::Slider voice1PanSlider_, voice2PanSlider_, voice3PanSlider_, voice4PanSlider_;
    juce::ComboBox scaleBox_;
    juce::Slider formantSlider_;
    juce::Slider highPassSlider_, lowPassSlider_;
    juce::Slider mixSlider_, gainSlider_;

    juce::Label voicesLabel_;
    juce::Label voice1PitchLabel_, voice2PitchLabel_, voice3PitchLabel_, voice4PitchLabel_;
    juce::Label voice1PanLabel_, voice2PanLabel_, voice3PanLabel_, voice4PanLabel_;
    juce::Label scaleLabel_;
    juce::Label formantLabel_;
    juce::Label highPassLabel_, lowPassLabel_;
    juce::Label mixLabel_, gainLabel_;

    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> voicesAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> voice1PitchAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> voice2PitchAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> voice3PitchAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> voice4PitchAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> voice1PanAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> voice2PanAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> voice3PanAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> voice4PanAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> scaleAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> formantAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highPassAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowPassAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VocalHarmonizerEditor)
};
