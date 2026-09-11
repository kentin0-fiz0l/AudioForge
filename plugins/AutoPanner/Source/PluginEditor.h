#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "../../../shared/preset/PresetBrowser.h"

class AutoPannerEditor : public juce::AudioProcessorEditor {
public:
    AutoPannerEditor(AutoPannerProcessor&);
    ~AutoPannerEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    AutoPannerProcessor& processor_;

    // Preset browser
    AudioForge::PresetBrowser presetBrowser_;
    juce::ComboBox waveformBox_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> waveformAttachment_;
    juce::Label rateLabel_, depthLabel_, phaseLabel_, widthLabel_;
    juce::Slider rateSlider_, depthSlider_, phaseSlider_, widthSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        rateAttachment_, depthAttachment_, phaseAttachment_, widthAttachment_;
    void setupRotarySlider(juce::Slider&);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AutoPannerEditor)
};
