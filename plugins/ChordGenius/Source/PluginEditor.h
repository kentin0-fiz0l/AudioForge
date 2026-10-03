/*
  PluginEditor.h - ChordGenius UI
*/

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class ChordGeniusEditor : public juce::AudioProcessorEditor,
                          private juce::Button::Listener
{
public:
    ChordGeniusEditor (ChordGeniusProcessor&);
    ~ChordGeniusEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void buttonClicked (juce::Button* button) override;

    ChordGeniusProcessor& processor_;

    // UI Components
    juce::TextButton generateButton_;
    juce::TextButton exportButton_;
    juce::ComboBox genreSelector_;
    juce::Label keyLabel_;
    juce::Label titleLabel_;

    // Chord display
    std::vector<std::unique_ptr<juce::Label>> chordLabels_;
    std::vector<std::unique_ptr<juce::Label>> romanLabels_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChordGeniusEditor)
};
