#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "../../../midi/MIDILearnSlider.h"

class MIDIGrooveQuantizerEditor : public juce::AudioProcessorEditor {
public:
    explicit MIDIGrooveQuantizerEditor(MIDIGrooveQuantizerProcessor&);
    ~MIDIGrooveQuantizerEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    MIDIGrooveQuantizerProcessor& processor_;

    // Grid resolution
    juce::ComboBox gridCombo_;
    juce::Label gridLabel_;

    // Sliders with MIDI Learn
    AudioForge::MIDILearnSlider strengthSlider_;
    AudioForge::MIDILearnSlider swingSlider_;
    AudioForge::MIDILearnSlider humanizeTimeSlider_;
    AudioForge::MIDILearnSlider humanizeVelSlider_;

    juce::Label strengthLabel_, swingLabel_, humanizeTimeLabel_, humanizeVelLabel_;

    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> gridAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> strengthAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> swingAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> humanizeTimeAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> humanizeVelAttach_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDIGrooveQuantizerEditor)
};
