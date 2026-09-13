/*
  ==============================================================================

    PluginEditor.cpp
    Created: 1 Sep 2026
    Author:  AudioForge Team

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "../../shared/ui/AudioForgeTheme.h"

//==============================================================================
LimiterAudioProcessorEditor::LimiterAudioProcessorEditor (LimiterAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Set size
    setSize (600, 400);

    // Configure sliders
    auto setupSlider = [this](juce::Slider& slider, const juce::String& suffix)
    {
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
        slider.setTextValueSuffix(suffix);
        addAndMakeVisible(slider);
    };

    setupSlider(ceilingSlider, " dB");
    setupSlider(thresholdSlider, " dB");
    setupSlider(releaseSlider, " ms");
    setupSlider(lookaheadSlider, " ms");
    setupSlider(outputTrimSlider, " dB");

    // Configure labels
    auto setupLabel = [this](juce::Label& label, const juce::String& text, juce::Slider& slider)
    {
        label.setText(text, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.attachToComponent(&slider, false);
        addAndMakeVisible(label);
    };

    setupLabel(ceilingLabel, "Ceiling", ceilingSlider);
    setupLabel(thresholdLabel, "Threshold", thresholdSlider);
    setupLabel(releaseLabel, "Release", releaseSlider);
    setupLabel(lookaheadLabel, "Lookahead", lookaheadSlider);
    setupLabel(outputTrimLabel, "Output", outputTrimSlider);

    // Configure auto makeup button
    autoMakeupButton.setButtonText("Auto Makeup");
    addAndMakeVisible(autoMakeupButton);

    // Attach parameters to controls
    auto& params = audioProcessor.getParameters();

    ceilingAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(params, "ceiling", ceilingSlider));
    thresholdAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(params, "threshold", thresholdSlider));
    releaseAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(params, "release", releaseSlider));
    lookaheadAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(params, "lookahead", lookaheadSlider));
    outputTrimAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(params, "outputTrim", outputTrimSlider));
    autoMakeupAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(params, "autoMakeup", autoMakeupButton));

    // Start timer for metering (30 Hz refresh)
    startTimerHz(30);
}

LimiterAudioProcessorEditor::~LimiterAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void LimiterAudioProcessorEditor::paint (juce::Graphics& g)
{
    using namespace AudioForge;

    // Background
    g.fillAll(Colors::Background);

    // Title bar (standardized)
    Layout::drawTitleBar(g, "AudioForge Limiter", Categories::Mastering, getWidth());

    // Metering section background
    g.setColour(Colors::SurfaceVariant);
    g.fillRect(20, 300, getWidth() - 40, 80);

    // Draw meters (using theme meter helper)
    int meterX = 40;
    int meterY = 320;

    // Input meter
    g.setColour(Colors::TextSecondary);
    g.setFont(Typography::Body);
    g.drawText("Input:", meterX, meterY - 20, 100, 20, juce::Justification::left);
    Layout::drawMeter(g, meterX, meterY, Dimensions::MeterWidth, Dimensions::MeterHeight, inputLevel);

    // Output meter
    meterY += 40;
    g.setColour(Colors::TextSecondary);
    g.drawText("Output:", meterX, meterY - 20, 100, 20, juce::Justification::left);
    Layout::drawMeter(g, meterX, meterY, Dimensions::MeterWidth, Dimensions::MeterHeight, outputLevel);

    // Gain reduction meter
    meterX = getWidth() - 220;
    meterY = 320;
    g.setColour(Colors::TextSecondary);
    g.drawText("GR:", meterX, meterY - 20, 100, 20, juce::Justification::left);

    // GR meter (inverted - more GR = more fill)
    float grNorm = juce::jlimit(0.0f, 1.0f, gainReduction / 20.0f);
    g.setColour(Colors::MeterBackground);
    g.fillRect(meterX, meterY, Dimensions::MeterWidth, Dimensions::MeterHeight);
    g.setColour(Colors::MeterMid); // Yellow for gain reduction
    g.fillRect(meterX, meterY, static_cast<int>(Dimensions::MeterWidth * grNorm), Dimensions::MeterHeight);

    // True peak indicator
    meterY += 40;
    g.setColour(truePeakClipping ? Colors::MeterHigh : Colors::ControlFill);
    g.fillEllipse(static_cast<float>(meterX), static_cast<float>(meterY), 20.0f, 20.0f);
    g.setColour(Colors::Text);
    g.drawText("TP", meterX + 25, meterY, 100, 20, juce::Justification::left);
}

void LimiterAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);
    area.removeFromTop(60); // Title space

    // Parameter controls in two rows
    auto controlsArea = area.removeFromTop(200);

    int sliderWidth = (controlsArea.getWidth() - 20) / 3;
    int sliderHeight = 100;

    // First row
    auto row1 = controlsArea.removeFromTop(sliderHeight);
    ceilingSlider.setBounds(row1.removeFromLeft(sliderWidth).reduced(5));
    thresholdSlider.setBounds(row1.removeFromLeft(sliderWidth).reduced(5));
    releaseSlider.setBounds(row1.removeFromLeft(sliderWidth).reduced(5));

    // Second row
    auto row2 = controlsArea.removeFromTop(sliderHeight);
    lookaheadSlider.setBounds(row2.removeFromLeft(sliderWidth).reduced(5));
    outputTrimSlider.setBounds(row2.removeFromLeft(sliderWidth).reduced(5));
    autoMakeupButton.setBounds(row2.removeFromLeft(sliderWidth).reduced(30, 40));
}

void LimiterAudioProcessorEditor::timerCallback()
{
    // Update metering values from limiter engine
    auto& engine = audioProcessor.getLimiterEngine();

    inputLevel = juce::Decibels::decibelsToGain(engine.getInputLevelDb() + 12.0f) / 4.0f; // Normalize to 0-1
    outputLevel = juce::Decibels::decibelsToGain(engine.getOutputLevelDb() + 12.0f) / 4.0f;
    gainReduction = std::abs(engine.getGainReductionDb());
    truePeakClipping = engine.isTruePeakClipping();

    // Repaint metering section
    repaint();
}
