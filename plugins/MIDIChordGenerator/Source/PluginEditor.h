#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "../../../midi/MIDILearnSlider.h"

class MIDIChordGeneratorEditor : public juce::AudioProcessorEditor {
public:
    explicit MIDIChordGeneratorEditor(MIDIChordGeneratorProcessor&);
    ~MIDIChordGeneratorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    MIDIChordGeneratorProcessor& processor_;

    // Chord type & voicing
    juce::ComboBox chordTypeCombo_;
    juce::ComboBox voicingCombo_;
    juce::Label chordTypeLabel_, voicingLabel_;

    // Sliders with MIDI Learn
    AudioForge::MIDILearnSlider octaveShiftSlider_;
    AudioForge::MIDILearnSlider strumSlider_;
    AudioForge::MIDILearnSlider fixedVelSlider_;

    juce::Label octaveShiftLabel_, strumLabel_, fixedVelLabel_;

    // Toggle
    juce::ToggleButton useInputVelButton_{"Use Input Velocity"};

    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> chordTypeAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> voicingAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> octaveShiftAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> strumAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fixedVelAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> useInputVelAttach_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDIChordGeneratorEditor)
};
