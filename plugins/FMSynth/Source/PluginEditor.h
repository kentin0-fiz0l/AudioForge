#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

/**
 * FM Synth Plugin Editor
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

    // Modulator section
    juce::Slider modRatioSlider;
    juce::Label modRatioLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modRatioAttachment;

    juce::Slider modDepthSlider;
    juce::Label modDepthLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modDepthAttachment;

    // Carrier ADSR
    juce::Slider carrierAttackSlider;
    juce::Label carrierAttackLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> carrierAttackAttachment;

    juce::Slider carrierDecaySlider;
    juce::Label carrierDecayLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> carrierDecayAttachment;

    juce::Slider carrierSustainSlider;
    juce::Label carrierSustainLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> carrierSustainAttachment;

    juce::Slider carrierReleaseSlider;
    juce::Label carrierReleaseLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> carrierReleaseAttachment;

    // Modulator ADSR
    juce::Slider modAttackSlider;
    juce::Label modAttackLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modAttackAttachment;

    juce::Slider modDecaySlider;
    juce::Label modDecayLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modDecayAttachment;

    juce::Slider modSustainSlider;
    juce::Label modSustainLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modSustainAttachment;

    juce::Slider modReleaseSlider;
    juce::Label modReleaseLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modReleaseAttachment;

    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText,
                    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment,
                    const juce::String& parameterID);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};
