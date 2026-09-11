#include "PluginEditor.h"

PolysynthEditor::PolysynthEditor(PolysynthProcessor& p)
    : AudioProcessorEditor(&p), processor_(p)
{
    setSize(900, 550);

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

    setupControl(oscMixLabel_, "Saw/Sq", oscMixSlider_, oscMixAttachment_, "oscMix");
    setupControl(detuneLabel_, "Detune", detuneSlider_, detuneAttachment_, "detune");
    setupControl(unisonVoicesLabel_, "Unison", unisonVoicesSlider_, unisonVoicesAttachment_, "unisonVoices");
    setupControl(unisonDetuneLabel_, "Spread", unisonDetuneSlider_, unisonDetuneAttachment_, "unisonDetune");
    setupControl(filterCutoffLabel_, "Cutoff", filterCutoffSlider_, filterCutoffAttachment_, "filterCutoff");
    setupControl(filterResonanceLabel_, "Resonance", filterResonanceSlider_, filterResonanceAttachment_, "filterResonance");
    setupControl(filterMixLabel_, "LP/HP", filterMixSlider_, filterMixAttachment_, "filterMix");
    setupControl(attackLabel_, "A", attackSlider_, attackAttachment_, "attack");
    setupControl(decayLabel_, "D", decaySlider_, decayAttachment_, "decay");
    setupControl(sustainLabel_, "S", sustainSlider_, sustainAttachment_, "sustain");
    setupControl(releaseLabel_, "R", releaseSlider_, releaseAttachment_, "release");
    setupControl(chorusDepthLabel_, "Depth", chorusDepthSlider_, chorusDepthAttachment_, "chorusDepth");
    setupControl(chorusRateLabel_, "Rate", chorusRateSlider_, chorusRateAttachment_, "chorusRate");
}

PolysynthEditor::~PolysynthEditor() {}

void PolysynthEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1a1a));
    g.setColour(juce::Colour(0xff00ccff)); // Cyan
    g.setFont(juce::Font(36.0f, juce::Font::bold));
    g.drawText("POLYSYNTH", getLocalBounds().removeFromTop(70), juce::Justification::centred);

    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff222222));
    g.fillRect(20, 80, 420, 200); // OSC/UNISON
    g.fillRect(460, 80, 420, 200); // FILTER
    g.fillRect(20, 300, 420, 220); // ENVELOPE
    g.fillRect(460, 300, 420, 220); // CHORUS

    g.setColour(juce::Colour(0xff00ccff));
    g.drawText("OSCILLATORS & UNISON", 30, 85, 400, 25, juce::Justification::left);
    g.drawText("FILTER", 470, 85, 400, 25, juce::Justification::left);
    g.drawText("ENVELOPE", 30, 305, 400, 25, juce::Justification::left);
    g.drawText("CHORUS", 470, 305, 400, 25, juce::Justification::left);
}

void PolysynthEditor::resized()
{
    int knobSize = 65;
    oscMixLabel_.setBounds(40, 120, knobSize, 20); oscMixSlider_.setBounds(40, 145, knobSize, knobSize);
    detuneLabel_.setBounds(120, 120, knobSize, 20); detuneSlider_.setBounds(120, 145, knobSize, knobSize);
    unisonVoicesLabel_.setBounds(200, 120, knobSize, 20); unisonVoicesSlider_.setBounds(200, 145, knobSize, knobSize);
    unisonDetuneLabel_.setBounds(280, 120, knobSize, 20); unisonDetuneSlider_.setBounds(280, 145, knobSize, knobSize);

    filterCutoffLabel_.setBounds(480, 120, knobSize, 20); filterCutoffSlider_.setBounds(480, 145, knobSize, knobSize);
    filterResonanceLabel_.setBounds(560, 120, knobSize, 20); filterResonanceSlider_.setBounds(560, 145, knobSize, knobSize);
    filterMixLabel_.setBounds(640, 120, knobSize, 20); filterMixSlider_.setBounds(640, 145, knobSize, knobSize);

    attackLabel_.setBounds(40, 340, knobSize, 20); attackSlider_.setBounds(40, 365, knobSize, knobSize);
    decayLabel_.setBounds(120, 340, knobSize, 20); decaySlider_.setBounds(120, 365, knobSize, knobSize);
    sustainLabel_.setBounds(200, 340, knobSize, 20); sustainSlider_.setBounds(200, 365, knobSize, knobSize);
    releaseLabel_.setBounds(280, 340, knobSize, 20); releaseSlider_.setBounds(280, 365, knobSize, knobSize);

    chorusDepthLabel_.setBounds(480, 340, knobSize, 20); chorusDepthSlider_.setBounds(480, 365, knobSize, knobSize);
    chorusRateLabel_.setBounds(560, 340, knobSize, 20); chorusRateSlider_.setBounds(560, 365, knobSize, knobSize);
}

void PolysynthEditor::setupRotarySlider(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 65, 20);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff00ccff)); // Cyan
    slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xff00aadd));
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffdddddd));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}
