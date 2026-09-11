#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "../../../shared/preset/PresetBrowser.h"

class PhaserFlangerEditor : public juce::AudioProcessorEditor {
public:
    PhaserFlangerEditor(PhaserFlangerProcessor&);
    ~PhaserFlangerEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    PhaserFlangerProcessor& processor_;

    // Preset browser
    AudioForge::PresetBrowser presetBrowser_;
    juce::ComboBox modeBox_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modeAttachment_;
    juce::Label rateLabel_, depthLabel_, feedbackLabel_, centreFreqLabel_, mixLabel_;
    juce::Slider rateSlider_, depthSlider_, feedbackSlider_, centreFreqSlider_, mixSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        rateAttachment_, depthAttachment_, feedbackAttachment_, centreFreqAttachment_, mixAttachment_;
    void setupRotarySlider(juce::Slider&);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PhaserFlangerEditor)
};
