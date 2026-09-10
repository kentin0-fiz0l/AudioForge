#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "../../../midi/MIDILearnSlider.h"

class MIDIArpeggiatorEditor : public juce::AudioProcessorEditor {
public:
    explicit MIDIArpeggiatorEditor(MIDIArpeggiatorProcessor&);
    ~MIDIArpeggiatorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    MIDIArpeggiatorProcessor& processor_;

    // Pattern & Rate
    juce::ComboBox patternCombo_;
    juce::ComboBox rateCombo_;
    juce::Label patternLabel_, rateLabel_;

    // Sliders with MIDI Learn
    AudioForge::MIDILearnSlider octavesSlider_;
    AudioForge::MIDILearnSlider gateSlider_;
    AudioForge::MIDILearnSlider swingSlider_;
    AudioForge::MIDILearnSlider fixedVelSlider_;

    juce::Label octavesLabel_, gateLabel_, swingLabel_, fixedVelLabel_;

    // Toggles
    juce::ToggleButton latchButton_{"Latch"};
    juce::ToggleButton useInputVelButton_{"Use Input Velocity"};

    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> patternAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> rateAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> octavesAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gateAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> swingAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fixedVelAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> latchAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> useInputVelAttach_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDIArpeggiatorEditor)
};
