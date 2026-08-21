#include "PluginProcessor.h"
#include "PluginEditor.h"

CleanDelayEditor::CleanDelayEditor(CleanDelayProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Set editor size
    setSize(500, 350);

    // Delay Time slider
    delayTimeLabel.setText("Delay Time", juce::dontSendNotification);
    delayTimeLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa0aec0));
    delayTimeLabel.setJustificationType(juce::Justification::centred);
    delayTimeLabel.attachToComponent(&delayTimeSlider, false);
    addAndMakeVisible(delayTimeLabel);

    delayTimeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    delayTimeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    delayTimeSlider.setRange(0.001, 2.0, 0.001);
    delayTimeSlider.setValue(0.25);
    delayTimeSlider.setSkewFactorFromMidPoint(0.25);  // Log scale with 250ms midpoint
    delayTimeSlider.setTextValueSuffix(" s");
    delayTimeSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff9f7aea));
    delayTimeSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xff805ad5));
    delayTimeSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffedf2f7));
    delayTimeSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff2d3748));
    delayTimeSlider.onValueChange = [this]
    {
        auto* param = audioProcessor.getParameters()[0];
        auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
        if (floatParam != nullptr)
            floatParam->setValueNotifyingHost(floatParam->convertTo0to1(delayTimeSlider.getValue()));
    };
    addAndMakeVisible(delayTimeSlider);

    // Feedback slider
    feedbackLabel.setText("Feedback", juce::dontSendNotification);
    feedbackLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa0aec0));
    feedbackLabel.setJustificationType(juce::Justification::centred);
    feedbackLabel.attachToComponent(&feedbackSlider, false);
    addAndMakeVisible(feedbackLabel);

    feedbackSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    feedbackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    feedbackSlider.setRange(0.0, 0.99, 0.01);
    feedbackSlider.setValue(0.3);
    feedbackSlider.setTextValueSuffix(" %");
    feedbackSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xfffbd38d));
    feedbackSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xfff6ad55));
    feedbackSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffedf2f7));
    feedbackSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff2d3748));
    feedbackSlider.onValueChange = [this]
    {
        auto* param = audioProcessor.getParameters()[1];
        auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
        if (floatParam != nullptr)
            floatParam->setValueNotifyingHost(floatParam->convertTo0to1(feedbackSlider.getValue()));
    };
    addAndMakeVisible(feedbackSlider);

    // Mix slider
    mixLabel.setText("Mix (Wet)", juce::dontSendNotification);
    mixLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa0aec0));
    mixLabel.setJustificationType(juce::Justification::centred);
    mixLabel.attachToComponent(&mixSlider, false);
    addAndMakeVisible(mixLabel);

    mixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    mixSlider.setRange(0.0, 1.0, 0.01);
    mixSlider.setValue(0.5);
    mixSlider.setTextValueSuffix(" %");
    mixSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff63b3ed));
    mixSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xff4299e1));
    mixSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffedf2f7));
    mixSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff2d3748));
    mixSlider.onValueChange = [this]
    {
        auto* param = audioProcessor.getParameters()[2];
        auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
        if (floatParam != nullptr)
            floatParam->setValueNotifyingHost(floatParam->convertTo0to1(mixSlider.getValue()));
    };
    addAndMakeVisible(mixSlider);

    // Ping-Pong button
    pingPongLabel.setText("Ping-Pong", juce::dontSendNotification);
    pingPongLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa0aec0));
    pingPongLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(pingPongLabel);

    pingPongButton.setButtonText("");
    pingPongButton.onClick = [this]
    {
        auto* param = audioProcessor.getParameters()[3];
        auto* boolParam = dynamic_cast<juce::AudioParameterBool*>(param);
        if (boolParam != nullptr)
            boolParam->setValueNotifyingHost(pingPongButton.getToggleState() ? 1.0f : 0.0f);
    };
    addAndMakeVisible(pingPongButton);

    // Start timer for level meter updates (30 Hz)
    startTimer(33);
}

CleanDelayEditor::~CleanDelayEditor()
{
}

void CleanDelayEditor::paint(juce::Graphics& g)
{
    // Gradient background
    juce::ColourGradient gradient(juce::Colour(0xff2d3748), 0, 0,
                                 juce::Colour(0xff1a202c), 0, static_cast<float>(getHeight()),
                                 false);
    g.setGradientFill(gradient);
    g.fillAll();

    // Title area
    g.setColour(juce::Colour(0xff4a5568).withAlpha(0.3f));
    g.fillRoundedRectangle(10, 10, getWidth() - 20, 40, 8.0f);

    // Title text with shadow
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.setFont(juce::Font(26.0f, juce::Font::bold));
    g.drawText("CleanDelay", 21, 11, getWidth() - 40, 40, juce::Justification::centred);

    g.setColour(juce::Colour(0xff9f7aea));
    g.drawText("CleanDelay", 20, 10, getWidth() - 40, 40, juce::Justification::centred);

    // Level meters
    int meterX = 20;
    int meterY = getHeight() - 60;
    int meterW = (getWidth() - 50) / 2;
    int meterH = 30;

    // Input meter
    g.setColour(juce::Colours::black.withAlpha(0.8f));
    g.fillRoundedRectangle(meterX, meterY, meterW, meterH, 4.0f);

    if (displayInputLevel > 0.0f)
    {
        int filledWidth = static_cast<int>(meterW * displayInputLevel);
        g.setColour(juce::Colour(0xff48bb78));
        g.fillRoundedRectangle(meterX + 2, meterY + 2, filledWidth - 4, meterH - 4, 2.0f);
    }

    g.setColour(juce::Colour(0xff4a5568));
    g.drawRoundedRectangle(meterX, meterY, meterW, meterH, 4.0f, 2.0f);

    g.setColour(juce::Colour(0xffa0aec0));
    g.setFont(10.0f);
    g.drawText("INPUT", meterX, meterY - 15, 100, 15, juce::Justification::left);

    // Output meter
    meterX += meterW + 10;
    g.setColour(juce::Colours::black.withAlpha(0.8f));
    g.fillRoundedRectangle(meterX, meterY, meterW, meterH, 4.0f);

    if (displayOutputLevel > 0.0f)
    {
        int filledWidth = static_cast<int>(meterW * displayOutputLevel);
        g.setColour(juce::Colour(0xff63b3ed));
        g.fillRoundedRectangle(meterX + 2, meterY + 2, filledWidth - 4, meterH - 4, 2.0f);
    }

    g.setColour(juce::Colour(0xff4a5568));
    g.drawRoundedRectangle(meterX, meterY, meterW, meterH, 4.0f, 2.0f);

    g.setColour(juce::Colour(0xffa0aec0));
    g.drawText("OUTPUT", meterX, meterY - 15, 100, 15, juce::Justification::left);
}

void CleanDelayEditor::resized()
{
    const int controlWidth = 100;
    const int controlHeight = 100;
    const int margin = 20;

    int y = 70;

    // Controls in a row
    int x = margin + 50;
    delayTimeSlider.setBounds(x, y, controlWidth, controlHeight);
    x += controlWidth + 30;

    feedbackSlider.setBounds(x, y, controlWidth, controlHeight);
    x += controlWidth + 30;

    mixSlider.setBounds(x, y, controlWidth, controlHeight);

    y += controlHeight + 30;

    // Ping-Pong button
    pingPongLabel.setBounds(margin, y, 100, 24);
    pingPongButton.setBounds(margin + 110, y, 50, 24);
}

void CleanDelayEditor::timerCallback()
{
    // Update level meter displays
    displayInputLevel = audioProcessor.getCurrentInputLevel();
    displayOutputLevel = audioProcessor.getCurrentOutputLevel();
    repaint();
}
