/*
  PluginEditor.h - BassLine UI
*/

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class BassLineEditor : public juce::AudioProcessorEditor,
                       private juce::Button::Listener,
                       private juce::ComboBox::Listener,
                       private juce::Slider::Listener,
                       private juce::Timer
{
public:
    BassLineEditor (BassLineProcessor&);
    ~BassLineEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    void buttonClicked (juce::Button* button) override;
    void comboBoxChanged (juce::ComboBox* comboBox) override;
    void sliderValueChanged (juce::Slider* slider) override;
    void drawBassLine (juce::Graphics& g, const std::vector<BassEngine::Note>& bassLine);

    BassLineProcessor& processor_;

    // UI Components
    juce::TextButton generateButton_;
    juce::TextButton exportButton_;
    juce::ComboBox styleSelector_;
    juce::ComboBox keySelector_;
    juce::ComboBox scaleSelector_;
    juce::ComboBox octaveSelector_;
    juce::ComboBox waveformSelector_;
    juce::Slider attackSlider_, decaySlider_, sustainSlider_, releaseSlider_;
    juce::Slider filterSlider_;
    juce::Slider reverbSlider_, delayMixSlider_;
    juce::Label titleLabel_;
    juce::Label statsLabel_;
    juce::Label waveformLabel_, adsrLabel_, filterLabel_, octaveLabel_;
    juce::Label attackLabel_, decayLabel_, sustainLabel_, releaseLabel_;
    juce::Label effectsLabel_, reverbLabel_, delayLabel_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BassLineEditor)
};
