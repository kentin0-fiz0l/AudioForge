#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
class HiHatEditor : public juce::AudioProcessorEditor {
public:
    HiHatEditor(HiHatProcessor&);
    ~HiHatEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    HiHatProcessor& processor_;
    juce::Label toneLabel_, decayLabel_, metallicLabel_, opennessLabel_;
    juce::Slider toneSlider_, decaySlider_, metallicSlider_, opennessSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        toneAttachment_, decayAttachment_, metallicAttachment_, opennessAttachment_;
    void setupRotarySlider(juce::Slider&);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HiHatEditor)
};
