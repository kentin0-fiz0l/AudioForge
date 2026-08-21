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

    // Level meter display
    float displayLevel = 0.0f;

    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> waveformAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BasicSynthEditor)
};
