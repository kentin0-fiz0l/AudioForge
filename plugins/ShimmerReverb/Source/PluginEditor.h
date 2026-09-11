#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
class ShimmerEditor : public juce::AudioProcessorEditor {
public:
    ShimmerEditor(ShimmerProcessor&); ~ShimmerEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    ShimmerProcessor& processor_;

    // Preset browser
    AudioForge::PresetBrowser presetBrowser_;
    juce::ComboBox pitchModeBox_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> pitchModeAttachment_;
    juce::Label sizeLabel_, dampingLabel_, shimmerLabel_, feedbackLabel_, mixLabel_;
    juce::Slider sizeSlider_, dampingSlider_, shimmerSlider_, feedbackSlider_, mixSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        sizeAttachment_, dampingAttachment_, shimmerAttachment_, feedbackAttachment_, mixAttachment_;
    void setupRotarySlider(juce::Slider&);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ShimmerEditor)
};
