#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

/**
 * PadSynth Editor
 *
 * Professional UI with sections:
 * - Oscillator (waveform, unison, detune, width)
 * - Filter
 * - Envelope
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

    // Oscillator Section
    juce::Slider waveformSlider;
    juce::Label waveformLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> waveformAttachment;

    juce::Slider unisonVoicesSlider;
    juce::Label unisonVoicesLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> unisonVoicesAttachment;

    juce::Slider detuneSlider;
    juce::Label detuneLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> detuneAttachment;

    juce::Slider stereoWidthSlider;
    juce::Label stereoWidthLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> stereoWidthAttachment;

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

    // Envelope Section
    juce::Slider attackSlider;
    juce::Label attackLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;

    juce::Slider decaySlider;
    juce::Label decayLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment;

    juce::Slider sustainSlider;
    juce::Label sustainLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAttachment;

    juce::Slider releaseSlider;
    juce::Label releaseLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;

    // Helper function
    void setupSlider(juce::Slider& slider, juce::Label& label,
                    const juce::String& labelText,
                    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment,
                    const juce::String& parameterID);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};
