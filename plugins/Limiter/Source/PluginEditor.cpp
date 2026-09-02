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
    // Background
    g.fillAll(juce::Colour(0xff1a1a1a));

    // Title
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(24.0f, juce::Font::bold));
    g.drawText("AudioForge Limiter", 0, 10, getWidth(), 40, juce::Justification::centred);

    // Subtitle
    g.setFont(juce::Font(14.0f));
    g.setColour(juce::Colour(0xff4a9eff));
    g.drawText("True Peak Limiting", 0, 35, getWidth(), 20, juce::Justification::centred);

    // Metering section background
    g.setColour(juce::Colour(0xff2d2d2d));
    g.fillRect(20, 300, getWidth() - 40, 80);

    // Draw meters
    int meterX = 40;
    int meterY = 320;
    int meterWidth = 150;
    int meterHeight = 20;

    // Input meter
    g.setColour(juce::Colours::grey);
    g.drawText("Input:", meterX, meterY - 20, 100, 20, juce::Justification::left);
    g.fillRect(meterX, meterY, meterWidth, meterHeight);

    float inputNorm = juce::jlimit(0.0f, 1.0f, inputLevel);
    g.setColour(juce::Colour(0xff4a9eff));
    g.fillRect(meterX, meterY, static_cast<int>(meterWidth * inputNorm), meterHeight);

    // Output meter
    meterY += 40;
    g.setColour(juce::Colours::grey);
    g.drawText("Output:", meterX, meterY - 20, 100, 20, juce::Justification::left);
    g.fillRect(meterX, meterY, meterWidth, meterHeight);

    float outputNorm = juce::jlimit(0.0f, 1.0f, outputLevel);
    g.setColour(juce::Colour(0xff4a9eff));
    g.fillRect(meterX, meterY, static_cast<int>(meterWidth * outputNorm), meterHeight);

    // Gain reduction meter
    meterX = getWidth() - 220;
    meterY = 320;
    g.setColour(juce::Colours::grey);
    g.drawText("GR:", meterX, meterY - 20, 100, 20, juce::Justification::left);
    g.fillRect(meterX, meterY, meterWidth, meterHeight);

    float grNorm = juce::jlimit(0.0f, 1.0f, gainReduction / 20.0f); // 0-20 dB range
    g.setColour(juce::Colours::red);
    g.fillRect(meterX, meterY, static_cast<int>(meterWidth * grNorm), meterHeight);

    // True peak indicator
    meterY += 40;
    g.setColour(truePeakClipping ? juce::Colours::red : juce::Colours::grey);
    g.fillEllipse(static_cast<float>(meterX), static_cast<float>(meterY), 20.0f, 20.0f);
    g.setColour(juce::Colours::white);
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
