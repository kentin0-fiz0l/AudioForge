/*
  PluginEditor.h - MelodyMaker UI
*/

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class MelodyMakerEditor : public juce::AudioProcessorEditor,
                          private juce::Button::Listener,
                          private juce::ComboBox::Listener,
                          private juce::Slider::Listener,
                          private juce::Timer
{
public:
    MelodyMakerEditor (MelodyMakerProcessor&);
    ~MelodyMakerEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    void buttonClicked (juce::Button* button) override;
    void comboBoxChanged (juce::ComboBox* comboBox) override;
    void sliderValueChanged (juce::Slider* slider) override;
    void drawMelody (juce::Graphics& g, const std::vector<MelodyEngine::Note>& melody);

    MelodyMakerProcessor& processor_;

    // UI Components
    juce::TextButton generateButton_;
    juce::TextButton exportButton_;
    juce::ComboBox styleSelector_;
    juce::ComboBox keySelector_;
    juce::ComboBox scaleSelector_;
    juce::ComboBox waveformSelector_;
    juce::Slider attackSlider_, decaySlider_, sustainSlider_, releaseSlider_;
    juce::Slider reverbSlider_, delayTimeSlider_, delayFeedbackSlider_, delayMixSlider_;
    juce::Slider filterCutoffSlider_, filterResSlider_;
    juce::Label titleLabel_;
    juce::Label statsLabel_;
    juce::Label waveformLabel_, adsrLabel_;
    juce::Label attackLabel_, decayLabel_, sustainLabel_, releaseLabel_;
    juce::Label effectsLabel_, reverbLabel_, delayLabel_, filterLabel_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MelodyMakerEditor)
};
