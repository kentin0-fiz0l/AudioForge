#pragma once

#include "PluginProcessor.h"
#include <juce_audio_processors/juce_audio_processors.h>

/**
 * GranularEngine Plugin Editor
 *
 * User interface for the GranularEngine granular synthesis plugin.
 */
class GranularEngineEditor : public juce::AudioProcessorEditor
{
public:
    GranularEngineEditor(GranularEngineProcessor&);
    ~GranularEngineEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // Reference to processor
    GranularEngineProcessor& audioProcessor;

    // UI Components (Phase 1: Basic)
    juce::Slider grainSizeSlider;
    juce::Label grainSizeLabel;

    juce::Slider dryWetSlider;
    juce::Label dryWetLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GranularEngineEditor)
};
