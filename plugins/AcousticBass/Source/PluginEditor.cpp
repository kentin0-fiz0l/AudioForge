#include "PluginEditor.h"

AcousticBassEditor::AcousticBassEditor(AcousticBassProcessor& p)
    : AudioProcessorEditor(&p), processor_(p)
{
    setSize(700, 450);

    // Preset browser
    addAndMakeVisible(presetBrowser_);

    // Bass type selector
    bassTypeLabel_.setText("Bass Type", juce::dontSendNotification);
    bassTypeLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(bassTypeLabel_);

    bassTypeCombo_.addItem("Upright", 1);
    bassTypeCombo_.addItem("Electric", 2);
    addAndMakeVisible(bassTypeCombo_);
    bassTypeAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor_.getValueTreeState(), "bassType", bassTypeCombo_);

    // Play style selector
    playStyleLabel_.setText("Play Style", juce::dontSendNotification);
    playStyleLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(playStyleLabel_);

    playStyleCombo_.addItem("Fingered", 1);
    playStyleCombo_.addItem("Picked", 2);
    playStyleCombo_.addItem("Slapped", 3);
    addAndMakeVisible(playStyleCombo_);
    playStyleAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor_.getValueTreeState(), "playStyle", playStyleCombo_);

    // Controls
    auto setupControl = [this](juce::Label& label, const char* text, juce::Slider& slider, auto& attachment, const char* paramId) {
        label.setText(text, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);
        setupRotarySlider(slider);
        addAndMakeVisible(slider);
        attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor_.getValueTreeState(), paramId, slider);
    };

    setupControl(toneLabel_, "Tone", toneSlider_, toneAttachment_, "tone");
    setupControl(attackLabel_, "Attack", attackSlider_, attackAttachment_, "attack");
    setupControl(decayLabel_, "Decay", decaySlider_, decayAttachment_, "decay");
    setupControl(fretNoiseLabel_, "Fret Noise", fretNoiseSlider_, fretNoiseAttachment_, "fretNoise");
}

AcousticBassEditor::~AcousticBassEditor() {}

void AcousticBassEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1a1a));

    g.setColour(juce::Colour(0xff8b4513)); // Saddle brown for bass
    g.setFont(juce::Font(28.0f, juce::Font::bold));
    g.drawText("Acoustic Bass", getLocalBounds().removeFromTop(60), juce::Justification::centred);

    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xffcccccc));
    g.drawText("CONTROLS", 20, 110, 200, 30, juce::Justification::left);

    g.setColour(juce::Colour(0xff555555));
    g.drawLine(20, 105, getWidth() - 20, 105, 2.0f);
}

void AcousticBassEditor::resized()
{
    bassTypeLabel_.setBounds(200, 45, 140, 20);
    bassTypeCombo_.setBounds(200, 70, 140, 25);

    playStyleLabel_.setBounds(360, 45, 140, 20);
    playStyleCombo_.setBounds(360, 70, 140, 25);

    int sliderSize = 80;
    int spacing = 95;

    toneLabel_.setBounds(80, 130, sliderSize, 20);
    toneSlider_.setBounds(80, 155, sliderSize, sliderSize);

    attackLabel_.setBounds(80 + spacing, 130, sliderSize, 20);
    attackSlider_.setBounds(80 + spacing, 155, sliderSize, sliderSize);

    decayLabel_.setBounds(80 + spacing * 2, 130, sliderSize, 20);
    decaySlider_.setBounds(80 + spacing * 2, 155, sliderSize, sliderSize);

    fretNoiseLabel_.setBounds(80 + spacing * 3, 130, sliderSize, 20);
    fretNoiseSlider_.setBounds(80 + spacing * 3, 155, sliderSize, sliderSize);
}

void AcousticBassEditor::setupRotarySlider(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff8b4513));
    slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xff8b4513));
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}
