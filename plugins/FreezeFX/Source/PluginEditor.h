#pragma once

#include "PluginProcessor.h"
#include <juce_audio_processors/juce_audio_processors.h>

/**
 * FreezeFX Plugin Editor
 *
 * User interface for the FreezeFX spectral freezing plugin.
 */
class FreezeFXEditor : public juce::AudioProcessorEditor,
                        private juce::Timer
{
public:
    FreezeFXEditor(FreezeFXProcessor&);
    ~FreezeFXEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    // Reference to processor
    FreezeFXProcessor& audioProcessor;

    // UI Components
    juce::TextButton freezeButton;
    juce::Label freezeLabel;

    juce::Slider freezeMixSlider;
    juce::Label freezeMixLabel;

    juce::ComboBox fftSizeSelector;
    juce::Label fftSizeLabel;

    juce::ComboBox overlapSelector;
    juce::Label overlapLabel;

    juce::Slider phaseRandomSlider;
    juce::Label phaseRandomLabel;

    juce::Slider phaseSpeedSlider;
    juce::Label phaseSpeedLabel;

    juce::Slider spectralBlurSlider;
    juce::Label spectralBlurLabel;

    juce::Slider highPassSlider;
    juce::Label highPassLabel;

    juce::Slider lowPassSlider;
    juce::Label lowPassLabel;

    //==============================================================================
    // Spectrum Visualizer
    void paintSpectrum(juce::Graphics& g, juce::Rectangle<int> bounds);
    juce::Rectangle<int> spectrumArea;

    //==============================================================================
    // Parameter Attachments (thread-safe!)
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> freezeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freezeMixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> fftSizeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> overlapAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> phaseRandomAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> phaseSpeedAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> spectralBlurAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highPassAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowPassAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FreezeFXEditor)
};
