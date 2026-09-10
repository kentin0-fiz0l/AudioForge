#include "PluginEditor.h"

SnareEditor::SnareEditor(SnareProcessor& p)
    : AudioProcessorEditor(&p), processor_(p)
{
    setSize(750, 400);

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

    setupControl(toneLabel_, "Tone", toneSlider_, toneAttachment_, "tone");
    setupControl(snappyLabel_, "Snappy", snappySlider_, snappyAttachment_, "snappy");
    setupControl(decayLabel_, "Decay", decaySlider_, decayAttachment_, "decay");
    setupControl(snareDecayLabel_, "Snare Decay", snareDecaySlider_, snareDecayAttachment_, "snareDecay");
    setupControl(pitchEnvLabel_, "Pitch Env", pitchEnvSlider_, pitchEnvAttachment_, "pitchEnv");
    setupControl(snapLabel_, "Snap", snapSlider_, snapAttachment_, "snap");
}

SnareEditor::~SnareEditor() {}

void SnareEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1a1a));
    g.setColour(juce::Colour(0xffffaa33)); // Gold
    g.setFont(juce::Font(42.0f, juce::Font::bold));
    g.drawText("SNARE SYNTH", getLocalBounds().removeFromTop(70), juce::Justification::centred);

    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff222222));
    g.fillRect(20, 90, 350, 280); // Body section
    g.fillRect(390, 90, 340, 280); // Snare section

    g.setColour(juce::Colour(0xffffaa33));
    g.drawText("BODY & TONE", 30, 95, 330, 25, juce::Justification::left);
    g.drawText("SNARE RATTLE", 400, 95, 320, 25, juce::Justification::left);
}

void SnareEditor::resized()
{
    int knobSize = 80;

    // Body section
    toneLabel_.setBounds(40, 130, knobSize, 20);
    toneSlider_.setBounds(40, 155, knobSize, knobSize);

    pitchEnvLabel_.setBounds(140, 130, knobSize, 20);
    pitchEnvSlider_.setBounds(140, 155, knobSize, knobSize);

    decayLabel_.setBounds(240, 130, knobSize, 20);
    decaySlider_.setBounds(240, 155, knobSize, knobSize);

    // Snare section
    snappyLabel_.setBounds(410, 130, knobSize, 20);
    snappySlider_.setBounds(410, 155, knobSize, knobSize);

    snareDecayLabel_.setBounds(510, 130, knobSize, 20);
    snareDecaySlider_.setBounds(510, 155, knobSize, knobSize);

    snapLabel_.setBounds(610, 130, knobSize, 20);
    snapSlider_.setBounds(610, 155, knobSize, knobSize);
}

void SnareEditor::setupRotarySlider(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffffaa33));
    slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffdd9922));
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffdddddd));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}
