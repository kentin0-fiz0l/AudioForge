#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "../../../shared/preset/PresetBrowser.h"

class ShakuhachiEditor : public juce::AudioProcessorEditor
{
public:
    ShakuhachiEditor(ShakuhachiProcessor&);
    ~ShakuhachiEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    ShakuhachiProcessor& processor_;

    // Preset browser
    AudioForge::PresetBrowser presetBrowser_;

    juce::Label breathPressureLabel_, embouchureLabel_, airNoiseLabel_, brightnessLabel_;
    juce::Label attackLabel_, releaseLabel_, vibratoRateLabel_, vibratoDepthLabel_;

    juce::Slider breathPressureSlider_, embouchureSlider_, airNoiseSlider_, brightnessSlider_;
    juce::Slider attackSlider_, releaseSlider_, vibratoRateSlider_, vibratoDepthSlider_;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> breathPressureAttachment_, embouchureAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> airNoiseAttachment_, brightnessAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment_, releaseAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> vibratoRateAttachment_, vibratoDepthAttachment_;

    void setupRotarySlider(juce::Slider& slider);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ShakuhachiEditor)
};
