#include "PluginProcessor.h"
#include "PluginEditor.h"

WaveShaperAudioProcessorEditor::WaveShaperAudioProcessorEditor (WaveShaperAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (500, 400);

    // Drive slider
    driveSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    driveSlider.setRange (0.0, 100.0, 0.1);
    driveSlider.setValue (audioProcessor.driveParam->get());
    addAndMakeVisible (driveSlider);

    driveLabel.setText ("Drive", juce::dontSendNotification);
    driveLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (driveLabel);

    // Mix slider
    mixSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    mixSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    mixSlider.setRange (0.0, 100.0, 0.1);
    mixSlider.setValue (audioProcessor.mixParam->get());
    addAndMakeVisible (mixSlider);

    mixLabel.setText ("Mix", juce::dontSendNotification);
    mixLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (mixLabel);

    // Tone slider
    toneSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    toneSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    toneSlider.setRange (0.0, 100.0, 0.1);
    toneSlider.setValue (audioProcessor.toneParam->get());
    addAndMakeVisible (toneSlider);

    toneLabel.setText ("Tone", juce::dontSendNotification);
    toneLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (toneLabel);

    // Shape selector
    shapeSelector.addItem ("Soft Clip", 1);
    shapeSelector.addItem ("Hard Clip", 2);
    shapeSelector.addItem ("Tanh", 3);
    shapeSelector.addItem ("Cubic", 4);
    shapeSelector.addItem ("Asymmetric", 5);
    shapeSelector.setSelectedId (audioProcessor.shapeParam->getIndex() + 1);
    addAndMakeVisible (shapeSelector);

    shapeLabel.setText ("Shape", juce::dontSendNotification);
    shapeLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (shapeLabel);

    // Output gain slider
    outputGainSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    outputGainSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    outputGainSlider.setRange (-12.0, 12.0, 0.1);
    outputGainSlider.setValue (audioProcessor.outputGainParam->get());
    addAndMakeVisible (outputGainSlider);

    outputGainLabel.setText ("Output", juce::dontSendNotification);
    outputGainLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (outputGainLabel);

    // Metering labels
    inputMeterLabel.setText ("Input: 0.0 dB", juce::dontSendNotification);
    addAndMakeVisible (inputMeterLabel);

    outputMeterLabel.setText ("Output: 0.0 dB", juce::dontSendNotification);
    addAndMakeVisible (outputMeterLabel);

    // Start timer for meter updates
    startTimerHz (30);
}

WaveShaperAudioProcessorEditor::~WaveShaperAudioProcessorEditor()
{
}

void WaveShaperAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff1a1a1a));

    g.setColour (juce::Colours::white);
    g.setFont (juce::Font(24.0f, juce::Font::bold));
    g.drawText ("WaveShaper", getLocalBounds().removeFromTop(50),
                juce::Justification::centred, true);

    g.setFont (juce::Font(12.0f));
    g.drawText ("Waveshaping Distortion", getLocalBounds().removeFromTop(70).removeFromBottom(20),
                juce::Justification::centred, true);
}

void WaveShaperAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop (80);  // Title space

    // Top row: Drive, Mix, Tone
    auto topRow = bounds.removeFromTop (120);
    const int knobWidth = topRow.getWidth() / 3;

    auto driveArea = topRow.removeFromLeft (knobWidth);
    driveLabel.setBounds (driveArea.removeFromTop (20));
    driveSlider.setBounds (driveArea.reduced (10));

    auto mixArea = topRow.removeFromLeft (knobWidth);
    mixLabel.setBounds (mixArea.removeFromTop (20));
    mixSlider.setBounds (mixArea.reduced (10));

    auto toneArea = topRow.removeFromLeft (knobWidth);
    toneLabel.setBounds (toneArea.removeFromTop (20));
    toneSlider.setBounds (toneArea.reduced (10));

    bounds.removeFromTop (20);  // Spacing

    // Middle row: Shape selector and Output gain
    auto middleRow = bounds.removeFromTop (120);

    auto shapeArea = middleRow.removeFromLeft (knobWidth * 2);
    shapeLabel.setBounds (shapeArea.removeFromTop (20));
    shapeSelector.setBounds (shapeArea.reduced (10).removeFromTop (30));

    auto outputArea = middleRow;
    outputGainLabel.setBounds (outputArea.removeFromTop (20));
    outputGainSlider.setBounds (outputArea.reduced (10));

    bounds.removeFromTop (20);  // Spacing

    // Bottom row: Metering
    auto meterArea = bounds.removeFromTop (40);
    inputMeterLabel.setBounds (meterArea.removeFromLeft (getWidth() / 2).reduced (10));
    outputMeterLabel.setBounds (meterArea.reduced (10));
}

void WaveShaperAudioProcessorEditor::timerCallback()
{
    // Update meters
    const float inputLevel = audioProcessor.inputMeter.getLevelDb();
    const float outputLevel = audioProcessor.outputMeter.getLevelDb();

    inputMeterLabel.setText (
        "Input: " + juce::String (inputLevel, 1) + " dB",
        juce::dontSendNotification);

    outputMeterLabel.setText (
        "Output: " + juce::String (outputLevel, 1) + " dB",
        juce::dontSendNotification);
}
