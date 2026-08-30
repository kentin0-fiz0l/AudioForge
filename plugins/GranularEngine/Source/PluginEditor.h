#pragma once

#include "PluginProcessor.h"
#include <juce_audio_processors/juce_audio_processors.h>

/**
 * GranularEngine Plugin Editor
 *
 * User interface for the GranularEngine granular synthesis plugin.
 */
class GranularEngineEditor : public juce::AudioProcessorEditor,
                             private juce::Timer
{
public:
    GranularEngineEditor(GranularEngineProcessor&);
    ~GranularEngineEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

    //==============================================================================
    // Mouse interaction
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;

private:
    //==============================================================================
    // Visualization helpers
    void paintWaveform(juce::Graphics& g, juce::Rectangle<int> bounds);
    void paintWindowPreview(juce::Graphics& g, juce::Rectangle<int> bounds);
    void paintGrainActivity(juce::Graphics& g, juce::Rectangle<int> bounds);

    // UI bounds
    juce::Rectangle<int> waveformBounds;
    juce::Rectangle<int> windowPreviewBounds;
    juce::Rectangle<int> activityBounds;

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
