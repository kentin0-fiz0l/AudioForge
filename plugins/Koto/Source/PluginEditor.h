#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "../../../shared/preset/PresetBrowser.h"

class KotoEditor : public juce::AudioProcessorEditor
{
public:
    KotoEditor(KotoProcessor&);
    ~KotoEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    KotoProcessor& processor_;

    // Preset browser
    AudioForge::PresetBrowser presetBrowser_;

    juce::Label scaleLabel_, playStyleLabel_;
    juce::ComboBox scaleCombo_, playStyleCombo_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> scaleAttachment_, playStyleAttachment_;

    juce::Label bodyResonanceLabel_, attackLabel_, decayLabel_, toneLabel_;
    juce::Label pitchBendLabel_, tremoloRateLabel_;

    juce::Slider bodyResonanceSlider_, attackSlider_, decaySlider_, toneSlider_;
    juce::Slider pitchBendSlider_, tremoloRateSlider_;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bodyResonanceAttachment_, attackAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment_, toneAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pitchBendAttachment_, tremoloRateAttachment_;

    void setupRotarySlider(juce::Slider& slider);
    void setupLinearSlider(juce::Slider& slider);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KotoEditor)
};
