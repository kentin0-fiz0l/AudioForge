#include "PluginEditor.h"

BrassSectionEditor::BrassSectionEditor(BrassSectionProcessor& p)
    : AudioProcessorEditor(&p), processor_(p),
      presetBrowser_(processor_.getPresetManager())
{
    setSize(700, 500);

    // Preset browser
    addAndMakeVisible(presetBrowser_);

    // Instrument selector
    instrumentLabel_.setText("Instrument", juce::dontSendNotification);
    instrumentLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(instrumentLabel_);

    instrumentCombo_.addItem("Trumpet", 1);
    instrumentCombo_.addItem("Trombone", 2);
    instrumentCombo_.addItem("Saxophone", 3);
    addAndMakeVisible(instrumentCombo_);
    instrumentAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor_.getValueTreeState(), "instrument", instrumentCombo_);

    // Articulation selector
    articulationLabel_.setText("Articulation", juce::dontSendNotification);
    articulationLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(articulationLabel_);

    articulationCombo_.addItem("Sustain", 1);
    articulationCombo_.addItem("Staccato", 2);
    articulationCombo_.addItem("Marcato", 3);
    articulationCombo_.addItem("Fall-off", 4);
    addAndMakeVisible(articulationCombo_);
    articulationAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor_.getValueTreeState(), "articulation", articulationCombo_);

    // Tone controls
    brightnessLabel_.setText("Brightness", juce::dontSendNotification);
    brightnessLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(brightnessLabel_);
    setupRotarySlider(brightnessSlider_);
    addAndMakeVisible(brightnessSlider_);
    brightnessAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getValueTreeState(), "brightness", brightnessSlider_);

    breathLabel_.setText("Breath", juce::dontSendNotification);
    breathLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(breathLabel_);
    setupRotarySlider(breathSlider_);
    addAndMakeVisible(breathSlider_);
    breathAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getValueTreeState(), "breathNoise", breathSlider_);

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

    sectionLabel_.setText("Section Size", juce::dontSendNotification);
    sectionLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(sectionLabel_);
    setupRotarySlider(sectionSlider_);
    addAndMakeVisible(sectionSlider_);
    sectionAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getValueTreeState(), "sectionSize", sectionSlider_);
}

BrassSectionEditor::~BrassSectionEditor()
{
}

void BrassSectionEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1a2e));

    auto bounds = getLocalBounds();

    // Title
    g.setColour(juce::Colour(0xffd4af37)); // Gold color for brass
    g.setFont(juce::Font(28.0f, juce::Font::bold));
    g.drawText("Brass Section", bounds.removeFromTop(60), juce::Justification::centred);

    // Section headers
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xffcccccc));

    g.drawText("INSTRUMENT", 20, 100, 300, 30, juce::Justification::left);
    g.drawText("TONE", 20, 230, 200, 30, juce::Justification::left);
    g.drawText("EXPRESSION", 400, 230, 280, 30, juce::Justification::left);

    // Dividing lines
    g.setColour(juce::Colour(0xff555555));
    g.drawLine(20, 95, getWidth() - 20, 95, 2.0f);
    g.drawLine(20, 225, getWidth() - 20, 225, 2.0f);
}

void BrassSectionEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(80);

    // Instrument and articulation selection (top row)
    instrumentLabel_.setBounds(40, 110, 120, 20);
    instrumentCombo_.setBounds(40, 135, 140, 25);

    articulationLabel_.setBounds(220, 110, 120, 20);
    articulationCombo_.setBounds(220, 135, 140, 25);

    // Tone controls (middle row)
    int sliderSize = 80;
    int spacing = 95;

    brightnessLabel_.setBounds(40, 250, sliderSize, 20);
    brightnessSlider_.setBounds(40, 275, sliderSize, sliderSize);

    breathLabel_.setBounds(40 + spacing, 250, sliderSize, 20);
    breathSlider_.setBounds(40 + spacing, 275, sliderSize, sliderSize);

    sectionLabel_.setBounds(40 + spacing * 2, 250, sliderSize, 20);
    sectionSlider_.setBounds(40 + spacing * 2, 275, sliderSize, sliderSize);

    // Expression controls (right section)
    vibratoDepthLabel_.setBounds(410, 250, sliderSize + 15, 20);
    vibratoDepthSlider_.setBounds(410, 275, sliderSize, sliderSize);

    vibratoRateLabel_.setBounds(410 + spacing, 250, sliderSize + 15, 20);
    vibratoRateSlider_.setBounds(410 + spacing, 275, sliderSize, sliderSize);
}

void BrassSectionEditor::setupRotarySlider(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffd4af37)); // Gold
    slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffd4af37));
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}
