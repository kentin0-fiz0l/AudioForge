#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

class SnareEditor : public juce::AudioProcessorEditor
{
public:
    SnareEditor(SnareProcessor&);
    ~SnareEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    SnareProcessor& processor_;

    // Preset browser
    AudioForge::PresetBrowser presetBrowser_;

    juce::Label toneLabel_, snappyLabel_, decayLabel_, snareDecayLabel_, pitchEnvLabel_, snapLabel_;
    juce::Slider toneSlider_, snappySlider_, decaySlider_, snareDecaySlider_, pitchEnvSlider_, snapSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        toneAttachment_, snappyAttachment_, decayAttachment_, snareDecayAttachment_, pitchEnvAttachment_, snapAttachment_;

    void setupRotarySlider(juce::Slider&);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SnareEditor)
};
