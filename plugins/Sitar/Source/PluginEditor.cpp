#include "PluginEditor.h"

SitarEditor::SitarEditor(SitarProcessor& p)
    : AudioProcessorEditor(&p), processor_(p)
{
    setSize(800, 550);

    // Raga scale selector
    ragaLabel_.setText("Raga Scale", juce::dontSendNotification);
    ragaLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(ragaLabel_);

    ragaCombo_.addItem("Chromatic (All 12)", 1);
    ragaCombo_.addItem("Bhairavi", 2);
    ragaCombo_.addItem("Yaman/Kalyan", 3);
    ragaCombo_.addItem("Kafi", 4);
    addAndMakeVisible(ragaCombo_);
    ragaAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor_.getValueTreeState(), "ragaScale", ragaCombo_);

    // Setup controls with Indian-inspired naming
    auto setupControl = [this](juce::Label& label, const char* text, juce::Slider& slider,
                               auto& attachment, const char* paramId, bool rotary = true) {
        label.setText(text, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);

        if (rotary)
            setupRotarySlider(slider);
        else
            setupLinearSlider(slider);

        addAndMakeVisible(slider);
        attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor_.getValueTreeState(), paramId, slider);
    };

    setupControl(jawariLabel_, "Jawari (Buzz)", jawariSlider_, jawariAttachment_, "jawari");
    setupControl(sympatheticLabel_, "Tarab (Sympath.)", sympatheticSlider_, sympatheticAttachment_, "sympathetic");
    setupControl(attackLabel_, "Attack", attackSlider_, attackAttachment_, "attack");
    setupControl(decayLabel_, "Decay", decaySlider_, decayAttachment_, "decay");
    setupControl(pitchBendLabel_, "Meend (Bend)", pitchBendSlider_, pitchBendAttachment_, "pitchBend", false);
    setupControl(vibratoRateLabel_, "Vib. Rate", vibratoRateSlider_, vibratoRateAttachment_, "vibratoRate");
    setupControl(vibratoDepthLabel_, "Vib. Depth", vibratoDepthSlider_, vibratoDepthAttachment_, "vibratoDepth");
    setupControl(droneLevelLabel_, "Tanpura (Drone)", droneLevelSlider_, droneLevelAttachment_, "droneLevel");
}

SitarEditor::~SitarEditor() {}

void SitarEditor::paint(juce::Graphics& g)
{
    // Gradient background with Indian saffron/gold theme
    juce::ColourGradient gradient(
        juce::Colour(0xff1a0f0a), 0, 0,
        juce::Colour(0xff3d2816), 0, static_cast<float>(getHeight()),
        false);
    g.setGradientFill(gradient);
    g.fillAll();

    // Title with saffron accent
    g.setColour(juce::Colour(0xffff9933)); // Saffron
    g.setFont(juce::Font(32.0f, juce::Font::bold));
    g.drawText("सितार Sitar", getLocalBounds().removeFromTop(70), juce::Justification::centred);

    // Section headers
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xffffcc66)); // Gold
    g.drawText("TIMBRE", 20, 115, 200, 25, juce::Justification::left);
    g.drawText("ENVELOPE", 220, 115, 200, 25, juce::Justification::left);
    g.drawText("MODULATION", 420, 115, 200, 25, juce::Justification::left);
    g.drawText("DRONE", 620, 115, 150, 25, juce::Justification::left);

    // Decorative lines
    g.setColour(juce::Colour(0xffaa6600));
    g.drawLine(20, 110, getWidth() - 20, 110, 2.0f);
    g.drawLine(20, 320, getWidth() - 20, 320, 2.0f);

    // Pitch bend section
    g.setColour(juce::Colour(0xffffcc66));
    g.drawText("MEEND (PITCH CONTROL)", 40, 330, 720, 25, juce::Justification::centred);
}

void SitarEditor::resized()
{
    ragaLabel_.setBounds(300, 45, 200, 20);
    ragaCombo_.setBounds(300, 70, 200, 25);

    int sliderSize = 80;
    int yPos = 150;

    // Timbre controls
    jawariLabel_.setBounds(40, yPos, sliderSize, 20);
    jawariSlider_.setBounds(40, yPos + 25, sliderSize, sliderSize);

    sympatheticLabel_.setBounds(140, yPos, sliderSize, 20);
    sympatheticSlider_.setBounds(140, yPos + 25, sliderSize, sliderSize);

    // Envelope controls
    attackLabel_.setBounds(240, yPos, sliderSize, 20);
    attackSlider_.setBounds(240, yPos + 25, sliderSize, sliderSize);

    decayLabel_.setBounds(340, yPos, sliderSize, 20);
    decaySlider_.setBounds(340, yPos + 25, sliderSize, sliderSize);

    // Modulation controls
    vibratoRateLabel_.setBounds(440, yPos, sliderSize, 20);
    vibratoRateSlider_.setBounds(440, yPos + 25, sliderSize, sliderSize);

    vibratoDepthLabel_.setBounds(540, yPos, sliderSize, 20);
    vibratoDepthSlider_.setBounds(540, yPos + 25, sliderSize, sliderSize);

    // Drone control
    droneLevelLabel_.setBounds(640, yPos, sliderSize, 20);
    droneLevelSlider_.setBounds(640, yPos + 25, sliderSize, sliderSize);

    // Pitch bend slider (linear, horizontal)
    pitchBendLabel_.setBounds(40, 365, 120, 20);
    pitchBendSlider_.setBounds(40, 390, 720, 40);
}

void SitarEditor::setupRotarySlider(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffff9933)); // Saffron
    slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffffcc66)); // Gold
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffffeecc));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}

void SitarEditor::setupLinearSlider(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::LinearHorizontal);
    slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    slider.setColour(juce::Slider::trackColourId, juce::Colour(0xff663300));
    slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffff9933)); // Saffron
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffffeecc));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}
