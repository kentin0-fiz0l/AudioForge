#include "PluginProcessor.h"
#include "PluginEditor.h"

BasicSynthEditor::BasicSynthEditor(BasicSynthProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Set editor size (expanded for all new controls)
    setSize(700, 650);

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
    volumeLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa0aec0));
    volumeLabel.setJustificationType(juce::Justification::centred);
    volumeLabel.attachToComponent(&volumeSlider, false);
    addAndMakeVisible(volumeLabel);

    volumeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.7);
    volumeSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff63b3ed));
    volumeSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xff4299e1));
    volumeSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffedf2f7));
    volumeSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff2d3748));
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
    attackLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa0aec0));
    attackLabel.setJustificationType(juce::Justification::centred);
    attackLabel.attachToComponent(&attackSlider, false);
    addAndMakeVisible(attackLabel);

    attackSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    attackSlider.setRange(0.001, 2.0, 0.001);
    attackSlider.setValue(0.01);
    attackSlider.setSkewFactorFromMidPoint(0.1);
    attackSlider.setTextValueSuffix(" s");
    attackSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff68d391));
    attackSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xff48bb78));
    attackSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffedf2f7));
    attackSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff2d3748));
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
    decayLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa0aec0));
    decayLabel.setJustificationType(juce::Justification::centred);
    decayLabel.attachToComponent(&decaySlider, false);
    addAndMakeVisible(decayLabel);

    decaySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    decaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    decaySlider.setRange(0.001, 2.0, 0.001);
    decaySlider.setValue(0.1);
    decaySlider.setSkewFactorFromMidPoint(0.1);
    decaySlider.setTextValueSuffix(" s");
    decaySlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xfffbd38d));
    decaySlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xfff6ad55));
    decaySlider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffedf2f7));
    decaySlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff2d3748));
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
    sustainLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa0aec0));
    sustainLabel.setJustificationType(juce::Justification::centred);
    sustainLabel.attachToComponent(&sustainSlider, false);
    addAndMakeVisible(sustainLabel);

    sustainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    sustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    sustainSlider.setRange(0.0, 1.0, 0.01);
    sustainSlider.setValue(0.7);
    sustainSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xfffc8181));
    sustainSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xfff56565));
    sustainSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffedf2f7));
    sustainSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff2d3748));
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
    releaseLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa0aec0));
    releaseLabel.setJustificationType(juce::Justification::centred);
    releaseLabel.attachToComponent(&releaseSlider, false);
    addAndMakeVisible(releaseLabel);

    releaseSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    releaseSlider.setRange(0.001, 5.0, 0.001);
    releaseSlider.setValue(0.3);
    releaseSlider.setSkewFactorFromMidPoint(0.3);
    releaseSlider.setTextValueSuffix(" s");
    releaseSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffb794f4));
    releaseSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xff9f7aea));
    releaseSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffedf2f7));
    releaseSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff2d3748));
    releaseSlider.onValueChange = [this]
    {
        auto* param = audioProcessor.getParameters()[5];
        auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
        if (floatParam != nullptr)
            floatParam->setValueNotifyingHost(floatParam->convertTo0to1(releaseSlider.getValue()));
    };
    addAndMakeVisible(releaseSlider);

    // Filter Cutoff slider
    filterCutoffLabel.setText("Cutoff", juce::dontSendNotification);
    filterCutoffLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa0aec0));
    filterCutoffLabel.setJustificationType(juce::Justification::centred);
    filterCutoffLabel.attachToComponent(&filterCutoffSlider, false);
    addAndMakeVisible(filterCutoffLabel);

    filterCutoffSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    filterCutoffSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    filterCutoffSlider.setRange(20.0, 20000.0, 1.0);
    filterCutoffSlider.setValue(20000.0);
    filterCutoffSlider.setSkewFactorFromMidPoint(1000.0);  // Log scale with 1kHz midpoint
    filterCutoffSlider.setTextValueSuffix(" Hz");
    filterCutoffSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff81e6d9));
    filterCutoffSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xff4fd1c5));
    filterCutoffSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffedf2f7));
    filterCutoffSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff2d3748));
    filterCutoffSlider.onValueChange = [this]
    {
        auto* param = audioProcessor.getParameters()[6];
        auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
        if (floatParam != nullptr)
            floatParam->setValueNotifyingHost(floatParam->convertTo0to1(filterCutoffSlider.getValue()));
    };
    addAndMakeVisible(filterCutoffSlider);

    // Filter Resonance slider
    filterResonanceLabel.setText("Resonance", juce::dontSendNotification);
    filterResonanceLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa0aec0));
    filterResonanceLabel.setJustificationType(juce::Justification::centred);
    filterResonanceLabel.attachToComponent(&filterResonanceSlider, false);
    addAndMakeVisible(filterResonanceLabel);

    filterResonanceSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    filterResonanceSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    filterResonanceSlider.setRange(0.5, 10.0, 0.1);
    filterResonanceSlider.setValue(0.707);
    filterResonanceSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xfffbb6ce));
    filterResonanceSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xfff687b3));
    filterResonanceSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffedf2f7));
    filterResonanceSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff2d3748));
    filterResonanceSlider.onValueChange = [this]
    {
        auto* param = audioProcessor.getParameters()[7];
        auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
        if (floatParam != nullptr)
            floatParam->setValueNotifyingHost(floatParam->convertTo0to1(filterResonanceSlider.getValue()));
    };
    addAndMakeVisible(filterResonanceSlider);

    // Filter Type selector
    filterTypeLabel.setText("Filter Type", juce::dontSendNotification);
    filterTypeLabel.attachToComponent(&filterTypeSelector, true);
    addAndMakeVisible(filterTypeLabel);

    filterTypeSelector.addItem("Low-pass", 1);
    filterTypeSelector.addItem("High-pass", 2);
    filterTypeSelector.addItem("Band-pass", 3);
    filterTypeSelector.addItem("Notch", 4);
    filterTypeSelector.setSelectedId(1);
    filterTypeSelector.onChange = [this]
    {
        auto* param = audioProcessor.getParameters()[8];
        auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param);
        if (choiceParam != nullptr)
        {
            float normalizedValue = static_cast<float>(filterTypeSelector.getSelectedId() - 1) /
                                  (filterTypeSelector.getNumItems() - 1);
            choiceParam->setValueNotifyingHost(normalizedValue);
        }
    };
    addAndMakeVisible(filterTypeSelector);

    // Chorus Mix slider
    chorusMixLabel.setText("Chorus", juce::dontSendNotification);
    chorusMixLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa0aec0));
    chorusMixLabel.setJustificationType(juce::Justification::centred);
    chorusMixLabel.attachToComponent(&chorusMixSlider, false);
    addAndMakeVisible(chorusMixLabel);

    chorusMixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    chorusMixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    chorusMixSlider.setRange(0.0, 1.0, 0.01);
    chorusMixSlider.setValue(0.3);
    chorusMixSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffa78bfa));
    chorusMixSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xff9f7aea));
    chorusMixSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffedf2f7));
    chorusMixSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff2d3748));
    chorusMixSlider.onValueChange = [this]
    {
        auto* param = audioProcessor.getParameters()[11];
        auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
        if (floatParam != nullptr)
            floatParam->setValueNotifyingHost(floatParam->convertTo0to1(chorusMixSlider.getValue()));
    };
    addAndMakeVisible(chorusMixSlider);

    // Reverb Mix slider
    reverbMixLabel.setText("Reverb", juce::dontSendNotification);
    reverbMixLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa0aec0));
    reverbMixLabel.setJustificationType(juce::Justification::centred);
    reverbMixLabel.attachToComponent(&reverbMixSlider, false);
    addAndMakeVisible(reverbMixLabel);

    reverbMixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    reverbMixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    reverbMixSlider.setRange(0.0, 1.0, 0.01);
    reverbMixSlider.setValue(0.3);
    reverbMixSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff90cdf4));
    reverbMixSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xff63b3ed));
    reverbMixSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffedf2f7));
    reverbMixSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff2d3748));
    reverbMixSlider.onValueChange = [this]
    {
        auto* param = audioProcessor.getParameters()[14];
        auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
        if (floatParam != nullptr)
            floatParam->setValueNotifyingHost(floatParam->convertTo0to1(reverbMixSlider.getValue()));
    };
    addAndMakeVisible(reverbMixSlider);

    // Saturation Drive slider
    saturationDriveLabel.setText("Drive", juce::dontSendNotification);
    saturationDriveLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa0aec0));
    saturationDriveLabel.setJustificationType(juce::Justification::centred);
    saturationDriveLabel.attachToComponent(&saturationDriveSlider, false);
    addAndMakeVisible(saturationDriveLabel);

    saturationDriveSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    saturationDriveSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    saturationDriveSlider.setRange(0.0, 1.0, 0.01);
    saturationDriveSlider.setValue(0.0);
    saturationDriveSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xfffc8181));
    saturationDriveSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xfff56565));
    saturationDriveSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffedf2f7));
    saturationDriveSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff2d3748));
    saturationDriveSlider.onValueChange = [this]
    {
        auto* param = audioProcessor.getParameters()[15];
        auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
        if (floatParam != nullptr)
            floatParam->setValueNotifyingHost(floatParam->convertTo0to1(saturationDriveSlider.getValue()));
    };
    addAndMakeVisible(saturationDriveSlider);

    // Start timer for level meter updates (30 Hz)
    startTimer(33);
}

BasicSynthEditor::~BasicSynthEditor()
{
}

void BasicSynthEditor::paint(juce::Graphics& g)
{
    // Gradient background
    juce::ColourGradient gradient(juce::Colour(0xff2d3748), 0, 0,
                                 juce::Colour(0xff1a202c), 0, static_cast<float>(getHeight()),
                                 false);
    g.setGradientFill(gradient);
    g.fillAll();

    // Title area with rounded rectangle
    g.setColour(juce::Colour(0xff4a5568).withAlpha(0.3f));
    g.fillRoundedRectangle(10, 10, getWidth() - 20, 40, 8.0f);

    // Title text with shadow
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.setFont(juce::Font(26.0f, juce::Font::bold));
    g.drawText("BasicSynth", 21, 11, getWidth() - 40, 40, juce::Justification::centred);

    g.setColour(juce::Colour(0xff63b3ed));
    g.drawText("BasicSynth", 20, 10, getWidth() - 40, 40, juce::Justification::centred);

    // Section labels with better styling
    g.setColour(juce::Colour(0xffa0aec0));
    g.setFont(juce::Font(14.0f, juce::Font::bold));
    g.drawText("ENVELOPE", 100, 115, 200, 20, juce::Justification::left);
    g.drawText("FILTER", 100, 335, 200, 20, juce::Justification::left);
    g.drawText("EFFECTS", 100, 485, 200, 20, juce::Justification::left);

    // Level meter with improved design
    int meterX = 20;
    int meterY = getHeight() - 50;
    int meterW = getWidth() - 40;
    int meterH = 30;

    // Meter background with inset effect
    g.setColour(juce::Colours::black.withAlpha(0.8f));
    g.fillRoundedRectangle(meterX, meterY, meterW, meterH, 4.0f);

    // Meter level with gradient
    if (displayLevel > 0.0f)
    {
        int filledWidth = static_cast<int>(meterW * displayLevel);

        juce::ColourGradient meterGradient(
            juce::Colour(0xff48bb78), meterX, meterY,
            displayLevel > 0.8f ? juce::Colour(0xfff56565) : juce::Colour(0xff48bb78),
            meterX + filledWidth, meterY,
            false);

        g.setGradientFill(meterGradient);
        g.fillRoundedRectangle(meterX + 2, meterY + 2,
                              filledWidth - 4, meterH - 4, 2.0f);
    }

    // Meter border
    g.setColour(juce::Colour(0xff4a5568));
    g.drawRoundedRectangle(meterX, meterY, meterW, meterH, 4.0f, 2.0f);

    // Meter label
    g.setColour(juce::Colour(0xffa0aec0));
    g.setFont(10.0f);
    g.drawText("OUTPUT LEVEL", meterX, meterY - 15, 100, 15, juce::Justification::left);
}

void BasicSynthEditor::resized()
{
    const int labelWidth = 100;
    const int controlWidth = 90;
    const int controlHeight = 90;
    const int margin = 20;

    int y = 60;

    // Waveform selector styling
    waveformSelector.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff2d3748));
    waveformSelector.setColour(juce::ComboBox::textColourId, juce::Colour(0xffedf2f7));
    waveformSelector.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff4a5568));
    waveformSelector.setColour(juce::ComboBox::buttonColourId, juce::Colour(0xff4299e1));

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

    y += controlHeight + 20;

    // Filter controls in a row
    x = margin + labelWidth;
    filterCutoffSlider.setBounds(x, y, controlWidth, controlHeight);
    x += controlWidth + 10;
    filterResonanceSlider.setBounds(x, y, controlWidth, controlHeight);
    x += controlWidth + 10;

    // Filter type selector styling
    filterTypeSelector.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff2d3748));
    filterTypeSelector.setColour(juce::ComboBox::textColourId, juce::Colour(0xffedf2f7));
    filterTypeSelector.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff4a5568));
    filterTypeSelector.setColour(juce::ComboBox::buttonColourId, juce::Colour(0xff4fd1c5));

    filterTypeSelector.setBounds(x, y + 30, 120, 24);

    y += controlHeight + 30;

    // Effects controls in a row
    x = margin + labelWidth;
    chorusMixSlider.setBounds(x, y, controlWidth, controlHeight);
    x += controlWidth + 10;

    reverbMixSlider.setBounds(x, y, controlWidth, controlHeight);
    x += controlWidth + 10;

    saturationDriveSlider.setBounds(x, y, controlWidth, controlHeight);
}

void BasicSynthEditor::timerCallback()
{
    // Update level meter display
    displayLevel = audioProcessor.getCurrentLevel();
    repaint();
}
