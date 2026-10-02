/*
  PluginEditor.h - DrumMachine UI
*/

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class DrumMachineEditor : public juce::AudioProcessorEditor,
                          private juce::Button::Listener,
                          private juce::ComboBox::Listener,
                          private juce::Slider::Listener,
                          private juce::Timer
{
public:
    DrumMachineEditor (DrumMachineProcessor&);
    ~DrumMachineEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void mouseDown (const juce::MouseEvent& event) override;
    void mouseDrag (const juce::MouseEvent& event) override;

private:
    void buttonClicked (juce::Button* button) override;
    void comboBoxChanged (juce::ComboBox* comboBox) override;
    void sliderValueChanged (juce::Slider* slider) override;
    void drawPatternGrid (juce::Graphics& g, const DrumEngine::DrumPattern& pattern);

    // Pattern editing helpers
    void toggleHitAtPosition (int x, int y);
    void getCellFromPosition (int x, int y, int& voice, int& step);

    DrumMachineProcessor& processor_;

    // UI Components
    juce::TextButton generateButton_;
    juce::TextButton clearButton_;
    juce::TextButton exportButton_;
    juce::ComboBox styleSelector_;
    juce::Slider complexitySlider_;
    juce::Slider swingSlider_;
    juce::ToggleButton fillsToggle_;

    juce::Label titleLabel_;
    juce::Label statsLabel_;
    juce::Label complexityLabel_, swingLabel_, fillsLabel_;
    juce::Label editLabel_;

    // Grid bounds for hit detection
    float gridX_ = 40.0f;
    float gridY_ = 140.0f;
    float gridWidth_ = 0.0f;
    float gridHeight_ = 260.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DrumMachineEditor)
};
