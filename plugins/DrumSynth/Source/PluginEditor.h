#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

/**
 * DrumSynth Editor
 *
 * Professional UI with sections:
 * - Kick drum (5 controls)
 * - Snare drum (5 controls)
 * - Hi-Hat (4 controls)
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

    // Kick Section
    juce::Slider kickPitchSlider;
    juce::Label kickPitchLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> kickPitchAttachment;

    juce::Slider kickDecaySlider;
    juce::Label kickDecayLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> kickDecayAttachment;

    juce::Slider kickClickSlider;
    juce::Label kickClickLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> kickClickAttachment;

    juce::Slider kickToneSlider;
    juce::Label kickToneLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> kickToneAttachment;

    juce::Slider kickDriveSlider;
    juce::Label kickDriveLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> kickDriveAttachment;

    // Snare Section
    juce::Slider snareTuneSlider;
    juce::Label snareTuneLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> snareTuneAttachment;

    juce::Slider snareSnapSlider;
    juce::Label snareSnapLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> snareSnapAttachment;

    juce::Slider snareToneSlider;
    juce::Label snareToneLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> snareToneAttachment;

    juce::Slider snareDecaySlider;
    juce::Label snareDecayLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> snareDecayAttachment;

    juce::Slider snareMixSlider;
    juce::Label snareMixLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> snareMixAttachment;

    // Hi-Hat Section
    juce::Slider hihatTuneSlider;
    juce::Label hihatTuneLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hihatTuneAttachment;

    juce::Slider hihatDecaySlider;
    juce::Label hihatDecayLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hihatDecayAttachment;

    juce::Slider hihatToneSlider;
    juce::Label hihatToneLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hihatToneAttachment;

    juce::Slider hihatClickSlider;
    juce::Label hihatClickLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hihatClickAttachment;

    // Helper function
    void setupSlider(juce::Slider& slider, juce::Label& label,
                    const juce::String& labelText,
                    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment,
                    const juce::String& parameterID);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};
