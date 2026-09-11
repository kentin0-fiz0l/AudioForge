#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class PolysynthEditor : public juce::AudioProcessorEditor
{
public:
    PolysynthEditor(PolysynthProcessor&);
    ~PolysynthEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    PolysynthProcessor& processor_;

    // Preset browser
    AudioForge::PresetBrowser presetBrowser_;
    juce::Label oscMixLabel_, detuneLabel_, unisonVoicesLabel_, unisonDetuneLabel_;
    juce::Label filterCutoffLabel_, filterResonanceLabel_, filterMixLabel_;
    juce::Label attackLabel_, decayLabel_, sustainLabel_, releaseLabel_;
    juce::Label chorusDepthLabel_, chorusRateLabel_;

    juce::Slider oscMixSlider_, detuneSlider_, unisonVoicesSlider_, unisonDetuneSlider_;
    juce::Slider filterCutoffSlider_, filterResonanceSlider_, filterMixSlider_;
    juce::Slider attackSlider_, decaySlider_, sustainSlider_, releaseSlider_;
    juce::Slider chorusDepthSlider_, chorusRateSlider_;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> oscMixAttachment_, detuneAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> unisonVoicesAttachment_, unisonDetuneAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffAttachment_, filterResonanceAttachment_, filterMixAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment_, decayAttachment_, sustainAttachment_, releaseAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chorusDepthAttachment_, chorusRateAttachment_;

    void setupRotarySlider(juce::Slider& slider);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PolysynthEditor)
};
