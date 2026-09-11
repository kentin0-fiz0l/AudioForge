#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "../../../shared/preset/PresetBrowser.h"

class MIDICCMapperEditor : public juce::AudioProcessorEditor {
public:
    explicit MIDICCMapperEditor(MIDICCMapperProcessor&);
    ~MIDICCMapperEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    MIDICCMapperProcessor& processor_;

    // Preset browser
    AudioForge::PresetBrowser presetBrowser_;

    // Column headers
    juce::Label headerLabels_[6];

    // Mapping rows (simplified - showing status only)
    struct MappingRow {
        juce::Label indexLabel;
        juce::Label statusLabel;
        juce::Label inputLabel;
        juce::Label outputLabel;
        juce::Label rangeLabel;
    };
    std::array<MappingRow, 8> rows_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDICCMapperEditor)
};
