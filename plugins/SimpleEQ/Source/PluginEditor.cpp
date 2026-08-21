#include "PluginProcessor.h"
#include "PluginEditor.h"

SimpleEQEditor::SimpleEQEditor(SimpleEQProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Set editor size
    setSize(600, 450);

    // Low band controls (red color scheme)
    lowFreqLabel.setText("Low Freq", juce::dontSendNotification);
    lowFreqLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa0aec0));
    lowFreqLabel.setJustificationType(juce::Justification::centred);
    lowFreqLabel.attachToComponent(&lowFreqSlider, false);
    addAndMakeVisible(lowFreqLabel);

    lowFreqSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    lowFreqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    lowFreqSlider.setRange(20.0, 500.0, 1.0);
    lowFreqSlider.setValue(80.0);
    lowFreqSlider.setSkewFactorFromMidPoint(100.0);
    lowFreqSlider.setTextValueSuffix(" Hz");
    lowFreqSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xfffc8181));
    lowFreqSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xfff56565));
    addAndMakeVisible(lowFreqSlider);

    lowGainLabel.setText("Low Gain", juce::dontSendNotification);
    lowGainLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa0aec0));
    lowGainLabel.setJustificationType(juce::Justification::centred);
    lowGainLabel.attachToComponent(&lowGainSlider, false);
    addAndMakeVisible(lowGainLabel);

    lowGainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    lowGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    lowGainSlider.setRange(-12.0, 12.0, 0.1);
    lowGainSlider.setValue(0.0);
    lowGainSlider.setTextValueSuffix(" dB");
    lowGainSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xfffc8181));
    lowGainSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xfff56565));
    addAndMakeVisible(lowGainSlider);

    // Mid band controls (green color scheme)
    midFreqLabel.setText("Mid Freq", juce::dontSendNotification);
    midFreqLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa0aec0));
    midFreqLabel.setJustificationType(juce::Justification::centred);
    midFreqLabel.attachToComponent(&midFreqSlider, false);
    addAndMakeVisible(midFreqLabel);

    midFreqSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    midFreqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    midFreqSlider.setRange(200.0, 5000.0, 1.0);
    midFreqSlider.setValue(1000.0);
    midFreqSlider.setSkewFactorFromMidPoint(1000.0);
    midFreqSlider.setTextValueSuffix(" Hz");
    midFreqSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff68d391));
    midFreqSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xff48bb78));
    addAndMakeVisible(midFreqSlider);

    midGainLabel.setText("Mid Gain", juce::dontSendNotification);
    midGainLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa0aec0));
    midGainLabel.setJustificationType(juce::Justification::centred);
    midGainLabel.attachToComponent(&midGainSlider, false);
    addAndMakeVisible(midGainLabel);

    midGainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    midGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    midGainSlider.setRange(-12.0, 12.0, 0.1);
    midGainSlider.setValue(0.0);
    midGainSlider.setTextValueSuffix(" dB");
    midGainSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff68d391));
    midGainSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xff48bb78));
    addAndMakeVisible(midGainSlider);

    // High band controls (blue color scheme)
    highFreqLabel.setText("High Freq", juce::dontSendNotification);
    highFreqLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa0aec0));
    highFreqLabel.setJustificationType(juce::Justification::centred);
    highFreqLabel.attachToComponent(&highFreqSlider, false);
    addAndMakeVisible(highFreqLabel);

    highFreqSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    highFreqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    highFreqSlider.setRange(2000.0, 20000.0, 1.0);
    highFreqSlider.setValue(8000.0);
    highFreqSlider.setSkewFactorFromMidPoint(8000.0);
    highFreqSlider.setTextValueSuffix(" Hz");
    highFreqSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff63b3ed));
    highFreqSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xff4299e1));
    addAndMakeVisible(highFreqSlider);

    highGainLabel.setText("High Gain", juce::dontSendNotification);
    highGainLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa0aec0));
    highGainLabel.setJustificationType(juce::Justification::centred);
    highGainLabel.attachToComponent(&highGainSlider, false);
    addAndMakeVisible(highGainLabel);

    highGainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    highGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    highGainSlider.setRange(-12.0, 12.0, 0.1);
    highGainSlider.setValue(0.0);
    highGainSlider.setTextValueSuffix(" dB");
    highGainSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff63b3ed));
    highGainSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xff4299e1));
    addAndMakeVisible(highGainSlider);

    // HPF controls (yellow color scheme)
    hpfLabel.setText("HPF", juce::dontSendNotification);
    hpfLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa0aec0));
    hpfLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(hpfLabel);

    hpfEnabledButton.setButtonText("");
    addAndMakeVisible(hpfEnabledButton);

    hpfFreqSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    hpfFreqSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 70, 20);
    hpfFreqSlider.setRange(20.0, 500.0, 1.0);
    hpfFreqSlider.setValue(40.0);
    hpfFreqSlider.setSkewFactorFromMidPoint(100.0);
    hpfFreqSlider.setTextValueSuffix(" Hz");
    hpfFreqSlider.setColour(juce::Slider::trackColourId, juce::Colour(0xfffbd38d));
    hpfFreqSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xfff6ad55));
    addAndMakeVisible(hpfFreqSlider);

    // LPF controls (purple color scheme)
    lpfLabel.setText("LPF", juce::dontSendNotification);
    lpfLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa0aec0));
    lpfLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(lpfLabel);

    lpfEnabledButton.setButtonText("");
    addAndMakeVisible(lpfEnabledButton);

    lpfFreqSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    lpfFreqSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 70, 20);
    lpfFreqSlider.setRange(2000.0, 20000.0, 1.0);
    lpfFreqSlider.setValue(12000.0);
    lpfFreqSlider.setSkewFactorFromMidPoint(8000.0);
    lpfFreqSlider.setTextValueSuffix(" Hz");
    lpfFreqSlider.setColour(juce::Slider::trackColourId, juce::Colour(0xffd6bcfa));
    lpfFreqSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffa78bfa));
    addAndMakeVisible(lpfFreqSlider);

    // Wire up parameter connections
    const auto& params = audioProcessor.getParameters();
    lowFreqSlider.onValueChange = [this, params] {
        auto* param = dynamic_cast<juce::AudioParameterFloat*>(params[0]);
        if (param) param->setValueNotifyingHost(param->convertTo0to1(lowFreqSlider.getValue()));
    };
    lowGainSlider.onValueChange = [this, params] {
        auto* param = dynamic_cast<juce::AudioParameterFloat*>(params[1]);
        if (param) param->setValueNotifyingHost(param->convertTo0to1(lowGainSlider.getValue()));
    };
    midFreqSlider.onValueChange = [this, params] {
        auto* param = dynamic_cast<juce::AudioParameterFloat*>(params[2]);
        if (param) param->setValueNotifyingHost(param->convertTo0to1(midFreqSlider.getValue()));
    };
    midGainSlider.onValueChange = [this, params] {
        auto* param = dynamic_cast<juce::AudioParameterFloat*>(params[3]);
        if (param) param->setValueNotifyingHost(param->convertTo0to1(midGainSlider.getValue()));
    };
    highFreqSlider.onValueChange = [this, params] {
        auto* param = dynamic_cast<juce::AudioParameterFloat*>(params[4]);
        if (param) param->setValueNotifyingHost(param->convertTo0to1(highFreqSlider.getValue()));
    };
    highGainSlider.onValueChange = [this, params] {
        auto* param = dynamic_cast<juce::AudioParameterFloat*>(params[5]);
        if (param) param->setValueNotifyingHost(param->convertTo0to1(highGainSlider.getValue()));
    };
    hpfEnabledButton.onClick = [this, params] {
        auto* param = dynamic_cast<juce::AudioParameterBool*>(params[6]);
        if (param) param->setValueNotifyingHost(hpfEnabledButton.getToggleState() ? 1.0f : 0.0f);
    };
    hpfFreqSlider.onValueChange = [this, params] {
        auto* param = dynamic_cast<juce::AudioParameterFloat*>(params[7]);
        if (param) param->setValueNotifyingHost(param->convertTo0to1(hpfFreqSlider.getValue()));
    };
    lpfEnabledButton.onClick = [this, params] {
        auto* param = dynamic_cast<juce::AudioParameterBool*>(params[8]);
        if (param) param->setValueNotifyingHost(lpfEnabledButton.getToggleState() ? 1.0f : 0.0f);
    };
    lpfFreqSlider.onValueChange = [this, params] {
        auto* param = dynamic_cast<juce::AudioParameterFloat*>(params[9]);
        if (param) param->setValueNotifyingHost(param->convertTo0to1(lpfFreqSlider.getValue()));
    };

    // Start timer for level meter updates (30 Hz)
    startTimer(33);
}

SimpleEQEditor::~SimpleEQEditor()
{
}

void SimpleEQEditor::paint(juce::Graphics& g)
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
    g.drawText("SimpleEQ", 21, 11, getWidth() - 40, 40, juce::Justification::centred);

    g.setColour(juce::Colour(0xff68d391));
    g.drawText("SimpleEQ", 20, 10, getWidth() - 40, 40, juce::Justification::centred);

    // Section labels
    g.setColour(juce::Colour(0xffa0aec0));
    g.setFont(12.0f);
    g.drawText("LOW", 40, 60, 100, 20, juce::Justification::centred);
    g.drawText("MID", 220, 60, 100, 20, juce::Justification::centred);
    g.drawText("HIGH", 400, 60, 100, 20, juce::Justification::centred);
    g.drawText("FILTERS", 20, 280, 100, 20, juce::Justification::left);

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

void SimpleEQEditor::resized()
{
    const int knobSize = 90;
    const int margin = 20;

    // EQ band controls (3 columns)
    int y = 90;

    // Low band
    int x = 40;
    lowFreqSlider.setBounds(x, y, knobSize, knobSize);
    lowGainSlider.setBounds(x, y + knobSize + 40, knobSize, knobSize);

    // Mid band
    x = 220;
    midFreqSlider.setBounds(x, y, knobSize, knobSize);
    midGainSlider.setBounds(x, y + knobSize + 40, knobSize, knobSize);

    // High band
    x = 400;
    highFreqSlider.setBounds(x, y, knobSize, knobSize);
    highGainSlider.setBounds(x, y + knobSize + 40, knobSize, knobSize);

    // Filter controls
    y = 310;
    hpfLabel.setBounds(margin, y, 40, 24);
    hpfEnabledButton.setBounds(margin + 50, y, 40, 24);
    hpfFreqSlider.setBounds(margin + 100, y, 200, 24);

    y += 35;
    lpfLabel.setBounds(margin, y, 40, 24);
    lpfEnabledButton.setBounds(margin + 50, y, 40, 24);
    lpfFreqSlider.setBounds(margin + 100, y, 200, 24);
}

void SimpleEQEditor::timerCallback()
{
    // Update level meter displays
    displayInputLevel = audioProcessor.getCurrentInputLevel();
    displayOutputLevel = audioProcessor.getCurrentOutputLevel();
    repaint();
}
