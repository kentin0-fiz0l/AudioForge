#include "PluginEditor.h"

PanUtilEditor::PanUtilEditor(PanUtilProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    // Configure Pan slider
    panSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    panSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    panSlider.setRange(-1.0, 1.0, 0.01);
    panSlider.setValue(0.0);
    panSlider.setDoubleClickReturnValue(true, 0.0);  // Double-click returns to center
    addAndMakeVisible(panSlider);

    panSlider.onValueChange = [this]
    {
        auto* panParam = dynamic_cast<juce::AudioParameterFloat*>(
            processor.getParameters()[0]);
        if (panParam != nullptr)
            *panParam = static_cast<float>(panSlider.getValue());
    };

    panLabel.setText("Pan", juce::dontSendNotification);
    panLabel.setJustificationType(juce::Justification::centred);
    panLabel.setFont(juce::FontOptions(14.0f));
    addAndMakeVisible(panLabel);

    // Configure Width slider
    widthSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    widthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    widthSlider.setRange(0.0, 2.0, 0.01);
    widthSlider.setValue(1.0);
    widthSlider.setDoubleClickReturnValue(true, 1.0);  // Double-click returns to 100%
    widthSlider.setTextValueSuffix("%");
    addAndMakeVisible(widthSlider);

    widthSlider.onValueChange = [this]
    {
        auto* widthParam = dynamic_cast<juce::AudioParameterFloat*>(
            processor.getParameters()[1]);
        if (widthParam != nullptr)
            *widthParam = static_cast<float>(widthSlider.getValue());
    };

    widthLabel.setText("Width", juce::dontSendNotification);
    widthLabel.setJustificationType(juce::Justification::centred);
    widthLabel.setFont(juce::FontOptions(14.0f));
    addAndMakeVisible(widthLabel);

    // Configure Mode selector
    modeSelector.addItem("Pan", 1);
    modeSelector.addItem("Balance", 2);
    modeSelector.setSelectedId(1);
    addAndMakeVisible(modeSelector);

    modeSelector.onChange = [this]
    {
        auto* modeParam = dynamic_cast<juce::AudioParameterChoice*>(
            processor.getParameters()[2]);
        if (modeParam != nullptr)
            *modeParam = static_cast<float>(modeSelector.getSelectedItemIndex()) /
                        (modeParam->choices.size() - 1);
    };

    modeLabel.setText("Mode", juce::dontSendNotification);
    modeLabel.setJustificationType(juce::Justification::centred);
    modeLabel.setFont(juce::FontOptions(14.0f));
    addAndMakeVisible(modeLabel);

    // Start timer for meter updates (30 fps)
    startTimerHz(30);

    setSize(500, 350);
}

PanUtilEditor::~PanUtilEditor()
{
}

void PanUtilEditor::paint(juce::Graphics& g)
{
    // Background
    g.fillAll(juce::Colour(0xff1a1a1a));

    // Title bar
    g.setColour(juce::Colour(0xff2a2a2a));
    g.fillRect(0, 0, getWidth(), 50);

    // Title text
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(24.0f, juce::Font::bold));
    g.drawText("PanUtil", 0, 10, getWidth(), 30, juce::Justification::centred);

    // Subtitle
    g.setFont(juce::FontOptions(12.0f));
    g.setColour(juce::Colour(0xff888888));
    g.drawText("AudioForge • Stereo Panning Utility", 0, 35, getWidth(), 15,
               juce::Justification::centred);

    // Draw L/R meters
    const int meterX = 50;
    const int meterY = 280;
    const int meterWidth = 200;
    const int meterHeight = 15;
    const int meterSpacing = 20;

    // Left meter
    g.setColour(juce::Colour(0xff333333));
    g.fillRect(meterX, meterY, meterWidth, meterHeight);

    if (leftMeter > 0.0f)
    {
        float fillWidth = meterWidth * leftMeter;
        g.setColour(juce::Colours::cyan);
        g.fillRect(static_cast<float>(meterX), static_cast<float>(meterY),
                   fillWidth, static_cast<float>(meterHeight));
    }

    g.setColour(juce::Colours::white);
    g.drawText("L", meterX - 30, meterY, 25, meterHeight, juce::Justification::centredRight);

    // Right meter
    g.setColour(juce::Colour(0xff333333));
    g.fillRect(meterX + meterWidth + 50, meterY, meterWidth, meterHeight);

    if (rightMeter > 0.0f)
    {
        float fillWidth = meterWidth * rightMeter;
        g.setColour(juce::Colours::magenta);
        g.fillRect(static_cast<float>(meterX + meterWidth + 50), static_cast<float>(meterY),
                   fillWidth, static_cast<float>(meterHeight));
    }

    g.setColour(juce::Colours::white);
    g.drawText("R", meterX + meterWidth + 50 - 30, meterY,
               25, meterHeight, juce::Justification::centredRight);

    // Draw stereo field visualization (circle)
    const int circleX = getWidth() / 2;
    const int circleY = 160;
    const int circleRadius = 40;

    g.setColour(juce::Colour(0xff333333));
    g.drawEllipse(circleX - circleRadius, circleY - circleRadius,
                  circleRadius * 2, circleRadius * 2, 2.0f);

    // Draw center line
    g.setColour(juce::Colour(0xff555555));
    g.drawLine(circleX, circleY - circleRadius, circleX, circleY + circleRadius, 1.0f);

    // Draw pan position indicator
    float pan = static_cast<float>(panSlider.getValue());
    int dotX = circleX + static_cast<int>(pan * circleRadius);
    int dotY = circleY;

    g.setColour(juce::Colours::yellow);
    g.fillEllipse(dotX - 5, dotY - 5, 10, 10);
}

void PanUtilEditor::resized()
{
    // Layout UI components
    panLabel.setBounds(50, 70, 100, 20);
    panSlider.setBounds(50, 90, 400, 40);

    widthLabel.setBounds(50, 140, 100, 20);
    widthSlider.setBounds(50, 160, 400, 40);

    modeLabel.setBounds(50, 210, 100, 20);
    modeSelector.setBounds(50, 230, 150, 25);
}

void PanUtilEditor::timerCallback()
{
    // Update meters from processor
    leftMeter = processor.getLeftLevel();
    rightMeter = processor.getRightLevel();

    // Repaint to update visualization
    repaint();
}
