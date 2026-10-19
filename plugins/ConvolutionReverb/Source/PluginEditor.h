#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
class ConvolutionReverbEditor : public juce::AudioProcessorEditor {
public:
    ConvolutionReverbEditor(ConvolutionReverbProcessor&);
    ~ConvolutionReverbEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    ConvolutionReverbProcessor& processor_;

    juce::Label mixLabel_, preDelayLabel_, lowCutLabel_, highCutLabel_, outputLabel_, irLabel_;
    juce::Slider mixSlider_, preDelaySlider_, lowCutSlider_, highCutSlider_, outputSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        mixAttachment_, preDelayAttachment_, lowCutAttachment_, highCutAttachment_, outputAttachment_;
    juce::ToggleButton bypassButton_ { "Bypass" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment_;
    juce::TextButton loadButton_ { "Load IR..." };
    std::unique_ptr<juce::FileChooser> chooser_;
    void setupRotarySlider(juce::Slider&);
    void showImpulseResponse();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConvolutionReverbEditor)
};
