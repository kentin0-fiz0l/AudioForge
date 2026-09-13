#pragma once

#include "PluginProcessor.h"
#include <juce_gui_basics/juce_gui_basics.h>

class VocalDeEsserEditor : public juce::AudioProcessorEditor,
                           private juce::Timer {
public:
    VocalDeEsserEditor(VocalDeEsserProcessor&);
    ~VocalDeEsserEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    VocalDeEsserProcessor& processor_;

    // UI Components
    juce::Slider frequencySlider_, bandwidthSlider_;
    juce::Slider thresholdSlider_, ratioSlider_;
    juce::Slider attackSlider_, releaseSlider_, mixSlider_;

    juce::Label frequencyLabel_, bandwidthLabel_;
    juce::Label thresholdLabel_, ratioLabel_;
    juce::Label attackLabel_, releaseLabel_, mixLabel_;
    juce::Label grLabel_;

    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frequencyAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bandwidthAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ratioAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment_;

    float displayGainReduction_ = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VocalDeEsserEditor)
};
