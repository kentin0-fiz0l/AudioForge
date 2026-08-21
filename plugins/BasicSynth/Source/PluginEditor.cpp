#include "PluginProcessor.h"
#include "PluginEditor.h"

BasicSynthEditor::BasicSynthEditor(BasicSynthProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Set editor size
    setSize(500, 400);

    // Waveform selector
    waveformLabel.setText("Waveform", juce::dontSendNotification);
    waveformLabel.attachToComponent(&waveformSelector, true);
    addAndMakeVisible(waveformLabel);

    waveformSelector.addItem("Sine", 1);
    waveformSelector.addItem("Sawtooth", 2);
    waveformSelector.addItem("Square", 3);
    waveformSelector.setSelectedId(1);
    waveformSelector.onChange = [this]
    {
        auto* param = audioProcessor.getParameters()[0];
        auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param);
        if (choiceParam != nullptr)
        {
            float normalizedValue = static_cast<float>(waveformSelector.getSelectedId() - 1) /
                                  (waveformSelector.getNumItems() - 1);
            choiceParam->setValueNotifyingHost(normalizedValue);
        }
    };
    addAndMakeVisible(waveformSelector);

    // Volume slider
    volumeLabel.setText("Volume", juce::dontSendNotification);
    volumeLabel.attachToComponent(&volumeSlider, true);
    addAndMakeVisible(volumeLabel);

    volumeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.7);
    volumeSlider.onValueChange = [this]
    {
        auto* param = audioProcessor.getParameters()[1];
        auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
        if (floatParam != nullptr)
            floatParam->setValueNotifyingHost(floatParam->convertTo0to1(volumeSlider.getValue()));
    };
    addAndMakeVisible(volumeSlider);

    // Attack slider
    attackLabel.setText("Attack", juce::dontSendNotification);
    attackLabel.attachToComponent(&attackSlider, true);
    addAndMakeVisible(attackLabel);

    attackSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    attackSlider.setRange(0.001, 2.0, 0.001);
    attackSlider.setValue(0.01);
    attackSlider.setSkewFactorFromMidPoint(0.1);
    attackSlider.setTextValueSuffix(" s");
    attackSlider.onValueChange = [this]
    {
        auto* param = audioProcessor.getParameters()[2];
        auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
        if (floatParam != nullptr)
            floatParam->setValueNotifyingHost(floatParam->convertTo0to1(attackSlider.getValue()));
    };
    addAndMakeVisible(attackSlider);

    // Decay slider
    decayLabel.setText("Decay", juce::dontSendNotification);
    decayLabel.attachToComponent(&decaySlider, true);
    addAndMakeVisible(decayLabel);

    decaySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    decaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    decaySlider.setRange(0.001, 2.0, 0.001);
    decaySlider.setValue(0.1);
    decaySlider.setSkewFactorFromMidPoint(0.1);
    decaySlider.setTextValueSuffix(" s");
    decaySlider.onValueChange = [this]
    {
        auto* param = audioProcessor.getParameters()[3];
        auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
        if (floatParam != nullptr)
            floatParam->setValueNotifyingHost(floatParam->convertTo0to1(decaySlider.getValue()));
    };
    addAndMakeVisible(decaySlider);

    // Sustain slider
    sustainLabel.setText("Sustain", juce::dontSendNotification);
    sustainLabel.attachToComponent(&sustainSlider, true);
    addAndMakeVisible(sustainLabel);

    sustainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    sustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    sustainSlider.setRange(0.0, 1.0, 0.01);
    sustainSlider.setValue(0.7);
    sustainSlider.onValueChange = [this]
    {
        auto* param = audioProcessor.getParameters()[4];
        auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
        if (floatParam != nullptr)
            floatParam->setValueNotifyingHost(floatParam->convertTo0to1(sustainSlider.getValue()));
    };
    addAndMakeVisible(sustainSlider);

    // Release slider
    releaseLabel.setText("Release", juce::dontSendNotification);
    releaseLabel.attachToComponent(&releaseSlider, true);
    addAndMakeVisible(releaseLabel);

    releaseSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    releaseSlider.setRange(0.001, 5.0, 0.001);
    releaseSlider.setValue(0.3);
    releaseSlider.setSkewFactorFromMidPoint(0.3);
    releaseSlider.setTextValueSuffix(" s");
    releaseSlider.onValueChange = [this]
    {
        auto* param = audioProcessor.getParameters()[5];
        auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
        if (floatParam != nullptr)
            floatParam->setValueNotifyingHost(floatParam->convertTo0to1(releaseSlider.getValue()));
    };
    addAndMakeVisible(releaseSlider);

    // Start timer for level meter updates (30 Hz)
    startTimer(33);
}

BasicSynthEditor::~BasicSynthEditor()
{
}

void BasicSynthEditor::paint(juce::Graphics& g)
{
    // Background
    g.fillAll(juce::Colours::darkgrey);

    // Title
    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawText("BasicSynth", 20, 10, getWidth() - 40, 30, juce::Justification::centred);

    // Level meter
    g.setColour(juce::Colours::black);
    g.fillRect(20, getHeight() - 50, getWidth() - 40, 30);

    g.setColour(juce::Colours::green);
    int meterWidth = static_cast<int>((getWidth() - 40) * displayLevel);
    g.fillRect(20, getHeight() - 50, meterWidth, 30);

    g.setColour(juce::Colours::white);
    g.drawRect(20, getHeight() - 50, getWidth() - 40, 30);
}

void BasicSynthEditor::resized()
{
    const int labelWidth = 80;
    const int controlWidth = 100;
    const int controlHeight = 100;
    const int margin = 20;

    int y = 60;

    // Waveform selector (wider, not rotary)
    waveformSelector.setBounds(margin + labelWidth, y, 200, 24);
    y += 50;

    // ADSR controls in a row
    int x = margin + labelWidth;
    attackSlider.setBounds(x, y, controlWidth, controlHeight);
    x += controlWidth + 10;

    decaySlider.setBounds(x, y, controlWidth, controlHeight);
    x += controlWidth + 10;

    sustainSlider.setBounds(x, y, controlWidth, controlHeight);
    x += controlWidth + 10;

    releaseSlider.setBounds(x, y, controlWidth, controlHeight);

    y += controlHeight + 20;

    // Volume control
    volumeSlider.setBounds(margin + labelWidth, y, controlWidth, controlHeight);
}

void BasicSynthEditor::timerCallback()
{
    // Update level meter display
    displayLevel = audioProcessor.getCurrentLevel();
    repaint();
}
