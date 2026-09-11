#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "../../../shared/preset/PresetBrowser.h"

class CompressorEditor : public juce::AudioProcessorEditor, private juce::Timer {
public:
    CompressorEditor(CompressorProcessor&);
    ~CompressorEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
private:
    CompressorProcessor& processor_;

    // Preset browser
    AudioForge::PresetBrowser presetBrowser_;
    juce::ComboBox modeBox_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modeAttachment_;
    juce::Label thresholdLabel_, ratioLabel_, attackLabel_, releaseLabel_, kneeLabel_, makeupLabel_, mixLabel_;
    juce::Slider thresholdSlider_, ratioSlider_, attackSlider_, releaseSlider_, kneeSlider_, makeupSlider_, mixSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        thresholdAttachment_, ratioAttachment_, attackAttachment_, releaseAttachment_,
        kneeAttachment_, makeupAttachment_, mixAttachment_;
    float currentGainReduction_ = 0.0f;
    void setupRotarySlider(juce::Slider&);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorEditor)
};
