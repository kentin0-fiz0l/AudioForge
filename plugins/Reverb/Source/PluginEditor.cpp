/**
 * Reverb Audio Processor Editor Implementation
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "../../shared/ui/AudioForgeTheme.h"

//==============================================================================
ReverbAudioProcessorEditor::ReverbAudioProcessorEditor(ReverbAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Configure room size slider
    roomSizeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    roomSizeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    roomSizeSlider.setRange(0.0, 100.0, 0.1);
    roomSizeSlider.setValue(audioProcessor.roomSizeParam->get());
    roomSizeSlider.onValueChange = [this]()
    {
        *audioProcessor.roomSizeParam = static_cast<float>(roomSizeSlider.getValue());
    };
    addAndMakeVisible(roomSizeSlider);

    roomSizeLabel.setText("Room Size", juce::dontSendNotification);
    roomSizeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(roomSizeLabel);

    // Configure damping slider
    dampingSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    dampingSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    dampingSlider.setRange(0.0, 100.0, 0.1);
    dampingSlider.setValue(audioProcessor.dampingParam->get());
    dampingSlider.onValueChange = [this]()
    {
        *audioProcessor.dampingParam = static_cast<float>(dampingSlider.getValue());
    };
    addAndMakeVisible(dampingSlider);

    dampingLabel.setText("Damping", juce::dontSendNotification);
    dampingLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(dampingLabel);

    // Configure width slider
    widthSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    widthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    widthSlider.setRange(0.0, 100.0, 0.1);
    widthSlider.setValue(audioProcessor.widthParam->get());
    widthSlider.onValueChange = [this]()
    {
        *audioProcessor.widthParam = static_cast<float>(widthSlider.getValue());
    };
    addAndMakeVisible(widthSlider);

    widthLabel.setText("Width", juce::dontSendNotification);
    widthLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(widthLabel);

    // Configure pre-delay slider
    preDelaySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    preDelaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    preDelaySlider.setRange(0.0, 100.0, 0.1);
    preDelaySlider.setValue(audioProcessor.preDelayParam->get());
    preDelaySlider.onValueChange = [this]()
    {
        *audioProcessor.preDelayParam = static_cast<float>(preDelaySlider.getValue());
    };
    addAndMakeVisible(preDelaySlider);

    preDelayLabel.setText("Pre-Delay", juce::dontSendNotification);
    preDelayLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(preDelayLabel);

    // Configure mix slider
    mixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    mixSlider.setRange(0.0, 100.0, 0.1);
    mixSlider.setValue(audioProcessor.mixParam->get());
    mixSlider.onValueChange = [this]()
    {
        *audioProcessor.mixParam = static_cast<float>(mixSlider.getValue());
    };
    addAndMakeVisible(mixSlider);

    mixLabel.setText("Mix", juce::dontSendNotification);
    mixLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(mixLabel);

    // Configure freeze button
    freezeButton.setButtonText("Freeze");
    freezeButton.setClickingTogglesState(true);
    freezeButton.setToggleState(audioProcessor.freezeParam->get(), juce::dontSendNotification);
    freezeButton.onClick = [this]()
    {
        *audioProcessor.freezeParam = freezeButton.getToggleState();
    };
    addAndMakeVisible(freezeButton);

    // Start timer for meters
    startTimerHz(30);

    setSize(700, 500);
}

ReverbAudioProcessorEditor::~ReverbAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void ReverbAudioProcessorEditor::paint(juce::Graphics& g)
{
    using namespace AudioForge;

    // Background
    g.fillAll(Colors::Background);

    // Title bar (standardized)
    Layout::drawTitleBar(g, "AudioForge Reverb", Categories::Reverb, getWidth());

    // Draw level meters
    g.setColour(Colors::Surface);
    g.fillRect(inputMeterBounds);
    g.fillRect(outputMeterBounds);

    // Input meter level
    float inputLevel = audioProcessor.inputMeter.getLevelDb();
    float inputHeight = juce::jmap(inputLevel, -60.0f, 0.0f, 0.0f, (float)inputMeterBounds.getHeight());
    g.setColour(juce::Colours::green);
    g.fillRect(inputMeterBounds.getX(),
               inputMeterBounds.getBottom() - (int)inputHeight,
               inputMeterBounds.getWidth(),
               (int)inputHeight);

    // Output meter level
    float outputLevel = audioProcessor.outputMeter.getLevelDb();
    float outputHeight = juce::jmap(outputLevel, -60.0f, 0.0f, 0.0f, (float)outputMeterBounds.getHeight());
    g.setColour(juce::Colours::orange);
    g.fillRect(outputMeterBounds.getX(),
               outputMeterBounds.getBottom() - (int)outputHeight,
               outputMeterBounds.getWidth(),
               (int)outputHeight);

    // Meter labels
    g.setColour(juce::Colours::white);
    g.setFont(12.0f);
    auto inputLabelBounds = inputMeterBounds;
    auto outputLabelBounds = outputMeterBounds;
    g.drawText("IN", inputLabelBounds.removeFromBottom(20), juce::Justification::centred);
    g.drawText("OUT", outputLabelBounds.removeFromBottom(20), juce::Justification::centred);
}

void ReverbAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(60); // Title area

    // Level meters on the right
    auto meterArea = bounds.removeFromRight(80);
    meterArea.reduce(20, 20);
    inputMeterBounds = meterArea.removeFromLeft(25).removeFromTop(meterArea.getHeight() - 20);
    meterArea.removeFromLeft(10);
    outputMeterBounds = meterArea.removeFromLeft(25).removeFromTop(meterArea.getHeight() - 20);

    // Control area
    auto controlArea = bounds.reduced(30, 30);

    // Top row: Room Size, Damping, Width
    auto topRow = controlArea.removeFromTop(160);
    topRow.removeFromBottom(10);

    int knobWidth = topRow.getWidth() / 3;

    auto roomSizeArea = topRow.removeFromLeft(knobWidth);
    roomSizeLabel.setBounds(roomSizeArea.removeFromTop(20));
    roomSizeSlider.setBounds(roomSizeArea);

    auto dampingArea = topRow.removeFromLeft(knobWidth);
    dampingLabel.setBounds(dampingArea.removeFromTop(20));
    dampingSlider.setBounds(dampingArea);

    auto widthArea = topRow.removeFromLeft(knobWidth);
    widthLabel.setBounds(widthArea.removeFromTop(20));
    widthSlider.setBounds(widthArea);

    // Bottom row: Pre-Delay, Mix, Freeze
    controlArea.removeFromTop(20);
    auto bottomRow = controlArea.removeFromTop(160);

    auto preDelayArea = bottomRow.removeFromLeft(knobWidth);
    preDelayLabel.setBounds(preDelayArea.removeFromTop(20));
    preDelaySlider.setBounds(preDelayArea);

    auto mixArea = bottomRow.removeFromLeft(knobWidth);
    mixLabel.setBounds(mixArea.removeFromTop(20));
    mixSlider.setBounds(mixArea);

    auto freezeArea = bottomRow.removeFromLeft(knobWidth);
    freezeButton.setBounds(freezeArea.withSizeKeepingCentre(120, 40));
}

void ReverbAudioProcessorEditor::timerCallback()
{
    repaint();
}
