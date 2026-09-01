#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "ZoneEditor.h"
#include "WaveformDisplay.h"
#include "VelocityLayerEditor.h"
#include "SampleBrowser.h"
#include "FactoryPresets.h"

/**
 * Sampler Plugin Editor
 *
 * Professional UI for the sampler plugin with:
 * - Waveform display with loop markers
 * - Zone editor (piano keyboard)
 * - Parameter controls (ADSR, filter, etc.)
 * - Drag-and-drop sample loading
 * - Zone management (add, delete, edit)
 */
class SamplerPluginEditor : public juce::AudioProcessorEditor,
                            public juce::FileDragAndDropTarget,
                            public juce::Timer,
                            private ZoneEditor::Listener,
                            private WaveformDisplay::Listener,
                            private VelocityLayerEditor::Listener
{
public:
    SamplerPluginEditor(SamplerPluginProcessor& p);
    ~SamplerPluginEditor() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;

    //==============================================================================
    // Drag and drop
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;
    void fileDragEnter(const juce::StringArray& files, int x, int y) override;
    void fileDragExit(const juce::StringArray& files) override;

private:
    SamplerPluginProcessor& processor;

    // UI Components
    std::unique_ptr<WaveformDisplay> waveformDisplay;
    std::unique_ptr<ZoneEditor> zoneEditor;
    std::unique_ptr<VelocityLayerEditor> velocityLayerEditor;
    std::unique_ptr<SampleBrowser> sampleBrowser;

    juce::TextButton loadSampleButton;
    juce::TextButton addZoneButton;
    juce::TextButton deleteZoneButton;
    juce::TextButton addLayerButton;
    juce::TextButton deleteLayerButton;

    juce::Label memoryLabel;
    juce::Label zoneInfoLabel;
    juce::Label presetLabel;

    juce::ComboBox loopModeSelector;
    juce::Label loopModeLabel;

    juce::ComboBox presetSelector;
    juce::TextButton savePresetButton;

    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ampAttackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ampDecayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ampSustainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ampReleaseAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterResonanceAttachment;

    juce::Slider ampAttackSlider;
    juce::Slider ampDecaySlider;
    juce::Slider ampSustainSlider;
    juce::Slider ampReleaseSlider;
    juce::Slider filterCutoffSlider;
    juce::Slider filterResonanceSlider;

    juce::Label ampAttackLabel;
    juce::Label ampDecayLabel;
    juce::Label ampSustainLabel;
    juce::Label ampReleaseLabel;
    juce::Label filterCutoffLabel;
    juce::Label filterResonanceLabel;

    // Drag and drop state
    bool isDraggingFile = false;

    //==============================================================================
    // ZoneEditor::Listener
    void zoneSelectionChanged(int zoneIndex) override;
    void zoneRangeChanged(int zoneIndex, int newMin, int newMax) override;
    void zoneAdded(int zoneIndex) override;
    void zoneDeleted(int zoneIndex) override;

    //==============================================================================
    // WaveformDisplay::Listener
    void loopPointsChanged(double loopStartSec, double loopEndSec) override;

    //==============================================================================
    // VelocityLayerEditor::Listener
    void velocityLayerChanged(int layerIndex) override;

    //==============================================================================
    // Timer
    void timerCallback() override;

    //==============================================================================
    // Helpers
    void loadSampleFile(const juce::File& file);
    void updateMemoryDisplay();
    void updateZoneInfo();
    void updateVelocityLayerEditor();
    void loadPreset(int presetIndex);
    void populatePresetSelector();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SamplerPluginEditor)
};
