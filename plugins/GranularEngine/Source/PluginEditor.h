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

    // UI Components
    juce::Slider grainSizeSlider;
    juce::Label grainSizeLabel;

    juce::Slider grainDensitySlider;
    juce::Label grainDensityLabel;

    juce::Slider timeStretchSlider;
    juce::Label timeStretchLabel;

    juce::Slider positionSlider;
    juce::Label positionLabel;

    juce::Slider pitchShiftSlider;
    juce::Label pitchShiftLabel;

    juce::Slider spraySlider;
    juce::Label sprayLabel;

    juce::Slider reverseSlider;
    juce::Label reverseLabel;

    juce::Slider stereoWidthSlider;
    juce::Label stereoWidthLabel;

    juce::Slider dryWetSlider;
    juce::Label dryWetLabel;

    juce::ComboBox windowTypeCombo;
    juce::Label windowTypeLabel;

    juce::Slider windowShapeSlider;
    juce::Label windowShapeLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GranularEngineEditor)
};
