#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "../../../shared/preset/PresetBrowser.h"

class VocoderEditor : public juce::AudioProcessorEditor, private juce::Timer {
public:
    VocoderEditor(VocoderProcessor&);
    ~VocoderEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    VocoderProcessor& processor_;

    // Preset browser
    AudioForge::PresetBrowser presetBrowser_;
    
    juce::Label numBandsLabel_, formantLabel_, carrierLabel_, modulatorLabel_;
    juce::Label attackLabel_, releaseLabel_, mixLabel_;
    
    juce::Slider numBandsSlider_, formantSlider_, carrierSlider_, modulatorSlider_;
    juce::Slider attackSlider_, releaseSlider_, mixSlider_;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        numBandsAttachment_, formantAttachment_, carrierAttachment_, modulatorAttachment_,
        attackAttachment_, releaseAttachment_, mixAttachment_;
    
    void setupRotarySlider(juce::Slider&);
    void drawBandVisualizer(juce::Graphics& g, juce::Rectangle<int> bounds);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VocoderEditor)
};
