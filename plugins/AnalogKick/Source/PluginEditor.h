#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

class KickEditor : public juce::AudioProcessorEditor
{
public:
    KickEditor(KickProcessor&);
    ~KickEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    KickProcessor& processor_;

    juce::Label pitchLabel_, pitchEnvAmountLabel_, pitchEnvDecayLabel_;
    juce::Label decayLabel_, clickLabel_, driveLabel_, toneLabel_;

    juce::Slider pitchSlider_, pitchEnvAmountSlider_, pitchEnvDecaySlider_;
    juce::Slider decaySlider_, clickSlider_, driveSlider_, toneSlider_;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        pitchAttachment_, pitchEnvAmountAttachment_, pitchEnvDecayAttachment_,
        decayAttachment_, clickAttachment_, driveAttachment_, toneAttachment_;

    void setupRotarySlider(juce::Slider&);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KickEditor)
};
