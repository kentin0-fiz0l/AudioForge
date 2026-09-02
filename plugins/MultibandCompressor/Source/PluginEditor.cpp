/*
  ==============================================================================

    PluginEditor.cpp
    Created: 1 Sep 2026
    Author:  AudioForge Team

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MultibandCompressorAudioProcessorEditor::MultibandCompressorAudioProcessorEditor(
    MultibandCompressorAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(900, 600);

    // Setup crossover sliders
    auto setupSlider = [this](juce::Slider& slider) {
        slider.setSliderStyle(juce::Slider::LinearHorizontal);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
        addAndMakeVisible(slider);
    };

    auto setupRotary = [this](juce::Slider& slider) {
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
        addAndMakeVisible(slider);
    };

    setupSlider(crossover1Slider);
    setupSlider(crossover2Slider);
    setupSlider(crossover3Slider);

    // Band 1
    setupRotary(band1ThresholdSlider);
    setupRotary(band1RatioSlider);
    setupRotary(band1AttackSlider);
    setupRotary(band1ReleaseSlider);
    setupRotary(band1MakeupSlider);
    band1SoloButton.setButtonText("S");
    band1BypassButton.setButtonText("B");
    band1MidSideButton.setButtonText("M/S");
    addAndMakeVisible(band1SoloButton);
    addAndMakeVisible(band1BypassButton);
    addAndMakeVisible(band1MidSideButton);

    // Band 2
    setupRotary(band2ThresholdSlider);
    setupRotary(band2RatioSlider);
    setupRotary(band2AttackSlider);
    setupRotary(band2ReleaseSlider);
    setupRotary(band2MakeupSlider);
    band2SoloButton.setButtonText("S");
    band2BypassButton.setButtonText("B");
    band2MidSideButton.setButtonText("M/S");
    addAndMakeVisible(band2SoloButton);
    addAndMakeVisible(band2BypassButton);
    addAndMakeVisible(band2MidSideButton);

    // Band 3
    setupRotary(band3ThresholdSlider);
    setupRotary(band3RatioSlider);
    setupRotary(band3AttackSlider);
    setupRotary(band3ReleaseSlider);
    setupRotary(band3MakeupSlider);
    band3SoloButton.setButtonText("S");
    band3BypassButton.setButtonText("B");
    band3MidSideButton.setButtonText("M/S");
    addAndMakeVisible(band3SoloButton);
    addAndMakeVisible(band3BypassButton);
    addAndMakeVisible(band3MidSideButton);

    // Band 4
    setupRotary(band4ThresholdSlider);
    setupRotary(band4RatioSlider);
    setupRotary(band4AttackSlider);
    setupRotary(band4ReleaseSlider);
    setupRotary(band4MakeupSlider);
    band4SoloButton.setButtonText("S");
    band4BypassButton.setButtonText("B");
    band4MidSideButton.setButtonText("M/S");
    addAndMakeVisible(band4SoloButton);
    addAndMakeVisible(band4BypassButton);
    addAndMakeVisible(band4MidSideButton);

    // Create attachments - Crossovers
    crossover1Attachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "crossover1", crossover1Slider));
    crossover2Attachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "crossover2", crossover2Slider));
    crossover3Attachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "crossover3", crossover3Slider));

    // Band 1 attachments
    band1ThresholdAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "band1Threshold", band1ThresholdSlider));
    band1RatioAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "band1Ratio", band1RatioSlider));
    band1AttackAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "band1Attack", band1AttackSlider));
    band1ReleaseAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "band1Release", band1ReleaseSlider));
    band1MakeupAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "band1Makeup", band1MakeupSlider));
    band1SoloAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(
        audioProcessor.getParameters(), "band1Solo", band1SoloButton));
    band1BypassAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(
        audioProcessor.getParameters(), "band1Bypass", band1BypassButton));
    band1MidSideAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(
        audioProcessor.getParameters(), "band1MidSide", band1MidSideButton));

    // Band 2 attachments
    band2ThresholdAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "band2Threshold", band2ThresholdSlider));
    band2RatioAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "band2Ratio", band2RatioSlider));
    band2AttackAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "band2Attack", band2AttackSlider));
    band2ReleaseAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "band2Release", band2ReleaseSlider));
    band2MakeupAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "band2Makeup", band2MakeupSlider));
    band2SoloAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(
        audioProcessor.getParameters(), "band2Solo", band2SoloButton));
    band2BypassAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(
        audioProcessor.getParameters(), "band2Bypass", band2BypassButton));
    band2MidSideAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(
        audioProcessor.getParameters(), "band2MidSide", band2MidSideButton));

    // Band 3 attachments
    band3ThresholdAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "band3Threshold", band3ThresholdSlider));
    band3RatioAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "band3Ratio", band3RatioSlider));
    band3AttackAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "band3Attack", band3AttackSlider));
    band3ReleaseAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "band3Release", band3ReleaseSlider));
    band3MakeupAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "band3Makeup", band3MakeupSlider));
    band3SoloAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(
        audioProcessor.getParameters(), "band3Solo", band3SoloButton));
    band3BypassAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(
        audioProcessor.getParameters(), "band3Bypass", band3BypassButton));
    band3MidSideAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(
        audioProcessor.getParameters(), "band3MidSide", band3MidSideButton));

    // Band 4 attachments
    band4ThresholdAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "band4Threshold", band4ThresholdSlider));
    band4RatioAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "band4Ratio", band4RatioSlider));
    band4AttackAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "band4Attack", band4AttackSlider));
    band4ReleaseAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "band4Release", band4ReleaseSlider));
    band4MakeupAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "band4Makeup", band4MakeupSlider));
    band4SoloAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(
        audioProcessor.getParameters(), "band4Solo", band4SoloButton));
    band4BypassAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(
        audioProcessor.getParameters(), "band4Bypass", band4BypassButton));
    band4MidSideAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(
        audioProcessor.getParameters(), "band4MidSide", band4MidSideButton));

    startTimerHz(30);
}

MultibandCompressorAudioProcessorEditor::~MultibandCompressorAudioProcessorEditor()
{
}

void MultibandCompressorAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1e1e1e));

    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawFittedText("AudioForge Multiband Compressor", 0, 10, getWidth(), 30,
                     juce::Justification::centred, 1);

    // Band labels
    g.setFont(14.0f);
    g.drawText("Band 1\nLow", 20, 100, 200, 40, juce::Justification::centred);
    g.drawText("Band 2\nLow-Mid", 230, 100, 200, 40, juce::Justification::centred);
    g.drawText("Band 3\nHigh-Mid", 440, 100, 200, 40, juce::Justification::centred);
    g.drawText("Band 4\nHigh", 650, 100, 200, 40, juce::Justification::centred);

    // Meters
    int meterY = 540;
    g.setFont(12.0f);

    // Band 1 GR
    g.drawText("GR:", 20, meterY, 30, 20, juce::Justification::left);
    g.setColour(juce::Colour(0xff2a2a2a));
    g.fillRect(55, meterY + 5, 150, 10);
    float gr1Width = juce::jmap(band1GR, -20.0f, 0.0f, 0.0f, 150.0f);
    g.setColour(juce::Colours::yellow);
    g.fillRect(55, meterY + 5, static_cast<int>(gr1Width), 10);

    // Band 2 GR
    g.setColour(juce::Colours::white);
    g.drawText("GR:", 230, meterY, 30, 20, juce::Justification::left);
    g.setColour(juce::Colour(0xff2a2a2a));
    g.fillRect(265, meterY + 5, 150, 10);
    float gr2Width = juce::jmap(band2GR, -20.0f, 0.0f, 0.0f, 150.0f);
    g.setColour(juce::Colours::yellow);
    g.fillRect(265, meterY + 5, static_cast<int>(gr2Width), 10);

    // Band 3 GR
    g.setColour(juce::Colours::white);
    g.drawText("GR:", 440, meterY, 30, 20, juce::Justification::left);
    g.setColour(juce::Colour(0xff2a2a2a));
    g.fillRect(475, meterY + 5, 150, 10);
    float gr3Width = juce::jmap(band3GR, -20.0f, 0.0f, 0.0f, 150.0f);
    g.setColour(juce::Colours::yellow);
    g.fillRect(475, meterY + 5, static_cast<int>(gr3Width), 10);

    // Band 4 GR
    g.setColour(juce::Colours::white);
    g.drawText("GR:", 650, meterY, 30, 20, juce::Justification::left);
    g.setColour(juce::Colour(0xff2a2a2a));
    g.fillRect(685, meterY + 5, 150, 10);
    float gr4Width = juce::jmap(band4GR, -20.0f, 0.0f, 0.0f, 150.0f);
    g.setColour(juce::Colours::yellow);
    g.fillRect(685, meterY + 5, static_cast<int>(gr4Width), 10);
}

void MultibandCompressorAudioProcessorEditor::resized()
{
    // Crossovers at top
    crossover1Slider.setBounds(20, 60, 200, 30);
    crossover2Slider.setBounds(300, 60, 200, 30);
    crossover3Slider.setBounds(580, 60, 200, 30);

    // 4-column layout (one per band)
    int colWidth = 200;
    int sliderSize = 60;
    int buttonWidth = 50;

    // Band 1 column
    int x1 = 20;
    band1ThresholdSlider.setBounds(x1 + 10, 150, sliderSize, sliderSize);
    band1RatioSlider.setBounds(x1 + 90, 150, sliderSize, sliderSize);
    band1AttackSlider.setBounds(x1 + 10, 240, sliderSize, sliderSize);
    band1ReleaseSlider.setBounds(x1 + 90, 240, sliderSize, sliderSize);
    band1MakeupSlider.setBounds(x1 + 50, 330, sliderSize, sliderSize);
    band1SoloButton.setBounds(x1 + 10, 420, buttonWidth, 25);
    band1BypassButton.setBounds(x1 + 70, 420, buttonWidth, 25);
    band1MidSideButton.setBounds(x1 + 130, 420, buttonWidth, 25);

    // Band 2 column
    int x2 = 230;
    band2ThresholdSlider.setBounds(x2 + 10, 150, sliderSize, sliderSize);
    band2RatioSlider.setBounds(x2 + 90, 150, sliderSize, sliderSize);
    band2AttackSlider.setBounds(x2 + 10, 240, sliderSize, sliderSize);
    band2ReleaseSlider.setBounds(x2 + 90, 240, sliderSize, sliderSize);
    band2MakeupSlider.setBounds(x2 + 50, 330, sliderSize, sliderSize);
    band2SoloButton.setBounds(x2 + 10, 420, buttonWidth, 25);
    band2BypassButton.setBounds(x2 + 70, 420, buttonWidth, 25);
    band2MidSideButton.setBounds(x2 + 130, 420, buttonWidth, 25);

    // Band 3 column
    int x3 = 440;
    band3ThresholdSlider.setBounds(x3 + 10, 150, sliderSize, sliderSize);
    band3RatioSlider.setBounds(x3 + 90, 150, sliderSize, sliderSize);
    band3AttackSlider.setBounds(x3 + 10, 240, sliderSize, sliderSize);
    band3ReleaseSlider.setBounds(x3 + 90, 240, sliderSize, sliderSize);
    band3MakeupSlider.setBounds(x3 + 50, 330, sliderSize, sliderSize);
    band3SoloButton.setBounds(x3 + 10, 420, buttonWidth, 25);
    band3BypassButton.setBounds(x3 + 70, 420, buttonWidth, 25);
    band3MidSideButton.setBounds(x3 + 130, 420, buttonWidth, 25);

    // Band 4 column
    int x4 = 650;
    band4ThresholdSlider.setBounds(x4 + 10, 150, sliderSize, sliderSize);
    band4RatioSlider.setBounds(x4 + 90, 150, sliderSize, sliderSize);
    band4AttackSlider.setBounds(x4 + 10, 240, sliderSize, sliderSize);
    band4ReleaseSlider.setBounds(x4 + 90, 240, sliderSize, sliderSize);
    band4MakeupSlider.setBounds(x4 + 50, 330, sliderSize, sliderSize);
    band4SoloButton.setBounds(x4 + 10, 420, buttonWidth, 25);
    band4BypassButton.setBounds(x4 + 70, 420, buttonWidth, 25);
    band4MidSideButton.setBounds(x4 + 130, 420, buttonWidth, 25);
}

void MultibandCompressorAudioProcessorEditor::timerCallback()
{
    // Update metering
    band1GR = audioProcessor.getMultibandEngine().getBandGainReductionDb(0);
    band2GR = audioProcessor.getMultibandEngine().getBandGainReductionDb(1);
    band3GR = audioProcessor.getMultibandEngine().getBandGainReductionDb(2);
    band4GR = audioProcessor.getMultibandEngine().getBandGainReductionDb(3);

    repaint();
}
