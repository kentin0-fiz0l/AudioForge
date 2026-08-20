#include "PluginEditor.h"

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
    // Background
    g.fillAll(juce::Colour(0xff1a1a1a));

    // Title bar
    g.setColour(juce::Colour(0xff2a2a2a));
    g.fillRect(0, 0, getWidth(), 50);

    // Title text
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(24.0f, juce::Font::bold));
    g.drawText("SimpleGain", 0, 10, getWidth(), 30, juce::Justification::centred);

    // Subtitle
    g.setFont(juce::Font(12.0f));
    g.setColour(juce::Colour(0xff888888));
    g.drawText("AudioForge • Learning Plugin #1", 0, 35, getWidth(), 15,
               juce::Justification::centred);

    // Draw level meter
    const int meterX = 50;
    const int meterY = 230;
    const int meterWidth = 300;
    const int meterHeight = 20;

    // Meter background
    g.setColour(juce::Colour(0xff333333));
    g.fillRect(meterX, meterY, meterWidth, meterHeight);

    // Meter fill (green to yellow to red)
    if (meterLevel > 0.0f)
    {
        float meterWidthFilled = meterWidth * meterLevel;
        juce::Colour meterColour = juce::Colours::green;

        if (meterLevel > 0.7f)
            meterColour = juce::Colours::orange;
        if (meterLevel > 0.9f)
            meterColour = juce::Colours::red;

        g.setColour(meterColour);
        g.fillRect(static_cast<float>(meterX), static_cast<float>(meterY),
                   meterWidthFilled, static_cast<float>(meterHeight));
    }

    // Meter border
    g.setColour(juce::Colour(0xff666666));
    g.drawRect(meterX, meterY, meterWidth, meterHeight, 1);
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
