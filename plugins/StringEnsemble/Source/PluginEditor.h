#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "../../../shared/preset/PresetBrowser.h"

class StringEnsembleEditor : public juce::AudioProcessorEditor
{
public:
    StringEnsembleEditor(StringEnsembleProcessor&);
    ~StringEnsembleEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    StringEnsembleProcessor& processor_;

    // Preset browser
    AudioForge::PresetBrowser presetBrowser_;

    juce::Label stringTypeLabel_;
    juce::ComboBox stringTypeCombo_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> stringTypeAttachment_;

    juce::Label brightnessLabel_;
    juce::Slider brightnessSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> brightnessAttachment_;

    juce::Label attackLabel_;
    juce::Slider attackSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment_;

    juce::Label decayLabel_;
    juce::Slider decaySlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment_;

    juce::Label vibratoDepthLabel_;
    juce::Slider vibratoDepthSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> vibratoDepthAttachment_;

    juce::Label vibratoRateLabel_;
    juce::Slider vibratoRateSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> vibratoRateAttachment_;

    juce::Label ensembleLabel_;
    juce::Slider ensembleSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ensembleAttachment_;

    void setupRotarySlider(juce::Slider& slider);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StringEnsembleEditor)
};
