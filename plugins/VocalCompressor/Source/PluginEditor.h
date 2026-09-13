#pragma once

#include "PluginProcessor.h"
#include <juce_gui_basics/juce_gui_basics.h>

class VocalCompressorEditor : public juce::AudioProcessorEditor,
                               private juce::Timer {
public:
    VocalCompressorEditor(VocalCompressorProcessor&);
    ~VocalCompressorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    VocalCompressorProcessor& processor_;

    // UI Components
    juce::Slider thresholdSlider_, ratioSlider_, attackSlider_, releaseSlider_;
    juce::Slider kneeSlider_, makeupSlider_, parallelMixSlider_;
    juce::ComboBox detectionBox_;

    juce::Label thresholdLabel_, ratioLabel_, attackLabel_, releaseLabel_;
    juce::Label kneeLabel_, makeupLabel_, parallelMixLabel_, detectionLabel_;
    juce::Label grLabel_, outputLabel_;

    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ratioAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> kneeAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> makeupAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> parallelMixAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> detectionAttachment_;

    float displayGainReduction_ = 0.0f;
    float displayOutputLevel_ = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VocalCompressorEditor)
};
