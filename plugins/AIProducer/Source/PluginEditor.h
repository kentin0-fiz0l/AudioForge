#pragma once

#include "PluginProcessor.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <array>

class AIProducerEditor : public juce::AudioProcessorEditor,
                          private juce::Timer {
public:
    AIProducerEditor(AIProducerProcessor&);
    ~AIProducerEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    struct Choice {
        juce::Label label;
        juce::ComboBox box;
        std::unique_ptr<ComboAttachment> attachment;
    };

    struct Part {
        juce::ToggleButton button;
        std::unique_ptr<ButtonAttachment> attachment;
    };

    AIProducerProcessor& processor_;

    std::array<Choice, 5> choices_;   // Genre, key, scale, length, part
    std::array<Part, 4> parts_;       // Drums, bass, chords, lead
    juce::TextButton variationButton_;
    juce::Label variationLabel_;
    juce::Label descriptionLabel_;
    juce::Label routingLabel_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AIProducerEditor)
};
