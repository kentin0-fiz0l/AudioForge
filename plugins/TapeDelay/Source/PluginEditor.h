#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "../../../shared/preset/PresetBrowser.h"
class DelayEditor : public juce::AudioProcessorEditor {
public:
    DelayEditor(DelayProcessor&); ~DelayEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    DelayProcessor& processor_;

    // Preset browser
    AudioForge::PresetBrowser presetBrowser_;
    juce::Label timeLabel_, feedbackLabel_, wowLabel_, flutterLabel_, saturationLabel_, mixLabel_;
    juce::Slider timeSlider_, feedbackSlider_, wowSlider_, flutterSlider_, saturationSlider_, mixSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        timeAttachment_, feedbackAttachment_, wowAttachment_, flutterAttachment_, saturationAttachment_, mixAttachment_;
    void setupRotarySlider(juce::Slider&);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayEditor)
};
