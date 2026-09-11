#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class BrassSectionEditor : public juce::AudioProcessorEditor
{
public:
    BrassSectionEditor(BrassSectionProcessor&);
    ~BrassSectionEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    BrassSectionProcessor& processor_;

    // Preset browser
    AudioForge::PresetBrowser presetBrowser_;

    // Instrument and articulation selection
    juce::Label instrumentLabel_;
    juce::ComboBox instrumentCombo_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> instrumentAttachment_;

    juce::Label articulationLabel_;
    juce::ComboBox articulationCombo_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> articulationAttachment_;

    // Tone controls
    juce::Label brightnessLabel_;
    juce::Slider brightnessSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> brightnessAttachment_;

    juce::Label breathLabel_;
    juce::Slider breathSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> breathAttachment_;

    juce::Label vibratoDepthLabel_;
    juce::Slider vibratoDepthSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> vibratoDepthAttachment_;

    juce::Label vibratoRateLabel_;
    juce::Slider vibratoRateSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> vibratoRateAttachment_;

    juce::Label sectionLabel_;
    juce::Slider sectionSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sectionAttachment_;

    void setupRotarySlider(juce::Slider& slider);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BrassSectionEditor)
};
