#include "PluginEditor.h"

KickEditor::KickEditor(KickProcessor& p)
    : AudioProcessorEditor(&p), processor_(p),
      presetBrowser_(processor_.getPresetManager())
{
    setSize(750, 400);

    // Preset browser
    addAndMakeVisible(presetBrowser_);

    auto setupControl = [this](juce::Label& label, const char* text, juce::Slider& slider, auto& attachment, const char* paramId) {
        label.setText(text, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, juce::Colour(0xffdddddd));
        addAndMakeVisible(label);
        setupRotarySlider(slider);
        addAndMakeVisible(slider);
        attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor_.getValueTreeState(), paramId, slider);
    };

    setupControl(pitchLabel_, "Pitch", pitchSlider_, pitchAttachment_, "pitch");
    setupControl(pitchEnvAmountLabel_, "Pitch Env", pitchEnvAmountSlider_, pitchEnvAmountAttachment_, "pitchEnvAmount");
    setupControl(pitchEnvDecayLabel_, "Env Decay", pitchEnvDecaySlider_, pitchEnvDecayAttachment_, "pitchEnvDecay");
    setupControl(decayLabel_, "Decay", decaySlider_, decayAttachment_, "decay");
    setupControl(clickLabel_, "Click", clickSlider_, clickAttachment_, "click");
    setupControl(driveLabel_, "Drive", driveSlider_, driveAttachment_, "drive");
    setupControl(toneLabel_, "Tone", toneSlider_, toneAttachment_, "tone");
}

KickEditor::~KickEditor() {}

void KickEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1a1a));
    g.setColour(juce::Colour(0xffff3333)); // Red
    g.setFont(juce::Font(42.0f, juce::Font::bold));
    g.drawText("ANALOG KICK", getLocalBounds().removeFromTop(70), juce::Justification::centred);

    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff222222));
    g.fillRect(20, 90, 350, 280); // Pitch section
    g.fillRect(390, 90, 340, 280); // Body section

    g.setColour(juce::Colour(0xffff3333));
    g.drawText("PITCH ENVELOPE", 30, 95, 330, 25, juce::Justification::left);
    g.drawText("BODY & TONE", 400, 95, 320, 25, juce::Justification::left);
}

void KickEditor::resized()
{
    int knobSize = 80;

    // Pitch section
    pitchLabel_.setBounds(40, 130, knobSize, 20);
    pitchSlider_.setBounds(40, 155, knobSize, knobSize);

    pitchEnvAmountLabel_.setBounds(140, 130, knobSize, 20);
    pitchEnvAmountSlider_.setBounds(140, 155, knobSize, knobSize);

    pitchEnvDecayLabel_.setBounds(240, 130, knobSize, 20);
    pitchEnvDecaySlider_.setBounds(240, 155, knobSize, knobSize);

    decayLabel_.setBounds(40, 260, knobSize, 20);
    decaySlider_.setBounds(40, 285, knobSize, knobSize);

    // Body section
    clickLabel_.setBounds(410, 130, knobSize, 20);
    clickSlider_.setBounds(410, 155, knobSize, knobSize);

    driveLabel_.setBounds(510, 130, knobSize, 20);
    driveSlider_.setBounds(510, 155, knobSize, knobSize);

    toneLabel_.setBounds(610, 130, knobSize, 20);
    toneSlider_.setBounds(610, 155, knobSize, knobSize);
}

void KickEditor::setupRotarySlider(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffff3333));
    slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffdd2222));
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffdddddd));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}
