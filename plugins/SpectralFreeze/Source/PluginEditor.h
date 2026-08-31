#pragma once

#include "PluginProcessor.h"
#include <juce_audio_processors/juce_audio_processors.h>

/**
 * SpectralFreeze Plugin Editor
 *
 * User interface for the SpectralFreeze spectral manipulation plugin.
 * Features real-time spectral visualization with 60 FPS updates.
 */
class SpectralFreezeEditor : public juce::AudioProcessorEditor,
                              private juce::Timer
{
public:
    SpectralFreezeEditor(SpectralFreezeProcessor&);
    ~SpectralFreezeEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    // Timer callback for 60 FPS visualization updates
    void timerCallback() override;

    //==============================================================================
    // Visualization methods
    void paintSpectrogram(juce::Graphics& g, juce::Rectangle<int> bounds);
    void paintFrozenSpectrumOverlay(juce::Graphics& g, juce::Rectangle<int> bounds);
    void paintFrequencyAxis(juce::Graphics& g, juce::Rectangle<int> bounds);

private:
    // Reference to processor
    SpectralFreezeProcessor& audioProcessor;

    // UI Components
    juce::ToggleButton freezeButton;
    juce::Label freezeLabel;

    juce::Slider blurSlider;
    juce::Label blurLabel;

    juce::Slider lowCutSlider;
    juce::Label lowCutLabel;

    juce::Slider highCutSlider;
    juce::Label highCutLabel;

    juce::Slider dryWetSlider;
    juce::Label dryWetLabel;

    // Phase 2: Advanced Effect Controls
    juce::Slider stretchSlider;
    juce::Label stretchLabel;

    juce::Slider shiftSlider;
    juce::Label shiftLabel;

    juce::Slider morphSlider;
    juce::Label morphLabel;

    juce::Slider gateSlider;
    juce::Label gateLabel;

    juce::Slider randomizeSlider;
    juce::Label randomizeLabel;

    //==============================================================================
    // Visualization data
    std::vector<std::vector<float>> spectrogramHistory;  // Rolling buffer of magnitude spectra
    static constexpr int SPECTROGRAM_HISTORY_SIZE = 200; // ~3.3 seconds at 60 FPS
    int spectrogramWriteIndex = 0;

    //==============================================================================
    // Phase 4: Preset Controls
    juce::TextButton savePresetButton;
    juce::TextButton loadPresetButton;
    juce::ComboBox factoryPresetsCombo;
    juce::Label presetsLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectralFreezeEditor)
};
