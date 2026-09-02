#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "../../shared/ui/AudioForgeTheme.h"

SaturationAudioProcessorEditor::SaturationAudioProcessorEditor (SaturationAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), harmonicAnalyzer(p)
{
    setSize (700, 480);

    // Drive slider
    driveSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    driveSlider.setRange (0.0, 100.0, 0.1);
    driveSlider.setValue (audioProcessor.driveParam->get());
    driveSlider.onValueChange = [this]()
    {
        *audioProcessor.driveParam = static_cast<float>(driveSlider.getValue());
    };
    addAndMakeVisible (driveSlider);

    driveLabel.setText ("Drive", juce::dontSendNotification);
    driveLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (driveLabel);

    // Mix slider
    mixSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    mixSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    mixSlider.setRange (0.0, 100.0, 0.1);
    mixSlider.setValue (audioProcessor.mixParam->get());
    mixSlider.onValueChange = [this]()
    {
        *audioProcessor.mixParam = static_cast<float>(mixSlider.getValue());
    };
    addAndMakeVisible (mixSlider);

    mixLabel.setText ("Mix", juce::dontSendNotification);
    mixLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (mixLabel);

    // Tone Low slider
    toneLowSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    toneLowSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    toneLowSlider.setRange (-12.0, 12.0, 0.1);
    toneLowSlider.setValue (audioProcessor.toneLowParam->get());
    toneLowSlider.onValueChange = [this]()
    {
        *audioProcessor.toneLowParam = static_cast<float>(toneLowSlider.getValue());
    };
    addAndMakeVisible (toneLowSlider);

    toneLowLabel.setText ("Tone Low", juce::dontSendNotification);
    toneLowLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (toneLowLabel);

    // Tone High slider
    toneHighSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    toneHighSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    toneHighSlider.setRange (-12.0, 12.0, 0.1);
    toneHighSlider.setValue (audioProcessor.toneHighParam->get());
    toneHighSlider.onValueChange = [this]()
    {
        *audioProcessor.toneHighParam = static_cast<float>(toneHighSlider.getValue());
    };
    addAndMakeVisible (toneHighSlider);

    toneHighLabel.setText ("Tone High", juce::dontSendNotification);
    toneHighLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (toneHighLabel);

    // Mode selector
    modeSelector.addItem ("Tape", 1);
    modeSelector.addItem ("Tube", 2);
    modeSelector.addItem ("Transistor", 3);
    modeSelector.addItem ("Transformer", 4);
    modeSelector.addItem ("Diode", 5);
    modeSelector.setSelectedId (audioProcessor.modeParam->getIndex() + 1);
    modeSelector.onChange = [this]()
    {
        *audioProcessor.modeParam = modeSelector.getSelectedId() - 1;
    };
    addAndMakeVisible (modeSelector);

    modeLabel.setText ("Mode", juce::dontSendNotification);
    modeLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (modeLabel);

    // Oversampling selector
    oversamplingSelector.addItem ("Off", 1);
    oversamplingSelector.addItem ("2x", 2);
    oversamplingSelector.addItem ("4x", 3);
    oversamplingSelector.setSelectedId (audioProcessor.oversamplingParam->getIndex() + 1);
    oversamplingSelector.onChange = [this]()
    {
        *audioProcessor.oversamplingParam = oversamplingSelector.getSelectedId() - 1;
    };
    addAndMakeVisible (oversamplingSelector);

    oversamplingLabel.setText ("Oversampling", juce::dontSendNotification);
    oversamplingLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (oversamplingLabel);

    // Metering labels
    inputMeterLabel.setText ("Input: 0.0 dB", juce::dontSendNotification);
    inputMeterLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (inputMeterLabel);

    outputMeterLabel.setText ("Output: 0.0 dB", juce::dontSendNotification);
    outputMeterLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (outputMeterLabel);

    // Harmonic analyzer
    addAndMakeVisible (harmonicAnalyzer);

    // Start timer for meter updates
    startTimerHz (30);
}

SaturationAudioProcessorEditor::~SaturationAudioProcessorEditor()
{
}

void SaturationAudioProcessorEditor::paint (juce::Graphics& g)
{
    using namespace AudioForge;

    // Background
    g.fillAll(Colors::Background);

    // Title bar (standardized)
    Layout::drawTitleBar(g, "AudioForge Saturation", Categories::Distortion, getWidth());
}

void SaturationAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop (80);  // Title space

    // Harmonic analyzer at top
    auto analyzerArea = bounds.removeFromTop (180);
    harmonicAnalyzer.setBounds (analyzerArea.reduced (10));

    bounds.removeFromTop (10);  // Spacing

    // Top row: Mode and Oversampling selectors
    auto selectorRow = bounds.removeFromTop (50);

    auto modeArea = selectorRow.removeFromLeft (getWidth() / 2);
    modeLabel.setBounds (modeArea.removeFromTop (20).reduced (10, 0));
    modeSelector.setBounds (modeArea.reduced (10));

    auto oversamplingArea = selectorRow;
    oversamplingLabel.setBounds (oversamplingArea.removeFromTop (20).reduced (10, 0));
    oversamplingSelector.setBounds (oversamplingArea.reduced (10));

    bounds.removeFromTop (10);  // Spacing

    // Middle row: Drive, Mix, Tone Low, Tone High knobs
    auto knobRow = bounds.removeFromTop (120);
    const int knobWidth = knobRow.getWidth() / 4;

    auto driveArea = knobRow.removeFromLeft (knobWidth);
    driveLabel.setBounds (driveArea.removeFromTop (20));
    driveSlider.setBounds (driveArea.reduced (10));

    auto mixArea = knobRow.removeFromLeft (knobWidth);
    mixLabel.setBounds (mixArea.removeFromTop (20));
    mixSlider.setBounds (mixArea.reduced (10));

    auto toneLowArea = knobRow.removeFromLeft (knobWidth);
    toneLowLabel.setBounds (toneLowArea.removeFromTop (20));
    toneLowSlider.setBounds (toneLowArea.reduced (10));

    auto toneHighArea = knobRow;
    toneHighLabel.setBounds (toneHighArea.removeFromTop (20));
    toneHighSlider.setBounds (toneHighArea.reduced (10));

    bounds.removeFromTop (10);  // Spacing

    // Bottom row: Metering
    auto meterArea = bounds.removeFromTop (30);
    inputMeterLabel.setBounds (meterArea.removeFromLeft (getWidth() / 2).reduced (10));
    outputMeterLabel.setBounds (meterArea.reduced (10));
}

void SaturationAudioProcessorEditor::timerCallback()
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

    // Sync sliders with parameters (for automation)
    driveSlider.setValue (audioProcessor.driveParam->get(), juce::dontSendNotification);
    mixSlider.setValue (audioProcessor.mixParam->get(), juce::dontSendNotification);
    toneLowSlider.setValue (audioProcessor.toneLowParam->get(), juce::dontSendNotification);
    toneHighSlider.setValue (audioProcessor.toneHighParam->get(), juce::dontSendNotification);
    modeSelector.setSelectedId (audioProcessor.modeParam->getIndex() + 1, juce::dontSendNotification);
    oversamplingSelector.setSelectedId (audioProcessor.oversamplingParam->getIndex() + 1, juce::dontSendNotification);
}
