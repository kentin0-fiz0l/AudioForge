#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "../../../shared/preset/PresetBrowser.h"
class TomEditor : public juce::AudioProcessorEditor {
public:
    TomEditor(TomProcessor&);
    ~TomEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    TomProcessor& processor_;

    // Preset browser
    AudioForge::PresetBrowser presetBrowser_;
    juce::Label pitchLabel_, pitchEnvLabel_, decayLabel_, bendLabel_, noiseLabel_;
    juce::Slider pitchSlider_, pitchEnvSlider_, decaySlider_, bendSlider_, noiseSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        pitchAttachment_, pitchEnvAttachment_, decayAttachment_, bendAttachment_, noiseAttachment_;
    void setupRotarySlider(juce::Slider&);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TomEditor)
};
