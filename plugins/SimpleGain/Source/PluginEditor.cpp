#include "PluginEditor.h"
#include "../../shared/ui/AudioForgeTheme.h"

SimpleGainEditor::SimpleGainEditor(SimpleGainProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    // Configure gain slider
    gainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    gainSlider.setRange(-60.0, 12.0, 0.1);
    gainSlider.setValue(0.0);
    gainSlider.setTextValueSuffix(" dB");
    gainSlider.setNumDecimalPlacesToDisplay(1);
    addAndMakeVisible(gainSlider);

    // Connect slider to parameter
    gainSlider.onValueChange = [this]
    {
        // Direct parameter access (alternative to value tree state)
        auto* gainParam = dynamic_cast<juce::AudioParameterFloat*>(
            processor.getParameters()[0]);
        if (gainParam != nullptr)
            *gainParam = static_cast<float>(gainSlider.getValue());
    };

    // Configure gain label
    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.setJustificationType(juce::Justification::centred);
    gainLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(gainLabel);

    // Configure meter label
    meterLabel.setText("Level: -inf dB", juce::dontSendNotification);
    meterLabel.setJustificationType(juce::Justification::centred);
    meterLabel.setFont(juce::Font(14.0f));
    addAndMakeVisible(meterLabel);

    // Start timer for meter updates (30 fps)
    startTimerHz(30);

    // Set editor size
    setSize(400, 300);
}

SimpleGainEditor::~SimpleGainEditor()
{
}

void SimpleGainEditor::paint(juce::Graphics& g)
{
    using namespace AudioForge;

    // Background
    g.fillAll(Colors::Background);

    // Title bar (standardized)
    Layout::drawTitleBar(g, "AudioForge SimpleGain", Categories::Utility, getWidth());

    // Draw level meter using theme helper
    const int meterX = 50;
    const int meterY = 230;

    Layout::drawMeter(g, meterX, meterY, 300, Dimensions::MeterHeight, meterLevel);

    // Meter border
    g.setColour(Colors::Border);
    g.drawRect(meterX, meterY, 300, Dimensions::MeterHeight, 1);
}

void SimpleGainEditor::resized()
{
    // Layout UI components
    gainLabel.setBounds(0, 70, getWidth(), 30);
    gainSlider.setBounds(100, 100, 200, 100);
    meterLabel.setBounds(0, 260, getWidth(), 20);
}

void SimpleGainEditor::timerCallback()
{
    // Update meter from processor
    float level = processor.getCurrentLevel();
    meterLevel = level;

    // Update meter label
    if (level > 0.0001f)
    {
        float levelDb = juce::Decibels::gainToDecibels(level);
        meterLabel.setText("Level: " + juce::String(levelDb, 1) + " dB",
                          juce::dontSendNotification);
    }
    else
    {
        meterLabel.setText("Level: -inf dB", juce::dontSendNotification);
    }

    // Repaint to update meter visualization
    repaint();
}
