#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

class FMEditor : public juce::AudioProcessorEditor
{
public:
    FMEditor(FMProcessor&);
    ~FMEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    FMProcessor& processor_;

    juce::Label algorithmLabel_, feedbackLabel_;
    juce::Slider algorithmSlider_, feedbackSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> algorithmAttachment_, feedbackAttachment_;

    juce::Label opLabel_[4];
    juce::Label levelLabel_[4], ratioLabel_[4], attackLabel_[4], decayLabel_[4], sustainLabel_[4], releaseLabel_[4];
    juce::Slider levelSlider_[4], ratioSlider_[4], attackSlider_[4], decaySlider_[4], sustainSlider_[4], releaseSlider_[4];
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> levelAttachment_[4], ratioAttachment_[4],
        attackAttachment_[4], decayAttachment_[4], sustainAttachment_[4], releaseAttachment_[4];

    void setupRotarySlider(juce::Slider&);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FMEditor)
};
