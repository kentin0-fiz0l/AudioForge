#include "PluginEditor.h"

ClassicMonosynthEditor::ClassicMonosynthEditor(ClassicMonosynthProcessor& p)
    : AudioProcessorEditor(&p), processor_(p)
{
    setSize(1000, 600);

    // Helper lambda for setup
    auto setupControl = [this](juce::Label& label, const char* text, juce::Slider& slider,
                               auto& attachment, const char* paramId, bool rotary = true) {
        label.setText(text, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, juce::Colour(0xffdddddd));
        addAndMakeVisible(label);

        if (rotary)
            setupRotarySlider(slider);
        else
            setupLinearSlider(slider);

        addAndMakeVisible(slider);
        attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor_.getValueTreeState(), paramId, slider);
    };

    // Oscillators
    setupControl(osc1LevelLabel_, "Saw Level", osc1LevelSlider_, osc1LevelAttachment_, "osc1Level");
    setupControl(osc2LevelLabel_, "Sq Level", osc2LevelSlider_, osc2LevelAttachment_, "osc2Level");
    setupControl(osc3LevelLabel_, "Tri Level", osc3LevelSlider_, osc3LevelAttachment_, "osc3Level");
    setupControl(osc2DetuneLabel_, "Sq Detune", osc2DetuneSlider_, osc2DetuneAttachment_, "osc2Detune");
    setupControl(osc3DetuneLabel_, "Tri Detune", osc3DetuneSlider_, osc3DetuneAttachment_, "osc3Detune");
    setupControl(pulseWidthLabel_, "PW", pulseWidthSlider_, pulseWidthAttachment_, "pulseWidth");

    oscSyncButton_.setButtonText("OSC SYNC");
    oscSyncButton_.setColour(juce::ToggleButton::textColourId, juce::Colour(0xffdddddd));
    addAndMakeVisible(oscSyncButton_);
    oscSyncAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor_.getValueTreeState(), "oscSync", oscSyncButton_);

    // Filter
    setupControl(filterCutoffLabel_, "Cutoff", filterCutoffSlider_, filterCutoffAttachment_, "filterCutoff");
    setupControl(filterResonanceLabel_, "Resonance", filterResonanceSlider_, filterResonanceAttachment_, "filterResonance");
    setupControl(filterEnvAmountLabel_, "Env Amount", filterEnvAmountSlider_, filterEnvAmountAttachment_, "filterEnvAmount");

    // Filter Envelope
    setupControl(filterAttackLabel_, "A", filterAttackSlider_, filterAttackAttachment_, "filterAttack");
    setupControl(filterDecayLabel_, "D", filterDecaySlider_, filterDecayAttachment_, "filterDecay");
    setupControl(filterSustainLabel_, "S", filterSustainSlider_, filterSustainAttachment_, "filterSustain");
    setupControl(filterReleaseLabel_, "R", filterReleaseSlider_, filterReleaseAttachment_, "filterRelease");

    // Amp Envelope
    setupControl(ampAttackLabel_, "A", ampAttackSlider_, ampAttackAttachment_, "ampAttack");
    setupControl(ampDecayLabel_, "D", ampDecaySlider_, ampDecayAttachment_, "ampDecay");
    setupControl(ampSustainLabel_, "S", ampSustainSlider_, ampSustainAttachment_, "ampSustain");
    setupControl(ampReleaseLabel_, "R", ampReleaseSlider_, ampReleaseAttachment_, "ampRelease");

    // LFOs
    setupControl(lfo1RateLabel_, "Rate", lfo1RateSlider_, lfo1RateAttachment_, "lfo1Rate");
    setupControl(lfo1AmountLabel_, "Amount", lfo1AmountSlider_, lfo1AmountAttachment_, "lfo1Amount");
    setupControl(lfo2RateLabel_, "Rate", lfo2RateSlider_, lfo2RateAttachment_, "lfo2Rate");
    setupControl(lfo2AmountLabel_, "Amount", lfo2AmountSlider_, lfo2AmountAttachment_, "lfo2Amount");

    // Glide
    setupControl(glideTimeLabel_, "Glide", glideTimeSlider_, glideTimeAttachment_, "glideTime");
}

ClassicMonosynthEditor::~ClassicMonosynthEditor() {}

void ClassicMonosynthEditor::paint(juce::Graphics& g)
{
    // Vintage analog panel aesthetic
    g.fillAll(juce::Colour(0xff2a2a2a));

    // Title
    g.setColour(juce::Colour(0xffff8800)); // Orange
    g.setFont(juce::Font(36.0f, juce::Font::bold));
    g.drawText("CLASSIC MONOSYNTH", getLocalBounds().removeFromTop(70), juce::Justification::centred);

    // Section backgrounds and labels
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff333333));
    g.fillRect(20, 80, 300, 220);  // OSC section
    g.fillRect(340, 80, 300, 220); // FILTER section
    g.fillRect(660, 80, 320, 220); // ENV section
    g.fillRect(20, 320, 480, 200); // LFO section
    g.fillRect(520, 320, 200, 200); // GLIDE section

    g.setColour(juce::Colour(0xffff8800));
    g.drawText("OSCILLATORS", 30, 85, 280, 25, juce::Justification::left);
    g.drawText("FILTER", 350, 85, 280, 25, juce::Justification::left);
    g.drawText("ENVELOPES", 670, 85, 300, 25, juce::Justification::left);
    g.drawText("LFO 1 (Pitch)", 30, 325, 200, 25, juce::Justification::left);
    g.drawText("LFO 2 (PW)", 270, 325, 200, 25, juce::Justification::left);
    g.drawText("PORTAMENTO", 530, 325, 180, 25, juce::Justification::left);

    // Sub-labels for envelopes
    g.setFont(juce::Font(14.0f, juce::Font::bold));
    g.drawText("Filter Env", 670, 115, 140, 20, juce::Justification::left);
    g.drawText("Amp Env", 830, 115, 140, 20, juce::Justification::left);
}

void ClassicMonosynthEditor::resized()
{
    int knobSize = 65;

    // Oscillators
    osc1LevelLabel_.setBounds(40, 120, knobSize, 20);
    osc1LevelSlider_.setBounds(40, 145, knobSize, knobSize);

    osc2LevelLabel_.setBounds(120, 120, knobSize, 20);
    osc2LevelSlider_.setBounds(120, 145, knobSize, knobSize);

    osc3LevelLabel_.setBounds(200, 120, knobSize, 20);
    osc3LevelSlider_.setBounds(200, 145, knobSize, knobSize);

    osc2DetuneLabel_.setBounds(40, 220, knobSize, 20);
    osc2DetuneSlider_.setBounds(40, 245, knobSize, knobSize);

    osc3DetuneLabel_.setBounds(120, 220, knobSize, 20);
    osc3DetuneSlider_.setBounds(120, 245, knobSize, knobSize);

    pulseWidthLabel_.setBounds(200, 220, knobSize, 20);
    pulseWidthSlider_.setBounds(200, 245, knobSize, knobSize);

    oscSyncButton_.setBounds(280, 260, 30, 30);

    // Filter
    filterCutoffLabel_.setBounds(360, 120, knobSize, 20);
    filterCutoffSlider_.setBounds(360, 145, knobSize, knobSize);

    filterResonanceLabel_.setBounds(440, 120, knobSize, 20);
    filterResonanceSlider_.setBounds(440, 145, knobSize, knobSize);

    filterEnvAmountLabel_.setBounds(520, 120, knobSize, 20);
    filterEnvAmountSlider_.setBounds(520, 145, knobSize, knobSize);

    // Filter Envelope
    filterAttackLabel_.setBounds(680, 145, knobSize, 20);
    filterAttackSlider_.setBounds(680, 170, knobSize, knobSize);

    filterDecayLabel_.setBounds(760, 145, knobSize, 20);
    filterDecaySlider_.setBounds(760, 170, knobSize, knobSize);

    filterSustainLabel_.setBounds(680, 245, knobSize, 20);
    filterSustainSlider_.setBounds(680, 270, knobSize, knobSize);

    filterReleaseLabel_.setBounds(760, 245, knobSize, 20);
    filterReleaseSlider_.setBounds(760, 270, knobSize, knobSize);

    // Amp Envelope
    ampAttackLabel_.setBounds(840, 145, knobSize, 20);
    ampAttackSlider_.setBounds(840, 170, knobSize, knobSize);

    ampDecayLabel_.setBounds(920, 145, knobSize, 20);
    ampDecaySlider_.setBounds(920, 170, knobSize, knobSize);

    ampSustainLabel_.setBounds(840, 245, knobSize, 20);
    ampSustainSlider_.setBounds(840, 270, knobSize, knobSize);

    ampReleaseLabel_.setBounds(920, 245, knobSize, 20);
    ampReleaseSlider_.setBounds(920, 270, knobSize, knobSize);

    // LFO 1
    lfo1RateLabel_.setBounds(40, 360, knobSize, 20);
    lfo1RateSlider_.setBounds(40, 385, knobSize, knobSize);

    lfo1AmountLabel_.setBounds(120, 360, knobSize, 20);
    lfo1AmountSlider_.setBounds(120, 385, knobSize, knobSize);

    // LFO 2
    lfo2RateLabel_.setBounds(280, 360, knobSize, 20);
    lfo2RateSlider_.setBounds(280, 385, knobSize, knobSize);

    lfo2AmountLabel_.setBounds(360, 360, knobSize, 20);
    lfo2AmountSlider_.setBounds(360, 385, knobSize, knobSize);

    // Glide
    glideTimeLabel_.setBounds(540, 360, 160, 20);
    glideTimeSlider_.setBounds(540, 385, 160, knobSize);
}

void ClassicMonosynthEditor::setupRotarySlider(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 65, 20);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffff8800)); // Orange
    slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffffaa00));
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffdddddd));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}

void ClassicMonosynthEditor::setupLinearSlider(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::LinearVertical);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 65, 20);
    slider.setColour(juce::Slider::trackColourId, juce::Colour(0xff555555));
    slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffff8800));
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffdddddd));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}
