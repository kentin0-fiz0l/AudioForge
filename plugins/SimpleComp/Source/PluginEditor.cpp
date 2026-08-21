#include "PluginEditor.h"

SimpleCompEditor::SimpleCompEditor(SimpleCompProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Set editor size
    setSize(600, 500);

    // Get parameters
    const auto& params = audioProcessor.getParameters();

    // Setup Threshold slider (vertical)
    thresholdSlider.setSliderStyle(juce::Slider::LinearVertical);
    thresholdSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    for (auto* param : params)
    {
        if (juce::String(param->getName(32)).contains("Threshold"))
        {
            auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
            thresholdSlider.setRange(floatParam->range.start, floatParam->range.end, 0.1);
            thresholdSlider.setValue(floatParam->get());
            thresholdSlider.onValueChange = [this, param]()
            {
                auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
                floatParam->setValueNotifyingHost(floatParam->convertTo0to1(thresholdSlider.getValue()));
            };
        }
    }
    addAndMakeVisible(thresholdSlider);

    thresholdLabel.setText("Threshold", juce::dontSendNotification);
    thresholdLabel.setJustificationType(juce::Justification::centred);
    thresholdLabel.attachToComponent(&thresholdSlider, false);
    addAndMakeVisible(thresholdLabel);

    // Setup Ratio slider (vertical)
    ratioSlider.setSliderStyle(juce::Slider::LinearVertical);
    ratioSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    for (auto* param : params)
    {
        if (juce::String(param->getName(32)).contains("Ratio"))
        {
            auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
            ratioSlider.setRange(floatParam->range.start, floatParam->range.end, 0.1);
            ratioSlider.setValue(floatParam->get());
            ratioSlider.onValueChange = [this, param]()
            {
                auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
                floatParam->setValueNotifyingHost(floatParam->convertTo0to1(ratioSlider.getValue()));
            };
        }
    }
    addAndMakeVisible(ratioSlider);

    ratioLabel.setText("Ratio", juce::dontSendNotification);
    ratioLabel.setJustificationType(juce::Justification::centred);
    ratioLabel.attachToComponent(&ratioSlider, false);
    addAndMakeVisible(ratioLabel);

    // Setup Attack slider (rotary)
    attackSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    for (auto* param : params)
    {
        if (juce::String(param->getName(32)).contains("Attack"))
        {
            auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
            attackSlider.setRange(floatParam->range.start, floatParam->range.end, 0.1);
            attackSlider.setSkewFactor(0.3);  // Log scale
            attackSlider.setValue(floatParam->get());
            attackSlider.onValueChange = [this, param]()
            {
                auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
                floatParam->setValueNotifyingHost(floatParam->convertTo0to1(attackSlider.getValue()));
            };
        }
    }
    addAndMakeVisible(attackSlider);

    attackLabel.setText("Attack (ms)", juce::dontSendNotification);
    attackLabel.setJustificationType(juce::Justification::centred);
    attackLabel.attachToComponent(&attackSlider, false);
    addAndMakeVisible(attackLabel);

    // Setup Release slider (rotary)
    releaseSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    for (auto* param : params)
    {
        if (juce::String(param->getName(32)).contains("Release"))
        {
            auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
            releaseSlider.setRange(floatParam->range.start, floatParam->range.end, 1.0);
            releaseSlider.setSkewFactor(0.3);  // Log scale
            releaseSlider.setValue(floatParam->get());
            releaseSlider.onValueChange = [this, param]()
            {
                auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
                floatParam->setValueNotifyingHost(floatParam->convertTo0to1(releaseSlider.getValue()));
            };
        }
    }
    addAndMakeVisible(releaseSlider);

    releaseLabel.setText("Release (ms)", juce::dontSendNotification);
    releaseLabel.setJustificationType(juce::Justification::centred);
    releaseLabel.attachToComponent(&releaseSlider, false);
    addAndMakeVisible(releaseLabel);

    // Setup Knee combo box
    for (auto* param : params)
    {
        if (juce::String(param->getName(32)).contains("Knee"))
        {
            auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param);
            for (const auto& choice : choiceParam->choices)
                kneeComboBox.addItem(choice, kneeComboBox.getNumItems() + 1);

            kneeComboBox.setSelectedId(choiceParam->getIndex() + 1);
            kneeComboBox.onChange = [this, param, choiceParam]()
            {
                int index = kneeComboBox.getSelectedId() - 1;
                choiceParam->setValueNotifyingHost(static_cast<float>(index) / static_cast<float>(choiceParam->choices.size() - 1));
            };
        }
    }
    addAndMakeVisible(kneeComboBox);

    kneeLabel.setText("Knee", juce::dontSendNotification);
    kneeLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(kneeLabel);

    // Start timer for UI updates (30 FPS)
    startTimer(33);
}

SimpleCompEditor::~SimpleCompEditor()
{
    stopTimer();
}

void SimpleCompEditor::timerCallback()
{
    // Update gain reduction display
    displayGainReduction = audioProcessor.getCurrentGainReduction();
    repaint();
}

void SimpleCompEditor::paint(juce::Graphics& g)
{
    // Background gradient
    g.fillAll(juce::Colour(0xff1a1a1a));
    auto bounds = getLocalBounds();

    juce::ColourGradient gradient(
        juce::Colour(0xff2a2a2a), bounds.getX(), bounds.getY(),
        juce::Colour(0xff1a1a1a), bounds.getX(), bounds.getBottom(),
        false);
    g.setGradientFill(gradient);
    g.fillRect(bounds);

    // Title
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(26.0f, juce::Font::bold));
    g.drawText("SimpleComp", 0, 10, getWidth(), 40, juce::Justification::centred);

    // Subtitle
    g.setFont(juce::FontOptions(14.0f));
    g.setColour(juce::Colours::lightgrey);
    g.drawText("Dynamic Range Compressor", 0, 45, getWidth(), 20, juce::Justification::centred);

    // Gain reduction meter
    int meterX = 20;
    int meterY = 80;
    int meterWidth = 560;
    int meterHeight = 40;

    // Meter background
    g.setColour(juce::Colour(0xff333333));
    g.fillRoundedRectangle(meterX, meterY, meterWidth, meterHeight, 4.0f);

    // Meter fill (0 to -20dB range)
    float meterFill = juce::jmap(displayGainReduction, 0.0f, 20.0f, 0.0f, 1.0f);
    meterFill = juce::jlimit(0.0f, 1.0f, meterFill);

    if (meterFill > 0.0f)
    {
        // Gradient from yellow to red
        juce::ColourGradient meterGradient(
            juce::Colours::orange, meterX, meterY,
            juce::Colours::red, meterX + meterWidth, meterY,
            false);
        g.setGradientFill(meterGradient);
        g.fillRoundedRectangle(meterX, meterY, meterWidth * meterFill, meterHeight, 4.0f);
    }

    // Meter border
    g.setColour(juce::Colours::grey);
    g.drawRoundedRectangle(meterX, meterY, meterWidth, meterHeight, 4.0f, 2.0f);

    // Meter label
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(12.0f));
    g.drawText("Gain Reduction", meterX, meterY - 20, meterWidth, 20, juce::Justification::centredLeft);

    // Meter value
    juce::String grText = juce::String(displayGainReduction, 1) + " dB";
    g.drawText(grText, meterX, meterY + meterHeight + 5, meterWidth, 20, juce::Justification::centredLeft);
}

void SimpleCompEditor::resized()
{
    auto bounds = getLocalBounds();
    int controlsY = 180;

    // Vertical sliders (left side)
    int sliderWidth = 100;
    int sliderHeight = 200;

    thresholdSlider.setBounds(50, controlsY, sliderWidth, sliderHeight);
    ratioSlider.setBounds(180, controlsY, sliderWidth, sliderHeight);

    // Rotary knobs (right side)
    int knobSize = 120;
    int knobY = controlsY + 40;

    attackSlider.setBounds(320, knobY, knobSize, knobSize);
    releaseSlider.setBounds(460, knobY, knobSize, knobSize);

    // Knee selector (bottom)
    kneeLabel.setBounds(50, 420, 100, 30);
    kneeComboBox.setBounds(150, 420, 150, 30);
}
