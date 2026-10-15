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
    void generateMelody();
    void updateStats();
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

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> waveformAttachment_;
    std::unique_ptr<SliderAttachment> attackAttachment_, decayAttachment_, sustainAttachment_, releaseAttachment_;
    std::unique_ptr<SliderAttachment> reverbAttachment_, delayMixAttachment_, filterCutoffAttachment_;

    // Kept here while its dialog is open: launchAsync returns at once, and
    // the dialog closes without calling back if the chooser is destroyed
    std::unique_ptr<juce::FileChooser> fileChooser_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MelodyMakerEditor)
};
