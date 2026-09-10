#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

class TremoloVibratoEditor : public juce::AudioProcessorEditor {
public:
    TremoloVibratoEditor(TremoloVibratoProcessor&);
    ~TremoloVibratoEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    TremoloVibratoProcessor& processor_;
    juce::ComboBox modeBox_, waveformBox_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modeAttachment_, waveformAttachment_;
    juce::Label rateLabel_, depthLabel_, mixLabel_;
    juce::Slider rateSlider_, depthSlider_, mixSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        rateAttachment_, depthAttachment_, mixAttachment_;
    void setupRotarySlider(juce::Slider&);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TremoloVibratoEditor)
};
