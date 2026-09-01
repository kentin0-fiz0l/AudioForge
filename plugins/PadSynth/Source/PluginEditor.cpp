#include "PluginProcessor.h"
#include "PluginEditor.h"

PluginEditor::PluginEditor(PluginProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Setup all sliders
    setupSlider(waveformSlider, waveformLabel, "Waveform", waveformAttachment, "waveform");
    setupSlider(unisonVoicesSlider, unisonVoicesLabel, "Voices", unisonVoicesAttachment, "unisonVoices");
    setupSlider(detuneSlider, detuneLabel, "Detune", detuneAttachment, "detuneAmount");
    setupSlider(stereoWidthSlider, stereoWidthLabel, "Width", stereoWidthAttachment, "stereoWidth");

    setupSlider(filterCutoffSlider, filterCutoffLabel, "Cutoff", filterCutoffAttachment, "filterCutoff");
    setupSlider(filterResSlider, filterResLabel, "Resonance", filterResAttachment, "filterRes");
    setupSlider(filterTypeSlider, filterTypeLabel, "Type", filterTypeAttachment, "filterType");

    setupSlider(attackSlider, attackLabel, "Attack", attackAttachment, "attack");
    setupSlider(decaySlider, decayLabel, "Decay", decayAttachment, "decay");
    setupSlider(sustainSlider, sustainLabel, "Sustain", sustainAttachment, "sustain");
    setupSlider(releaseSlider, releaseLabel, "Release", releaseAttachment, "release");

    // Customize waveform slider to show labels
    waveformSlider.textFromValueFunction = [](double value) {
        int type = static_cast<int>(value);
        switch (type) {
            case 0: return juce::String("Sine");
            case 1: return juce::String("Saw");
            case 2: return juce::String("Square");
            case 3: return juce::String("Triangle");
            default: return juce::String(type);
        }
    };

    // Customize filter type slider to show labels
    filterTypeSlider.textFromValueFunction = [](double value) {
        int type = static_cast<int>(value);
        switch (type) {
            case 0: return juce::String("LP");
            case 1: return juce::String("HP");
            case 2: return juce::String("BP");
            case 3: return juce::String("Notch");
            default: return juce::String(type);
        }
    };

    setSize(850, 500);
}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::setupSlider(juce::Slider& slider, juce::Label& label,
                               const juce::String& labelText,
                               std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment,
                               const juce::String& parameterID)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(slider);

    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.attachToComponent(&slider, false);
    addAndMakeVisible(label);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), parameterID, slider);
}

void PluginEditor::paint(juce::Graphics& g)
{
    // Dark background
    g.fillAll(juce::Colour(0xff1a1a1a));

    // Title
    g.setColour(juce::Colours::white);
    g.setFont(28.0f);
    g.drawFittedText("Pad Synth", 10, 10, getWidth() - 20, 40, juce::Justification::centred, 1);

    // Section backgrounds
    auto oscArea = juce::Rectangle<int>(20, 70, 420, 180);
    auto filterArea = juce::Rectangle<int>(460, 70, 370, 180);
    auto envArea = juce::Rectangle<int>(20, 270, 810, 210);

    g.setColour(juce::Colour(0xff2a2a2a));
    g.fillRoundedRectangle(oscArea.toFloat(), 8.0f);
    g.fillRoundedRectangle(filterArea.toFloat(), 8.0f);
    g.fillRoundedRectangle(envArea.toFloat(), 8.0f);

    // Section labels
    g.setColour(juce::Colours::lightblue);
    g.setFont(18.0f);
    g.drawText("Oscillator & Unison", oscArea.getX(), oscArea.getY() + 5, oscArea.getWidth(), 25, juce::Justification::centred);
    g.drawText("Filter", filterArea.getX(), filterArea.getY() + 5, filterArea.getWidth(), 25, juce::Justification::centred);
    g.drawText("Envelope", envArea.getX(), envArea.getY() + 5, envArea.getWidth(), 25, juce::Justification::centred);
}

void PluginEditor::resized()
{
    // Oscillator section (4 controls)
    waveformSlider.setBounds(40, 120, 90, 110);
    unisonVoicesSlider.setBounds(145, 120, 90, 110);
    detuneSlider.setBounds(250, 120, 90, 110);
    stereoWidthSlider.setBounds(355, 120, 90, 110);

    // Filter section (3 controls)
    filterCutoffSlider.setBounds(480, 120, 100, 110);
    filterResSlider.setBounds(595, 120, 100, 110);
    filterTypeSlider.setBounds(710, 120, 100, 110);

    // Envelope section (4 controls)
    attackSlider.setBounds(65, 320, 95, 115);
    decaySlider.setBounds(230, 320, 95, 115);
    sustainSlider.setBounds(395, 320, 95, 115);
    releaseSlider.setBounds(560, 320, 95, 115);
}
