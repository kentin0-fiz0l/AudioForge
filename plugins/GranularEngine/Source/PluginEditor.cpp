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

    // Grain Density Slider
    grainDensitySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    grainDensitySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    grainDensitySlider.setRange(1.0, 100.0, 0.1);
    grainDensitySlider.setValue(10.0);
    addAndMakeVisible(grainDensitySlider);

    grainDensityLabel.setText("Density (g/s)", juce::dontSendNotification);
    grainDensityLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(grainDensityLabel);

    // Time Stretch Slider
    timeStretchSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    timeStretchSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    timeStretchSlider.setRange(0.25, 4.0, 0.01);
    timeStretchSlider.setSkewFactor(0.5);
    timeStretchSlider.setValue(1.0);
    addAndMakeVisible(timeStretchSlider);

    timeStretchLabel.setText("Time Stretch", juce::dontSendNotification);
    timeStretchLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(timeStretchLabel);

    // Position Slider
    positionSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    positionSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    positionSlider.setRange(0.0, 1.0, 0.01);
    positionSlider.setValue(0.5);
    addAndMakeVisible(positionSlider);

    positionLabel.setText("Position", juce::dontSendNotification);
    positionLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(positionLabel);

    // Pitch Shift Slider
    pitchShiftSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    pitchShiftSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    pitchShiftSlider.setRange(-24.0, 24.0, 0.1);
    pitchShiftSlider.setValue(0.0);
    addAndMakeVisible(pitchShiftSlider);

    pitchShiftLabel.setText("Pitch (st)", juce::dontSendNotification);
    pitchShiftLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(pitchShiftLabel);

    // Spray Slider
    spraySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    spraySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    spraySlider.setRange(0.0, 100.0, 1.0);
    spraySlider.setValue(0.0);
    addAndMakeVisible(spraySlider);

    sprayLabel.setText("Spray (%)", juce::dontSendNotification);
    sprayLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(sprayLabel);

    // Reverse Slider
    reverseSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    reverseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    reverseSlider.setRange(0.0, 100.0, 1.0);
    reverseSlider.setValue(0.0);
    addAndMakeVisible(reverseSlider);

    reverseLabel.setText("Reverse (%)", juce::dontSendNotification);
    reverseLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(reverseLabel);

    // Stereo Width Slider
    stereoWidthSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    stereoWidthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    stereoWidthSlider.setRange(0.0, 200.0, 1.0);
    stereoWidthSlider.setValue(100.0);
    addAndMakeVisible(stereoWidthSlider);

    stereoWidthLabel.setText("Width (%)", juce::dontSendNotification);
    stereoWidthLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(stereoWidthLabel);

    // Parameter connections
    grainSizeSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[0];
        param->setValueNotifyingHost((grainSizeSlider.getValue() - 10.0f) / 490.0f);
    };

    grainDensitySlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[1];
        param->setValueNotifyingHost((grainDensitySlider.getValue() - 1.0f) / 99.0f);
    };

    timeStretchSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[2];
        param->setValueNotifyingHost((timeStretchSlider.getValue() - 0.25f) / 3.75f);
    };

    positionSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[3];
        param->setValueNotifyingHost(positionSlider.getValue());
    };

    pitchShiftSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[4];
        param->setValueNotifyingHost((pitchShiftSlider.getValue() + 24.0f) / 48.0f);
    };

    spraySlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[5];
        param->setValueNotifyingHost(spraySlider.getValue() / 100.0f);
    };

    reverseSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[6];
        param->setValueNotifyingHost(reverseSlider.getValue() / 100.0f);
    };

    stereoWidthSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[7];
        param->setValueNotifyingHost(stereoWidthSlider.getValue() / 200.0f);
    };

    dryWetSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[8];
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
    g.drawText("Phase 3: Pitch Shifting & Advanced Controls", 0, 45, getWidth(), 20, juce::Justification::centred);

    // Section headers
    g.setFont(16.0f);
    g.setColour(juce::Colours::white);
    g.drawText("GRAIN PARAMETERS", 20, 100, 300, 30, juce::Justification::centredLeft);
    g.drawText("CREATIVE CONTROLS", 20, 240, 300, 30, juce::Justification::centredLeft);
}

void GranularEngineEditor::resized()
{
    int y = 130;
    int sliderSize = 85;
    int margin = 25;
    int spacing = 110;

    // Row 1: Grain parameters
    grainSizeSlider.setBounds(margin, y, sliderSize, sliderSize);
    grainSizeLabel.setBounds(margin, y + sliderSize, sliderSize, 20);

    grainDensitySlider.setBounds(margin + spacing, y, sliderSize, sliderSize);
    grainDensityLabel.setBounds(margin + spacing, y + sliderSize, sliderSize, 20);

    timeStretchSlider.setBounds(margin + spacing * 2, y, sliderSize, sliderSize);
    timeStretchLabel.setBounds(margin + spacing * 2, y + sliderSize, sliderSize, 20);

    dryWetSlider.setBounds(margin + spacing * 3, y, sliderSize, sliderSize);
    dryWetLabel.setBounds(margin + spacing * 3, y + sliderSize, sliderSize, 20);

    // Row 2: Creative controls
    y = 270;

    pitchShiftSlider.setBounds(margin, y, sliderSize, sliderSize);
    pitchShiftLabel.setBounds(margin, y + sliderSize, sliderSize, 20);

    spraySlider.setBounds(margin + spacing, y, sliderSize, sliderSize);
    sprayLabel.setBounds(margin + spacing, y + sliderSize, sliderSize, 20);

    reverseSlider.setBounds(margin + spacing * 2, y, sliderSize, sliderSize);
    reverseLabel.setBounds(margin + spacing * 2, y + sliderSize, sliderSize, 20);

    stereoWidthSlider.setBounds(margin + spacing * 3, y, sliderSize, sliderSize);
    stereoWidthLabel.setBounds(margin + spacing * 3, y + sliderSize, sliderSize, 20);

    // Position slider (horizontal at bottom)
    y = 380;
    positionLabel.setBounds(margin, y, 80, 20);
    positionSlider.setBounds(margin + 90, y, 350, 20);
}
