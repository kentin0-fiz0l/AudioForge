#pragma once

#include "PluginProcessor.h"
#include <juce_gui_basics/juce_gui_basics.h>

class AIProducerEditor : public juce::AudioProcessorEditor,
                          private juce::Timer {
public:
    AIProducerEditor(AIProducerProcessor&);
    ~AIProducerEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void generateButtonClicked();

    AIProducerProcessor& processor_;

    // UI Components
    juce::TextButton generateButton_;
    juce::Label statusLabel_;
    juce::Label promptLabel_;
    juce::TextEditor promptEditor_;

    juce::Label infoLabel1_;
    juce::Label infoLabel2_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AIProducerEditor)
};
