#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "../../../shared/preset/PresetBrowser.h"
class ChorusEditor : public juce::AudioProcessorEditor {
public:
    ChorusEditor(ChorusProcessor&); ~ChorusEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    ChorusProcessor& processor_;

    // Preset browser
    AudioForge::PresetBrowser presetBrowser_;
    juce::Label rateLabel_, depthLabel_, centreDelayLabel_, feedbackLabel_, mixLabel_;
    juce::Slider rateSlider_, depthSlider_, centreDelaySlider_, feedbackSlider_, mixSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        rateAttachment_, depthAttachment_, centreDelayAttachment_, feedbackAttachment_, mixAttachment_;
    void setupRotarySlider(juce::Slider&);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChorusEditor)
};
