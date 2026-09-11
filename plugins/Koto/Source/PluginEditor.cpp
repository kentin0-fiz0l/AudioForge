#include "PluginEditor.h"

KotoEditor::KotoEditor(KotoProcessor& p)
    : AudioProcessorEditor(&p), processor_(p)
{
    setSize(750, 500);

    // Preset browser
    addAndMakeVisible(presetBrowser_);

    // Scale selector
    scaleLabel_.setText("Scale", juce::dontSendNotification);
    scaleLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(scaleLabel_);

    scaleCombo_.addItem("In-sen (E F A B D)", 1);
    scaleCombo_.addItem("Hirajoshi (E F A B♭ D)", 2);
    scaleCombo_.addItem("Iwato (E F A♭ B♭ D♭)", 3);
    scaleCombo_.addItem("Kumoi (E F A B C)", 4);
    scaleCombo_.addItem("Chromatic", 5);
    addAndMakeVisible(scaleCombo_);
    scaleAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor_.getValueTreeState(), "scale", scaleCombo_);

    // Play style selector
    playStyleLabel_.setText("Play Style", juce::dontSendNotification);
    playStyleLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(playStyleLabel_);

    playStyleCombo_.addItem("Pluck", 1);
    playStyleCombo_.addItem("Tremolo", 2);
    playStyleCombo_.addItem("Scrape", 3);
    addAndMakeVisible(playStyleCombo_);
    playStyleAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor_.getValueTreeState(), "playStyle", playStyleCombo_);

    // Setup controls
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

    setupControl(bodyResonanceLabel_, "Body", bodyResonanceSlider_, bodyResonanceAttachment_, "bodyResonance");
    setupControl(attackLabel_, "Attack", attackSlider_, attackAttachment_, "attack");
    setupControl(decayLabel_, "Decay", decaySlider_, decayAttachment_, "decay");
    setupControl(toneLabel_, "Tone", toneSlider_, toneAttachment_, "tone");
    setupControl(tremoloRateLabel_, "Tremolo Rate", tremoloRateSlider_, tremoloRateAttachment_, "tremoloRate");
    setupControl(pitchBendLabel_, "Pitch Bend", pitchBendSlider_, pitchBendAttachment_, "pitchBend", false);
}

KotoEditor::~KotoEditor() {}

void KotoEditor::paint(juce::Graphics& g)
{
    // Japanese wood aesthetic - warm brown gradient
    juce::ColourGradient gradient(
        juce::Colour(0xff2a1810), 0, 0,
        juce::Colour(0xff4a3020), 0, static_cast<float>(getHeight()),
        false);
    g.setGradientFill(gradient);
    g.fillAll();

    // Title with Japanese characters
    g.setColour(juce::Colour(0xffd4a574)); // Paulownia wood color
    g.setFont(juce::Font(30.0f, juce::Font::bold));
    g.drawText("琴 Koto", getLocalBounds().removeFromTop(60), juce::Justification::centred);

    // Section headers
    g.setFont(juce::Font(15.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xffc4956d));
    g.drawText("TIMBRE", 30, 105, 200, 25, juce::Justification::left);
    g.drawText("ENVELOPE", 230, 105, 200, 25, juce::Justification::left);
    g.drawText("MODULATION", 450, 105, 200, 25, juce::Justification::left);

    // Decorative lines
    g.setColour(juce::Colour(0xff8b6f47));
    g.drawLine(30, 100, getWidth() - 30, 100, 2.0f);
    g.drawLine(30, 300, getWidth() - 30, 300, 2.0f);

    // Pitch bend section
    g.setColour(juce::Colour(0xffc4956d));
    g.drawText("PITCH CONTROL", 40, 310, 670, 25, juce::Justification::centred);
}

void KotoEditor::resized()
{
    scaleLabel_.setBounds(200, 40, 160, 20);
    scaleCombo_.setBounds(200, 65, 160, 25);

    playStyleLabel_.setBounds(390, 40, 160, 20);
    playStyleCombo_.setBounds(390, 65, 160, 25);

    int sliderSize = 75;
    int yPos = 135;

    // Timbre controls
    bodyResonanceLabel_.setBounds(50, yPos, sliderSize, 20);
    bodyResonanceSlider_.setBounds(50, yPos + 25, sliderSize, sliderSize);

    toneLabel_.setBounds(140, yPos, sliderSize, 20);
    toneSlider_.setBounds(140, yPos + 25, sliderSize, sliderSize);

    // Envelope controls
    attackLabel_.setBounds(250, yPos, sliderSize, 20);
    attackSlider_.setBounds(250, yPos + 25, sliderSize, sliderSize);

    decayLabel_.setBounds(340, yPos, sliderSize, 20);
    decaySlider_.setBounds(340, yPos + 25, sliderSize, sliderSize);

    // Modulation controls
    tremoloRateLabel_.setBounds(470, yPos, sliderSize, 20);
    tremoloRateSlider_.setBounds(470, yPos + 25, sliderSize, sliderSize);

    // Pitch bend slider (linear, horizontal)
    pitchBendLabel_.setBounds(40, 345, 120, 20);
    pitchBendSlider_.setBounds(40, 370, 670, 40);
}

void KotoEditor::setupRotarySlider(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 75, 20);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffd4a574)); // Paulownia
    slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffc4956d));
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffeeddcc));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}

void KotoEditor::setupLinearSlider(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::LinearHorizontal);
    slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    slider.setColour(juce::Slider::trackColourId, juce::Colour(0xff4a3020));
    slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffd4a574));
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffeeddcc));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}
