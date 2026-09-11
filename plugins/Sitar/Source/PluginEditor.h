#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "../../../shared/preset/PresetBrowser.h"

class SitarEditor : public juce::AudioProcessorEditor
{
public:
    SitarEditor(SitarProcessor&);
    ~SitarEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    SitarProcessor& processor_;

    // Preset browser
    AudioForge::PresetBrowser presetBrowser_;

    juce::Label ragaLabel_;
    juce::ComboBox ragaCombo_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> ragaAttachment_;

    juce::Label jawariLabel_, sympatheticLabel_, attackLabel_, decayLabel_;
    juce::Label pitchBendLabel_, vibratoRateLabel_, vibratoDepthLabel_, droneLevelLabel_;

    juce::Slider jawariSlider_, sympatheticSlider_, attackSlider_, decaySlider_;
    juce::Slider pitchBendSlider_, vibratoRateSlider_, vibratoDepthSlider_, droneLevelSlider_;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> jawariAttachment_, sympatheticAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment_, decayAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pitchBendAttachment_, vibratoRateAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> vibratoDepthAttachment_, droneLevelAttachment_;

    void setupRotarySlider(juce::Slider& slider);
    void setupLinearSlider(juce::Slider& slider);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SitarEditor)
};
