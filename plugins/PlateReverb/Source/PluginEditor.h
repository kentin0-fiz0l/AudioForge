#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "../../../shared/preset/PresetBrowser.h"
class ReverbEditor : public juce::AudioProcessorEditor {
public:
    ReverbEditor(ReverbProcessor&);
    ~ReverbEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    ReverbProcessor& processor_;

    // Preset browser
    AudioForge::PresetBrowser presetBrowser_;
    juce::Label sizeLabel_, dampingLabel_, widthLabel_, predelayLabel_, mixLabel_;
    juce::Slider sizeSlider_, dampingSlider_, widthSlider_, predelaySlider_, mixSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        sizeAttachment_, dampingAttachment_, widthAttachment_, predelayAttachment_, mixAttachment_;
    void setupRotarySlider(juce::Slider&);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverbEditor)
};
