#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "../../../shared/preset/PresetBrowser.h"

class AcousticBassEditor : public juce::AudioProcessorEditor
{
public:
    AcousticBassEditor(AcousticBassProcessor&);
    ~AcousticBassEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    AcousticBassProcessor& processor_;

    // Preset browser
    AudioForge::PresetBrowser presetBrowser_;

    juce::Label bassTypeLabel_, playStyleLabel_;
    juce::ComboBox bassTypeCombo_, playStyleCombo_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> bassTypeAttachment_, playStyleAttachment_;

    juce::Label toneLabel_, attackLabel_, decayLabel_, fretNoiseLabel_;
    juce::Slider toneSlider_, attackSlider_, decaySlider_, fretNoiseSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toneAttachment_, attackAttachment_, decayAttachment_, fretNoiseAttachment_;

    void setupRotarySlider(juce::Slider& slider);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AcousticBassEditor)
};
