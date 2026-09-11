#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "XYPadComponent.h"

class XYPadControllerEditor : public juce::AudioProcessorEditor {
public:
    explicit XYPadControllerEditor(XYPadControllerProcessor&);
    ~XYPadControllerEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    XYPadControllerProcessor& processor_;

    // Preset browser
    AudioForge::PresetBrowser presetBrowser_;

    // Main XY pad
    XYPadComponent xyPad_;

    // CC assignments
    juce::Label xCCLabel_, yCCLabel_, channelLabel_;
    juce::Slider xCCSlider_, yCCSlider_, channelSlider_;

    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> xCCAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> yCCAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> channelAttach_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(XYPadControllerEditor)
};
