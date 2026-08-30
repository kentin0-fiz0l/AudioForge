#include "PluginEditor.h"

GranularEngineEditor::GranularEngineEditor(GranularEngineProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Set window size
    setSize(600, 400);

    // Grain Size Slider
    grainSizeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    grainSizeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    grainSizeSlider.setRange(10.0, 500.0, 1.0);
    grainSizeSlider.setValue(50.0);
    addAndMakeVisible(grainSizeSlider);

    grainSizeLabel.setText("Grain Size (ms)", juce::dontSendNotification);
    grainSizeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(grainSizeLabel);

    // Dry/Wet Slider
    dryWetSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    dryWetSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    dryWetSlider.setRange(0.0, 100.0, 1.0);
    dryWetSlider.setValue(100.0);
    addAndMakeVisible(dryWetSlider);

    dryWetLabel.setText("Dry/Wet (%)", juce::dontSendNotification);
    dryWetLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(dryWetLabel);

    // Parameter connections (direct for Phase 1)
    grainSizeSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[0];
        param->setValueNotifyingHost(grainSizeSlider.getValue() / 500.0f);
    };

    dryWetSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[1];
        param->setValueNotifyingHost(dryWetSlider.getValue() / 100.0f);
    };
}

GranularEngineEditor::~GranularEngineEditor()
{
}

void GranularEngineEditor::paint(juce::Graphics& g)
{
    // Background
    g.fillAll(juce::Colours::darkslategrey);

    // Title
    g.setColour(juce::Colours::white);
    g.setFont(28.0f);
    g.drawText("GranularEngine", 0, 10, getWidth(), 40, juce::Justification::centred);

    // Subtitle
    g.setFont(14.0f);
    g.setColour(juce::Colours::lightgrey);
    g.drawText("Phase 1: Buffer Management & Grain Extraction", 0, 45, getWidth(), 20, juce::Justification::centred);

    // Section header
    g.setFont(16.0f);
    g.setColour(juce::Colours::white);
    g.drawText("GRAIN PARAMETERS", 20, 100, 300, 30, juce::Justification::centredLeft);
}

void GranularEngineEditor::resized()
{
    int y = 150;
    int sliderSize = 100;
    int margin = 50;

    // Grain Size
    grainSizeSlider.setBounds(margin, y, sliderSize, sliderSize);
    grainSizeLabel.setBounds(margin, y + sliderSize, sliderSize, 20);

    // Dry/Wet
    dryWetSlider.setBounds(margin + 150, y, sliderSize, sliderSize);
    dryWetLabel.setBounds(margin + 150, y + sliderSize, sliderSize, 20);
}
