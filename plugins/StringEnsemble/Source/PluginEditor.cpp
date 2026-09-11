#include "PluginEditor.h"

StringEnsembleEditor::StringEnsembleEditor(StringEnsembleProcessor& p)
    : AudioProcessorEditor(&p), processor_(p)
{
    setSize(700, 480);

    // Preset browser
    addAndMakeVisible(presetBrowser_);

    // String type selector
    stringTypeLabel_.setText("String Type", juce::dontSendNotification);
    stringTypeLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(stringTypeLabel_);

    stringTypeCombo_.addItem("Violin", 1);
    stringTypeCombo_.addItem("Viola", 2);
    stringTypeCombo_.addItem("Cello", 3);
    stringTypeCombo_.addItem("Bass", 4);
    addAndMakeVisible(stringTypeCombo_);
    stringTypeAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor_.getValueTreeState(), "stringType", stringTypeCombo_);

    // Controls
    brightnessLabel_.setText("Brightness", juce::dontSendNotification);
    brightnessLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(brightnessLabel_);
    setupRotarySlider(brightnessSlider_);
    addAndMakeVisible(brightnessSlider_);
    brightnessAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getValueTreeState(), "brightness", brightnessSlider_);

    attackLabel_.setText("Attack", juce::dontSendNotification);
    attackLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(attackLabel_);
    setupRotarySlider(attackSlider_);
    addAndMakeVisible(attackSlider_);
    attackAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getValueTreeState(), "attack", attackSlider_);

    decayLabel_.setText("Decay", juce::dontSendNotification);
    decayLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(decayLabel_);
    setupRotarySlider(decaySlider_);
    addAndMakeVisible(decaySlider_);
    decayAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getValueTreeState(), "decay", decaySlider_);

    vibratoDepthLabel_.setText("Vibrato Depth", juce::dontSendNotification);
    vibratoDepthLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(vibratoDepthLabel_);
    setupRotarySlider(vibratoDepthSlider_);
    addAndMakeVisible(vibratoDepthSlider_);
    vibratoDepthAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getValueTreeState(), "vibratoDepth", vibratoDepthSlider_);

    vibratoRateLabel_.setText("Vibrato Rate", juce::dontSendNotification);
    vibratoRateLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(vibratoRateLabel_);
    setupRotarySlider(vibratoRateSlider_);
    addAndMakeVisible(vibratoRateSlider_);
    vibratoRateAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getValueTreeState(), "vibratoRate", vibratoRateSlider_);

    ensembleLabel_.setText("Ensemble", juce::dontSendNotification);
    ensembleLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(ensembleLabel_);
    setupRotarySlider(ensembleSlider_);
    addAndMakeVisible(ensembleSlider_);
    ensembleAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getValueTreeState(), "ensembleSize", ensembleSlider_);
}

StringEnsembleEditor::~StringEnsembleEditor() {}

void StringEnsembleEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff2e1a1a)); // Dark burgundy

    auto bounds = getLocalBounds();

    // Title
    g.setColour(juce::Colour(0xffcc8866)); // Warm wood color for strings
    g.setFont(juce::Font(28.0f, juce::Font::bold));
    g.drawText("String Ensemble", bounds.removeFromTop(60), juce::Justification::centred);

    // Section headers
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xffcccccc));

    g.drawText("TONE", 20, 110, 200, 30, juce::Justification::left);
    g.drawText("EXPRESSION", 390, 110, 280, 30, juce::Justification::left);

    // Dividing lines
    g.setColour(juce::Colour(0xff555555));
    g.drawLine(20, 105, getWidth() - 20, 105, 2.0f);
}

void StringEnsembleEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(80);

    // String type selector
    stringTypeLabel_.setBounds(250, 45, 200, 20);
    stringTypeCombo_.setBounds(250, 70, 200, 25);

    int sliderSize = 80;
    int spacing = 95;

    // Tone section
    brightnessLabel_.setBounds(40, 130, sliderSize, 20);
    brightnessSlider_.setBounds(40, 155, sliderSize, sliderSize);

    attackLabel_.setBounds(40 + spacing, 130, sliderSize, 20);
    attackSlider_.setBounds(40 + spacing, 155, sliderSize, sliderSize);

    decayLabel_.setBounds(40 + spacing * 2, 130, sliderSize, 20);
    decaySlider_.setBounds(40 + spacing * 2, 155, sliderSize, sliderSize);

    // Expression section
    vibratoDepthLabel_.setBounds(410, 130, sliderSize + 15, 20);
    vibratoDepthSlider_.setBounds(410, 155, sliderSize, sliderSize);

    vibratoRateLabel_.setBounds(410 + spacing, 130, sliderSize + 15, 20);
    vibratoRateSlider_.setBounds(410 + spacing, 155, sliderSize, sliderSize);

    // Ensemble
    ensembleLabel_.setBounds(235, 270, sliderSize, 20);
    ensembleSlider_.setBounds(235, 295, sliderSize, sliderSize);
}

void StringEnsembleEditor::setupRotarySlider(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffcc8866)); // Wood
    slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffcc8866));
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}
