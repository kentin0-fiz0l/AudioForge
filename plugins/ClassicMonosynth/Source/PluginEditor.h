#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class ClassicMonosynthEditor : public juce::AudioProcessorEditor
{
public:
    ClassicMonosynthEditor(ClassicMonosynthProcessor&);
    ~ClassicMonosynthEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    ClassicMonosynthProcessor& processor_;

    // Oscillator controls
    juce::Label osc1LevelLabel_, osc2LevelLabel_, osc3LevelLabel_;
    juce::Label osc2DetuneLabel_, osc3DetuneLabel_, pulseWidthLabel_;
    juce::Slider osc1LevelSlider_, osc2LevelSlider_, osc3LevelSlider_;
    juce::Slider osc2DetuneSlider_, osc3DetuneSlider_, pulseWidthSlider_;
    juce::ToggleButton oscSyncButton_;

    // Filter controls
    juce::Label filterCutoffLabel_, filterResonanceLabel_, filterEnvAmountLabel_;
    juce::Slider filterCutoffSlider_, filterResonanceSlider_, filterEnvAmountSlider_;

    // Envelope controls
    juce::Label filterAttackLabel_, filterDecayLabel_, filterSustainLabel_, filterReleaseLabel_;
    juce::Label ampAttackLabel_, ampDecayLabel_, ampSustainLabel_, ampReleaseLabel_;
    juce::Slider filterAttackSlider_, filterDecaySlider_, filterSustainSlider_, filterReleaseSlider_;
    juce::Slider ampAttackSlider_, ampDecaySlider_, ampSustainSlider_, ampReleaseSlider_;

    // LFO controls
    juce::Label lfo1RateLabel_, lfo1AmountLabel_, lfo2RateLabel_, lfo2AmountLabel_;
    juce::Slider lfo1RateSlider_, lfo1AmountSlider_, lfo2RateSlider_, lfo2AmountSlider_;

    // Glide
    juce::Label glideTimeLabel_;
    juce::Slider glideTimeSlider_;

    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1LevelAttachment_, osc2LevelAttachment_, osc3LevelAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc2DetuneAttachment_, osc3DetuneAttachment_, pulseWidthAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> oscSyncAttachment_;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffAttachment_, filterResonanceAttachment_, filterEnvAmountAttachment_;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterAttackAttachment_, filterDecayAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterSustainAttachment_, filterReleaseAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ampAttackAttachment_, ampDecayAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ampSustainAttachment_, ampReleaseAttachment_;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfo1RateAttachment_, lfo1AmountAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfo2RateAttachment_, lfo2AmountAttachment_;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> glideTimeAttachment_;

    void setupRotarySlider(juce::Slider& slider);
    void setupLinearSlider(juce::Slider& slider);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClassicMonosynthEditor)
};
