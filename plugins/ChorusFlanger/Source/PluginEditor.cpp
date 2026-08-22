#include "PluginProcessor.h"
#include "PluginEditor.h"

ChorusFlangerAudioProcessorEditor::ChorusFlangerAudioProcessorEditor (ChorusFlangerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), lfoVisualizer(p)
{
    setSize (700, 500);

    // Rate slider
    rateSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    rateSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    rateSlider.setRange (0.1, 10.0, 0.01);
    rateSlider.setSkewFactorFromMidPoint(1.0); // Log scale centered at 1 Hz
    rateSlider.setValue (audioProcessor.rateParam->get());
    rateSlider.onValueChange = [this]()
    {
        *audioProcessor.rateParam = static_cast<float>(rateSlider.getValue());
    };
    addAndMakeVisible (rateSlider);

    rateLabel.setText ("Rate (Hz)", juce::dontSendNotification);
    rateLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (rateLabel);

    // Depth slider
    depthSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    depthSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    depthSlider.setRange (0.0, 100.0, 0.1);
    depthSlider.setValue (audioProcessor.depthParam->get());
    depthSlider.onValueChange = [this]()
    {
        *audioProcessor.depthParam = static_cast<float>(depthSlider.getValue());
    };
    addAndMakeVisible (depthSlider);

    depthLabel.setText ("Depth", juce::dontSendNotification);
    depthLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (depthLabel);

    // Feedback slider
    feedbackSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    feedbackSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    feedbackSlider.setRange (0.0, 95.0, 0.1);
    feedbackSlider.setValue (audioProcessor.feedbackParam->get());
    feedbackSlider.onValueChange = [this]()
    {
        *audioProcessor.feedbackParam = static_cast<float>(feedbackSlider.getValue());
    };
    addAndMakeVisible (feedbackSlider);

    feedbackLabel.setText ("Feedback", juce::dontSendNotification);
    feedbackLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (feedbackLabel);

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

    // Stereo Width slider
    stereoWidthSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    stereoWidthSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    stereoWidthSlider.setRange (0.0, 100.0, 0.1);
    stereoWidthSlider.setValue (audioProcessor.stereoWidthParam->get());
    stereoWidthSlider.onValueChange = [this]()
    {
        *audioProcessor.stereoWidthParam = static_cast<float>(stereoWidthSlider.getValue());
    };
    addAndMakeVisible (stereoWidthSlider);

    stereoWidthLabel.setText ("Stereo Width", juce::dontSendNotification);
    stereoWidthLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (stereoWidthLabel);

    // Mode selector
    modeSelector.addItem ("Chorus", 1);
    modeSelector.addItem ("Flanger", 2);
    modeSelector.setSelectedId (audioProcessor.modeParam->getIndex() + 1);
    modeSelector.onChange = [this]()
    {
        *audioProcessor.modeParam = modeSelector.getSelectedId() - 1;
    };
    addAndMakeVisible (modeSelector);

    modeLabel.setText ("Mode", juce::dontSendNotification);
    modeLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (modeLabel);

    // Waveform selector
    waveformSelector.addItem ("Sine", 1);
    waveformSelector.addItem ("Triangle", 2);
    waveformSelector.setSelectedId (audioProcessor.waveformParam->getIndex() + 1);
    waveformSelector.onChange = [this]()
    {
        *audioProcessor.waveformParam = waveformSelector.getSelectedId() - 1;
    };
    addAndMakeVisible (waveformSelector);

    waveformLabel.setText ("Waveform", juce::dontSendNotification);
    waveformLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (waveformLabel);

    // Metering labels
    inputMeterLabel.setText ("Input: 0.0 dB", juce::dontSendNotification);
    inputMeterLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (inputMeterLabel);

    outputMeterLabel.setText ("Output: 0.0 dB", juce::dontSendNotification);
    outputMeterLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (outputMeterLabel);

    // LFO Visualizer
    addAndMakeVisible (lfoVisualizer);

    // Start timer for meter updates
    startTimerHz (30);
}

ChorusFlangerAudioProcessorEditor::~ChorusFlangerAudioProcessorEditor()
{
}

void ChorusFlangerAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff1a1a1a));

    g.setColour (juce::Colours::white);
    g.setFont (juce::Font(24.0f, juce::Font::bold));
    g.drawText ("Chorus/Flanger", getLocalBounds().removeFromTop(50),
                juce::Justification::centred, true);

    g.setFont (juce::Font(12.0f));
    g.drawText ("LFO-Based Modulation Effects",
                getLocalBounds().removeFromTop(70).removeFromBottom(20),
                juce::Justification::centred, true);
}

void ChorusFlangerAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop (80);  // Title space

    // LFO visualizer at top
    auto visualizerArea = bounds.removeFromTop (120);
    lfoVisualizer.setBounds (visualizerArea.reduced (10));

    bounds.removeFromTop (10);  // Spacing

    // Top row: Mode and Waveform selectors
    auto selectorRow = bounds.removeFromTop (50);

    auto modeArea = selectorRow.removeFromLeft (getWidth() / 2);
    modeLabel.setBounds (modeArea.removeFromTop (20).reduced (10, 0));
    modeSelector.setBounds (modeArea.reduced (10));

    auto waveformArea = selectorRow;
    waveformLabel.setBounds (waveformArea.removeFromTop (20).reduced (10, 0));
    waveformSelector.setBounds (waveformArea.reduced (10));

    bounds.removeFromTop (10);  // Spacing

    // Middle row: Rate, Depth, Feedback knobs
    auto knobRow1 = bounds.removeFromTop (120);
    const int knobWidth = knobRow1.getWidth() / 3;

    auto rateArea = knobRow1.removeFromLeft (knobWidth);
    rateLabel.setBounds (rateArea.removeFromTop (20));
    rateSlider.setBounds (rateArea.reduced (10));

    auto depthArea = knobRow1.removeFromLeft (knobWidth);
    depthLabel.setBounds (depthArea.removeFromTop (20));
    depthSlider.setBounds (depthArea.reduced (10));

    auto feedbackArea = knobRow1;
    feedbackLabel.setBounds (feedbackArea.removeFromTop (20));
    feedbackSlider.setBounds (feedbackArea.reduced (10));

    bounds.removeFromTop (10);  // Spacing

    // Bottom row: Mix, Stereo Width knobs
    auto knobRow2 = bounds.removeFromTop (120);

    auto mixArea = knobRow2.removeFromLeft (getWidth() / 2);
    mixLabel.setBounds (mixArea.removeFromTop (20));
    mixSlider.setBounds (mixArea.reduced (10));

    auto stereoWidthArea = knobRow2;
    stereoWidthLabel.setBounds (stereoWidthArea.removeFromTop (20));
    stereoWidthSlider.setBounds (stereoWidthArea.reduced (10));

    bounds.removeFromTop (10);  // Spacing

    // Metering row
    auto meterArea = bounds.removeFromTop (30);
    inputMeterLabel.setBounds (meterArea.removeFromLeft (getWidth() / 2).reduced (10));
    outputMeterLabel.setBounds (meterArea.reduced (10));
}

void ChorusFlangerAudioProcessorEditor::timerCallback()
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
    rateSlider.setValue (audioProcessor.rateParam->get(), juce::dontSendNotification);
    depthSlider.setValue (audioProcessor.depthParam->get(), juce::dontSendNotification);
    feedbackSlider.setValue (audioProcessor.feedbackParam->get(), juce::dontSendNotification);
    mixSlider.setValue (audioProcessor.mixParam->get(), juce::dontSendNotification);
    stereoWidthSlider.setValue (audioProcessor.stereoWidthParam->get(), juce::dontSendNotification);
    modeSelector.setSelectedId (audioProcessor.modeParam->getIndex() + 1, juce::dontSendNotification);
    waveformSelector.setSelectedId (audioProcessor.waveformParam->getIndex() + 1, juce::dontSendNotification);
}
