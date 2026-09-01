#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

/**
 * OrganEmulator Editor
 *
 * Professional UI with:
 * - 9 drawbar faders (vertical)
 * - 3 Leslie controls (rotary)
 * - 2 percussion controls (rotary)
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

    // Drawbar Faders (9)
    juce::Slider drawbar16Slider;
    juce::Label drawbar16Label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> drawbar16Attachment;

    juce::Slider drawbar5_3Slider;
    juce::Label drawbar5_3Label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> drawbar5_3Attachment;

    juce::Slider drawbar8Slider;
    juce::Label drawbar8Label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> drawbar8Attachment;

    juce::Slider drawbar4Slider;
    juce::Label drawbar4Label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> drawbar4Attachment;

    juce::Slider drawbar2_3Slider;
    juce::Label drawbar2_3Label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> drawbar2_3Attachment;

    juce::Slider drawbar2Slider;
    juce::Label drawbar2Label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> drawbar2Attachment;

    juce::Slider drawbar1_3_5Slider;
    juce::Label drawbar1_3_5Label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> drawbar1_3_5Attachment;

    juce::Slider drawbar1_1_3Slider;
    juce::Label drawbar1_1_3Label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> drawbar1_1_3Attachment;

    juce::Slider drawbar1Slider;
    juce::Label drawbar1Label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> drawbar1Attachment;

    // Leslie Controls (3)
    juce::Slider leslieSpeedSlider;
    juce::Label leslieSpeedLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> leslieSpeedAttachment;

    juce::Slider leslieDepthSlider;
    juce::Label leslieDepthLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> leslieDepthAttachment;

    juce::Slider leslieMixSlider;
    juce::Label leslieMixLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> leslieMixAttachment;

    // Percussion Controls (2)
    juce::Slider percussionClickSlider;
    juce::Label percussionClickLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> percussionClickAttachment;

    juce::Slider percussionDecaySlider;
    juce::Label percussionDecayLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> percussionDecayAttachment;

    // Helper function
    void setupDrawbar(juce::Slider& slider, juce::Label& label,
                     const juce::String& labelText,
                     std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment,
                     const juce::String& parameterID);

    void setupKnob(juce::Slider& slider, juce::Label& label,
                  const juce::String& labelText,
                  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment,
                  const juce::String& parameterID);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};
