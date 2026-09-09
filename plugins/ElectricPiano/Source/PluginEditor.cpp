#include "PluginEditor.h"

ElectricPianoEditor::ElectricPianoEditor(ElectricPianoProcessor& p)
    : AudioProcessorEditor(&p), processor_(p)
{
    // Window size
    setSize(700, 500);

    // Model selector
    modelLabel_.setText("Model", juce::dontSendNotification);
    modelLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(modelLabel_);

    modelCombo_.addItem("Rhodes", 1);
    modelCombo_.addItem("Wurlitzer", 2);
    modelCombo_.addItem("Hybrid", 3);
    addAndMakeVisible(modelCombo_);
    modelAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor_.getValueTreeState(), "model", modelCombo_);

    // Tone controls
    toneLabel_.setText("Tone", juce::dontSendNotification);
    toneLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(toneLabel_);
    setupRotarySlider(toneSlider_);
    addAndMakeVisible(toneSlider_);
    toneAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getValueTreeState(), "tone", toneSlider_);

    decayLabel_.setText("Decay", juce::dontSendNotification);
    decayLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(decayLabel_);
    setupRotarySlider(decaySlider_);
    addAndMakeVisible(decaySlider_);
    decayAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getValueTreeState(), "decay", decaySlider_);

    noiseLabel_.setText("Mech Noise", juce::dontSendNotification);
    noiseLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(noiseLabel_);
    setupRotarySlider(noiseSlider_);
    addAndMakeVisible(noiseSlider_);
    noiseAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getValueTreeState(), "mechanicalNoise", noiseSlider_);

    velocityLabel_.setText("Velocity", juce::dontSendNotification);
    velocityLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(velocityLabel_);
    setupRotarySlider(velocitySlider_);
    addAndMakeVisible(velocitySlider_);
    velocityAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getValueTreeState(), "velocityCurve", velocitySlider_);

    // Effects
    tremoloDepthLabel_.setText("Tremolo Depth", juce::dontSendNotification);
    tremoloDepthLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(tremoloDepthLabel_);
    setupRotarySlider(tremoloDepthSlider_);
    addAndMakeVisible(tremoloDepthSlider_);
    tremoloDepthAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getValueTreeState(), "tremoloDepth", tremoloDepthSlider_);

    tremoloRateLabel_.setText("Tremolo Rate", juce::dontSendNotification);
    tremoloRateLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(tremoloRateLabel_);
    setupRotarySlider(tremoloRateSlider_);
    addAndMakeVisible(tremoloRateSlider_);
    tremoloRateAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getValueTreeState(), "tremoloRate", tremoloRateSlider_);

    chorusLabel_.setText("Chorus", juce::dontSendNotification);
    chorusLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(chorusLabel_);
    setupRotarySlider(chorusSlider_);
    addAndMakeVisible(chorusSlider_);
    chorusAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getValueTreeState(), "chorusDepth", chorusSlider_);

    reverbLabel_.setText("Reverb", juce::dontSendNotification);
    reverbLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(reverbLabel_);
    setupRotarySlider(reverbSlider_);
    addAndMakeVisible(reverbSlider_);
    reverbAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getValueTreeState(), "reverbMix", reverbSlider_);
}

ElectricPianoEditor::~ElectricPianoEditor()
{
}

void ElectricPianoEditor::paint(juce::Graphics& g)
{
    // Background gradient
    g.fillAll(juce::Colour(0xff2a2a2a));

    auto bounds = getLocalBounds();

    // Title
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(28.0f, juce::Font::bold));
    g.drawText("Electric Piano", bounds.removeFromTop(60), juce::Justification::centred);

    // Section headers
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xffcccccc));

    g.drawText("TONE", 20, 100, 200, 30, juce::Justification::left);
    g.drawText("EFFECTS", 400, 100, 280, 30, juce::Justification::left);

    // Dividing lines
    g.setColour(juce::Colour(0xff555555));
    g.drawLine(20, 95, getWidth() - 20, 95, 2.0f);
    g.drawLine(380, 110, 380, getHeight() - 20, 2.0f);
}

void ElectricPianoEditor::resized()
{
    auto bounds = getLocalBounds();

    // Title area
    bounds.removeFromTop(80);

    // Model selector (top center)
    modelLabel_.setBounds(bounds.getX() + 250, 40, 200, 20);
    modelCombo_.setBounds(bounds.getX() + 250, 65, 200, 25);

    // Tone section (left)
    auto toneSection = bounds.removeFromLeft(360);
    toneSection.removeFromTop(40);

    int sliderSize = 80;
    int spacing = 90;

    // Row 1: Tone, Decay
    toneLabel_.setBounds(40, 150, sliderSize, 20);
    toneSlider_.setBounds(40, 175, sliderSize, sliderSize);

    decayLabel_.setBounds(40 + spacing, 150, sliderSize, 20);
    decaySlider_.setBounds(40 + spacing, 175, sliderSize, sliderSize);

    // Row 2: Noise, Velocity
    noiseLabel_.setBounds(40, 280, sliderSize, 20);
    noiseSlider_.setBounds(40, 305, sliderSize, sliderSize);

    velocityLabel_.setBounds(40 + spacing, 280, sliderSize, 20);
    velocitySlider_.setBounds(40 + spacing, 305, sliderSize, sliderSize);

    // Effects section (right)
    tremoloDepthLabel_.setBounds(410, 150, sliderSize, 20);
    tremoloDepthSlider_.setBounds(410, 175, sliderSize, sliderSize);

    tremoloRateLabel_.setBounds(410 + spacing, 150, sliderSize, 20);
    tremoloRateSlider_.setBounds(410 + spacing, 175, sliderSize, sliderSize);

    chorusLabel_.setBounds(410, 280, sliderSize, 20);
    chorusSlider_.setBounds(410, 305, sliderSize, sliderSize);

    reverbLabel_.setBounds(410 + spacing, 280, sliderSize, 20);
    reverbSlider_.setBounds(410 + spacing, 305, sliderSize, sliderSize);
}

void ElectricPianoEditor::setupRotarySlider(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff4a9eff));
    slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xff4a9eff));
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}
