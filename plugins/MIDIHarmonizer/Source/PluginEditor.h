#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "../../../midi/MIDILearnSlider.h"

class MIDIHarmonizerEditor : public juce::AudioProcessorEditor {
public:
    explicit MIDIHarmonizerEditor(MIDIHarmonizerProcessor&);
    ~MIDIHarmonizerEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    MIDIHarmonizerProcessor& processor_;

    // Key & Scale
    juce::ComboBox keyCombo_;
    juce::ComboBox scaleCombo_;
    juce::Label keyLabel_, scaleLabel_;

    // Harmony toggles
    juce::ToggleButton thirdAboveButton_{"3rd Above"};
    juce::ToggleButton fifthAboveButton_{"5th Above"};
    juce::ToggleButton octaveAboveButton_{"Octave Above"};
    juce::ToggleButton thirdBelowButton_{"3rd Below"};
    juce::ToggleButton fifthBelowButton_{"5th Below"};
    juce::ToggleButton octaveBelowButton_{"Octave Below"};

    // Mix slider
    AudioForge::MIDILearnSlider mixSlider_;
    AudioForge::MIDILearnSlider fixedVelSlider_;
    juce::Label mixLabel_, fixedVelLabel_;

    // Velocity toggle
    juce::ToggleButton useInputVelButton_{"Use Input Velocity"};

    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> keyAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> scaleAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> thirdAboveAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> fifthAboveAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> octaveAboveAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> thirdBelowAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> fifthBelowAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> octaveBelowAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fixedVelAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> useInputVelAttach_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDIHarmonizerEditor)
};
