#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "../../../midi/MIDILearnSlider.h"
#include "../../../shared/preset/PresetBrowser.h"

class MacroControllerEditor : public juce::AudioProcessorEditor, private juce::Timer {
public:
    explicit MacroControllerEditor(MacroControllerProcessor&);
    ~MacroControllerEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    MacroControllerProcessor& processor_;

    // Preset browser
    AudioForge::PresetBrowser presetBrowser_;

    // Main macro knob
    AudioForge::MIDILearnSlider macroSlider_;
    juce::Label macroLabel_;

    // Channel selector
    juce::Slider channelSlider_;
    juce::Label channelLabel_;

    // Target indicators (visual only)
    struct TargetDisplay {
        juce::Label nameLabel;
        juce::Label valueLabel;
    };
    std::array<TargetDisplay, 8> targetDisplays_;

    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> macroAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> channelAttach_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MacroControllerEditor)
};
