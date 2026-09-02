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
GateAudioProcessorEditor::GateAudioProcessorEditor(GateAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(700, 500);

    // Configure sliders
    auto setupSlider = [](juce::Slider& slider)
    {
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    };

    setupSlider(thresholdSlider);
    setupSlider(ratioSlider);
    setupSlider(rangeSlider);
    setupSlider(attackSlider);
    setupSlider(holdSlider);
    setupSlider(releaseSlider);
    setupSlider(lookaheadSlider);
    setupSlider(rmsWindowSlider);
    setupSlider(scHighPassSlider);
    setupSlider(scLowPassSlider);

    addAndMakeVisible(thresholdSlider);
    addAndMakeVisible(ratioSlider);
    addAndMakeVisible(rangeSlider);
    addAndMakeVisible(attackSlider);
    addAndMakeVisible(holdSlider);
    addAndMakeVisible(releaseSlider);
    addAndMakeVisible(lookaheadSlider);
    addAndMakeVisible(rmsWindowSlider);
    addAndMakeVisible(scHighPassSlider);
    addAndMakeVisible(scLowPassSlider);

    // Configure labels
    auto setupLabel = [](juce::Label& label, const juce::String& text)
    {
        label.setText(text, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
    };

    setupLabel(thresholdLabel, "Threshold");
    setupLabel(ratioLabel, "Ratio");
    setupLabel(rangeLabel, "Range");
    setupLabel(attackLabel, "Attack");
    setupLabel(holdLabel, "Hold");
    setupLabel(releaseLabel, "Release");
    setupLabel(lookaheadLabel, "Lookahead");
    setupLabel(rmsWindowLabel, "RMS Window");
    setupLabel(scHighPassLabel, "SC HP");
    setupLabel(scLowPassLabel, "SC LP");
    setupLabel(detectionModeLabel, "Detection");

    addAndMakeVisible(thresholdLabel);
    addAndMakeVisible(ratioLabel);
    addAndMakeVisible(rangeLabel);
    addAndMakeVisible(attackLabel);
    addAndMakeVisible(holdLabel);
    addAndMakeVisible(releaseLabel);
    addAndMakeVisible(lookaheadLabel);
    addAndMakeVisible(rmsWindowLabel);
    addAndMakeVisible(scHighPassLabel);
    addAndMakeVisible(scLowPassLabel);
    addAndMakeVisible(detectionModeLabel);

    // Configure buttons
    modeButton.setButtonText("Gate/Expander");
    sidechainEnableButton.setButtonText("Sidechain Enable");
    scListenButton.setButtonText("SC Listen");

    addAndMakeVisible(modeButton);
    addAndMakeVisible(sidechainEnableButton);
    addAndMakeVisible(scListenButton);

    // Configure combo box
    detectionModeCombo.addItem("RMS", 1);
    detectionModeCombo.addItem("Peak", 2);
    addAndMakeVisible(detectionModeCombo);

    // Create parameter attachments
    thresholdAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "threshold", thresholdSlider));
    ratioAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "ratio", ratioSlider));
    rangeAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "range", rangeSlider));
    attackAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "attack", attackSlider));
    holdAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "hold", holdSlider));
    releaseAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "release", releaseSlider));
    lookaheadAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "lookahead", lookaheadSlider));
    rmsWindowAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "rmsWindow", rmsWindowSlider));

    scHighPassAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "scHighPass", scHighPassSlider));
    scLowPassAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getParameters(), "scLowPass", scLowPassSlider));

    modeAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(
        audioProcessor.getParameters(), "mode", modeButton));
    sidechainEnableAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(
        audioProcessor.getParameters(), "sidechainEnable", sidechainEnableButton));
    scListenAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(
        audioProcessor.getParameters(), "scListen", scListenButton));

    detectionModeAttachment.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(
        audioProcessor.getParameters(), "detectionMode", detectionModeCombo));

    // Start timer for metering (30 Hz)
    startTimerHz(30);
}

GateAudioProcessorEditor::~GateAudioProcessorEditor()
{
}

//==============================================================================
void GateAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Background
    g.fillAll(juce::Colour(0xff1e1e1e));

    // Title
    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawFittedText("AudioForge Gate", 0, 10, getWidth(), 30, juce::Justification::centred, 1);

    g.setFont(14.0f);
    g.drawFittedText("Dynamic Noise Control", 0, 40, getWidth(), 20, juce::Justification::centred, 1);

    // Sidechain section background
    g.setColour(juce::Colour(0xff2a2a2a));
    g.fillRect(10, 330, getWidth() - 20, 90);

    g.setColour(juce::Colours::white);
    g.setFont(16.0f);
    g.drawText("Sidechain", 20, 335, 100, 20, juce::Justification::left);

    // Metering section
    g.setFont(12.0f);
    int meterY = 430;

    // Input meter
    g.drawText("Input:", 10, meterY, 50, 20, juce::Justification::left);
    g.setColour(juce::Colour(0xff2a2a2a));
    g.fillRect(70, meterY + 5, 180, 10);

    float inputWidth = juce::jmap(inputLevel, -60.0f, 0.0f, 0.0f, 180.0f);
    g.setColour(juce::Colours::green);
    g.fillRect(70, meterY + 5, static_cast<int>(inputWidth), 10);

    g.setColour(juce::Colours::white);
    g.drawText(juce::String(inputLevel, 1) + " dB", 260, meterY, 80, 20, juce::Justification::left);

    // GR meter
    g.drawText("GR:", 360, meterY, 40, 20, juce::Justification::left);
    g.setColour(juce::Colour(0xff2a2a2a));
    g.fillRect(410, meterY + 5, 100, 10);

    float grWidth = juce::jmap(gainReduction, 0.0f, -60.0f, 0.0f, 100.0f);
    g.setColour(juce::Colours::yellow);
    g.fillRect(410, meterY + 5, static_cast<int>(grWidth), 10);

    g.setColour(juce::Colours::white);
    g.drawText(juce::String(gainReduction, 1) + " dB", 520, meterY, 80, 20, juce::Justification::left);

    // Output meter
    meterY += 25;
    g.drawText("Output:", 10, meterY, 50, 20, juce::Justification::left);
    g.setColour(juce::Colour(0xff2a2a2a));
    g.fillRect(70, meterY + 5, 180, 10);

    float outputWidth = juce::jmap(outputLevel, -60.0f, 0.0f, 0.0f, 180.0f);
    g.setColour(juce::Colours::green);
    g.fillRect(70, meterY + 5, static_cast<int>(outputWidth), 10);

    g.setColour(juce::Colours::white);
    g.drawText(juce::String(outputLevel, 1) + " dB", 260, meterY, 80, 20, juce::Justification::left);
}

void GateAudioProcessorEditor::resized()
{
    // Main controls (top section)
    int sliderSize = 80;
    int sliderSpacing = 90;
    int row1Y = 70;
    int row2Y = 180;

    // Row 1: Threshold, Ratio, Range, Attack
    thresholdLabel.setBounds(20, row1Y, sliderSize, 20);
    thresholdSlider.setBounds(20, row1Y + 25, sliderSize, sliderSize);

    ratioLabel.setBounds(20 + sliderSpacing, row1Y, sliderSize, 20);
    ratioSlider.setBounds(20 + sliderSpacing, row1Y + 25, sliderSize, sliderSize);

    rangeLabel.setBounds(20 + sliderSpacing * 2, row1Y, sliderSize, 20);
    rangeSlider.setBounds(20 + sliderSpacing * 2, row1Y + 25, sliderSize, sliderSize);

    attackLabel.setBounds(20 + sliderSpacing * 3, row1Y, sliderSize, 20);
    attackSlider.setBounds(20 + sliderSpacing * 3, row1Y + 25, sliderSize, sliderSize);

    // Row 2: Hold, Release, Lookahead, RMS Window
    holdLabel.setBounds(20, row2Y, sliderSize, 20);
    holdSlider.setBounds(20, row2Y + 25, sliderSize, sliderSize);

    releaseLabel.setBounds(20 + sliderSpacing, row2Y, sliderSize, 20);
    releaseSlider.setBounds(20 + sliderSpacing, row2Y + 25, sliderSize, sliderSize);

    lookaheadLabel.setBounds(20 + sliderSpacing * 2, row2Y, sliderSize, 20);
    lookaheadSlider.setBounds(20 + sliderSpacing * 2, row2Y + 25, sliderSize, sliderSize);

    rmsWindowLabel.setBounds(20 + sliderSpacing * 3, row2Y, sliderSize, 20);
    rmsWindowSlider.setBounds(20 + sliderSpacing * 3, row2Y + 25, sliderSize, sliderSize);

    // Mode and detection controls
    modeButton.setBounds(20 + sliderSpacing * 4, row1Y + 25, 120, 30);

    detectionModeLabel.setBounds(20 + sliderSpacing * 4, row1Y + 70, 120, 20);
    detectionModeCombo.setBounds(20 + sliderSpacing * 4, row1Y + 95, 120, 25);

    // Sidechain section
    int scY = 345;
    sidechainEnableButton.setBounds(20, scY, 150, 30);

    scHighPassLabel.setBounds(180, scY - 20, 80, 20);
    scHighPassSlider.setBounds(180, scY + 5, 80, 60);

    scLowPassLabel.setBounds(280, scY - 20, 80, 20);
    scLowPassSlider.setBounds(280, scY + 5, 80, 60);

    scListenButton.setBounds(380, scY, 100, 30);
}

void GateAudioProcessorEditor::timerCallback()
{
    // Update metering values
    inputLevel = audioProcessor.getGateEngine().getInputLevelDb();
    outputLevel = audioProcessor.getGateEngine().getOutputLevelDb();
    gainReduction = audioProcessor.getGateEngine().getGainReductionDb();

    // Clamp values for display
    inputLevel = juce::jlimit(-60.0f, 0.0f, inputLevel);
    outputLevel = juce::jlimit(-60.0f, 0.0f, outputLevel);
    gainReduction = juce::jlimit(-60.0f, 0.0f, gainReduction);

    repaint();
}
