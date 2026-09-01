#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

/**
 * WavetableSynth Editor
 *
 * Professional UI with sections:
 * - Wavetable control
 * - Filter section
 * - Amplitude envelope
 * - Filter envelope
 */
class PluginEditor : public juce::AudioProcessorEditor
{
public:
    PluginEditor(PluginProcessor&);
    ~PluginEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    PluginProcessor& audioProcessor;

    // Wavetable Section
    juce::Slider wtPositionSlider;
    juce::Label wtPositionLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> wtPositionAttachment;

    // Filter Section
    juce::Slider filterCutoffSlider;
    juce::Label filterCutoffLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffAttachment;

    juce::Slider filterResSlider;
    juce::Label filterResLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterResAttachment;

    juce::Slider filterTypeSlider;
    juce::Label filterTypeLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterTypeAttachment;

    juce::Slider filterEnvSlider;
    juce::Label filterEnvLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterEnvAttachment;

    // Amplitude Envelope
    juce::Slider ampAttackSlider;
    juce::Label ampAttackLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ampAttackAttachment;

    juce::Slider ampDecaySlider;
    juce::Label ampDecayLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ampDecayAttachment;

    juce::Slider ampSustainSlider;
    juce::Label ampSustainLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ampSustainAttachment;

    juce::Slider ampReleaseSlider;
    juce::Label ampReleaseLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ampReleaseAttachment;

    // Filter Envelope
    juce::Slider filtAttackSlider;
    juce::Label filtAttackLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filtAttackAttachment;

    juce::Slider filtDecaySlider;
    juce::Label filtDecayLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filtDecayAttachment;

    juce::Slider filtSustainSlider;
    juce::Label filtSustainLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filtSustainAttachment;

    juce::Slider filtReleaseSlider;
    juce::Label filtReleaseLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filtReleaseAttachment;

    // Helper function to setup sliders
    void setupSlider(juce::Slider& slider, juce::Label& label,
                    const juce::String& labelText,
                    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment,
                    const juce::String& parameterID);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};
