#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

/**
 * ElectricPianoEditor - GUI for the ElectricPiano plugin
 *
 * Features:
 * - Model selector (Rhodes/Wurlitzer/Hybrid)
 * - Tone, Decay, Noise controls
 * - Velocity curve control
 * - Built-in effects (Tremolo, Chorus, Reverb)
 */
class ElectricPianoEditor : public juce::AudioProcessorEditor
{
public:
    ElectricPianoEditor(ElectricPianoProcessor&);
    ~ElectricPianoEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    ElectricPianoProcessor& processor_;

    // Model selection
    juce::Label modelLabel_;
    juce::ComboBox modelCombo_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modelAttachment_;

    // Tone controls
    juce::Label toneLabel_;
    juce::Slider toneSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toneAttachment_;

    juce::Label decayLabel_;
    juce::Slider decaySlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment_;

    juce::Label noiseLabel_;
    juce::Slider noiseSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> noiseAttachment_;

    juce::Label velocityLabel_;
    juce::Slider velocitySlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> velocityAttachment_;

    // Effects controls
    juce::Label tremoloDepthLabel_;
    juce::Slider tremoloDepthSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> tremoloDepthAttachment_;

    juce::Label tremoloRateLabel_;
    juce::Slider tremoloRateSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> tremoloRateAttachment_;

    juce::Label chorusLabel_;
    juce::Slider chorusSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chorusAttachment_;

    juce::Label reverbLabel_;
    juce::Slider reverbSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbAttachment_;

    // Helper to create rotary sliders
    void setupRotarySlider(juce::Slider& slider);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ElectricPianoEditor)
};
