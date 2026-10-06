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
    // The parameter runs from 0 to 2; show it as 0% to 200%
    widthSlider.textFromValueFunction = [] (double value) { return juce::String(juce::roundToInt(value * 100.0)) + "%"; };
    widthSlider.valueFromTextFunction = [this] (const juce::String& text)
    {
        // Text with no number in it changes nothing; zero would be mono
        const auto digits = text.retainCharacters("0123456789.");
        return digits.containsAnyOf("0123456789") ? digits.getDoubleValue() / 100.0 : widthSlider.getValue();
    };
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

    // Configure Gain slider
    gainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 70, 20);
    gainSlider.setRange(-36.0, 24.0, 0.1);
    gainSlider.setDoubleClickReturnValue(true, 0.0);  // Double-click returns to 0 dB
    gainSlider.setTextValueSuffix(" dB");
    addAndMakeVisible(gainSlider);

    gainSlider.onValueChange = [this]
    {
        auto* gainParam = dynamic_cast<juce::AudioParameterFloat*>(
            processor.getParameters()[3]);
        if (gainParam != nullptr)
            *gainParam = static_cast<float>(gainSlider.getValue());
    };

    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.setJustificationType(juce::Justification::centred);
    gainLabel.setFont(juce::FontOptions(14.0f));
    addAndMakeVisible(gainLabel);

    // Configure Mono switch
    monoButton.setButtonText("Mono");
    addAndMakeVisible(monoButton);

    monoButton.onClick = [this]
    {
        auto* monoParam = dynamic_cast<juce::AudioParameterBool*>(
            processor.getParameters()[4]);
        if (monoParam != nullptr)
            *monoParam = monoButton.getToggleState();
    };

    // The controls above were given the defaults; show what the processor
    // has, which differs when a saved set is opened
    syncFromParameters();

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
    g.drawText("AudioForge Track Utility", 0, 35, getWidth(), 15,
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
    const auto field = getStereoFieldBounds();
    const int circleX = field.getCentreX();
    const int circleY = field.getCentreY();
    const int circleRadius = field.getWidth() / 2;

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

juce::Rectangle<int> PanUtilEditor::getStereoFieldBounds() const
{
    // To the right of the Pan and Width sliders, level with the gap between them
    return { 365, 105, 80, 80 };
}

void PanUtilEditor::resized()
{
    // Layout UI components
    panLabel.setBounds(50, 70, 100, 20);
    panSlider.setBounds(50, 90, 280, 40);

    widthLabel.setBounds(50, 140, 100, 20);
    widthSlider.setBounds(50, 160, 280, 40);

    modeLabel.setBounds(50, 210, 100, 20);
    modeSelector.setBounds(50, 230, 150, 25);

    gainLabel.setBounds(230, 210, 60, 20);
    gainSlider.setBounds(230, 228, 220, 28);

    monoButton.setBounds(230, 256, 100, 22);
}

void PanUtilEditor::syncFromParameters()
{
    const auto& params = processor.getParameters();

    // The value a parameter's control shows
    const auto shown = [&params] (int index)
    {
        auto* param = dynamic_cast<juce::RangedAudioParameter*>(params[index]);
        return param != nullptr ? param->convertFrom0to1(param->getValue()) : 0.0f;
    };

    // A control being dragged is ahead of its parameter, so leave it alone
    const auto follow = [&shown] (juce::Slider& slider, int index)
    {
        if (! slider.isMouseButtonDown())
            slider.setValue(shown(index), juce::dontSendNotification);
    };

    follow(panSlider, 0);
    follow(widthSlider, 1);
    modeSelector.setSelectedItemIndex(juce::roundToInt(shown(2)), juce::dontSendNotification);
    follow(gainSlider, 3);
    monoButton.setToggleState(shown(4) > 0.5f, juce::dontSendNotification);
}

void PanUtilEditor::timerCallback()
{
    // Update meters from processor
    leftMeter = processor.getLeftLevel();
    rightMeter = processor.getRightLevel();

    // Follow changes made by the host
    syncFromParameters();

    // Repaint to update visualization
    repaint();
}
