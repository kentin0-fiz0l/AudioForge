#pragma once

#include "PluginProcessor.h"
#include <juce_audio_processors/juce_audio_processors.h>

/**
 * BasicSynth Plugin Editor
 *
 * User interface for the BasicSynth synthesizer.
 */
class BasicSynthEditor : public juce::AudioProcessorEditor,
                         private juce::Timer
{
public:
    BasicSynthEditor(BasicSynthProcessor&);
    ~BasicSynthEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    // Reference to processor
    BasicSynthProcessor& audioProcessor;

    // UI Components
    juce::ComboBox waveformSelector;
    juce::Label waveformLabel;

    juce::Slider volumeSlider;
    juce::Label volumeLabel;

    juce::Slider attackSlider;
    juce::Label attackLabel;

    juce::Slider decaySlider;
    juce::Label decayLabel;

    juce::Slider sustainSlider;
    juce::Label sustainLabel;

    juce::Slider releaseSlider;
    juce::Label releaseLabel;

    juce::Slider filterCutoffSlider;
    juce::Label filterCutoffLabel;

    juce::Slider filterResonanceSlider;
    juce::Label filterResonanceLabel;

    juce::ComboBox filterTypeSelector;
    juce::Label filterTypeLabel;

    // Effects controls
    juce::Slider chorusRateSlider;
    juce::Label chorusRateLabel;

    juce::Slider chorusDepthSlider;
    juce::Label chorusDepthLabel;

    juce::Slider chorusMixSlider;
    juce::Label chorusMixLabel;

    juce::Slider reverbSizeSlider;
    juce::Label reverbSizeLabel;

    juce::Slider reverbDampingSlider;
    juce::Label reverbDampingLabel;

    juce::Slider reverbMixSlider;
    juce::Label reverbMixLabel;

    juce::Slider saturationDriveSlider;
    juce::Label saturationDriveLabel;

    juce::Slider saturationMixSlider;
    juce::Label saturationMixLabel;

    juce::ComboBox saturationTypeSelector;
    juce::Label saturationTypeLabel;

    // Level meter display
    float displayLevel = 0.0f;

    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> waveformAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterResonanceAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeAttachment;

    // Effects attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chorusRateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chorusDepthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chorusMixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbSizeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbDampingAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbMixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> saturationDriveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> saturationMixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> saturationTypeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BasicSynthEditor)
};
