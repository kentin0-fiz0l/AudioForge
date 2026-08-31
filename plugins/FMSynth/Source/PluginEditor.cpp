#include "PluginProcessor.h"
#include "PluginEditor.h"

PluginEditor::PluginEditor(PluginProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(800, 500);

    // Setup modulator controls
    setupSlider(modRatioSlider, modRatioLabel, "Mod Ratio", modRatioAttachment, "modRatio");
    setupSlider(modDepthSlider, modDepthLabel, "Mod Depth", modDepthAttachment, "modDepth");

    // Setup carrier ADSR
    setupSlider(carrierAttackSlider, carrierAttackLabel, "Attack", carrierAttackAttachment, "carrierAttack");
    setupSlider(carrierDecaySlider, carrierDecayLabel, "Decay", carrierDecayAttachment, "carrierDecay");
    setupSlider(carrierSustainSlider, carrierSustainLabel, "Sustain", carrierSustainAttachment, "carrierSustain");
    setupSlider(carrierReleaseSlider, carrierReleaseLabel, "Release", carrierReleaseAttachment, "carrierRelease");

    // Setup modulator ADSR
    setupSlider(modAttackSlider, modAttackLabel, "Attack", modAttackAttachment, "modAttack");
    setupSlider(modDecaySlider, modDecayLabel, "Decay", modDecayAttachment, "modDecay");
    setupSlider(modSustainSlider, modSustainLabel, "Sustain", modSustainAttachment, "modSustain");
    setupSlider(modReleaseSlider, modReleaseLabel, "Release", modReleaseAttachment, "modRelease");
}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText,
                               std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment,
                               const juce::String& parameterID)
{
    slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
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
    // Background
    g.fillAll(juce::Colour(0xff1a1a1a));

    // Title
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(28.0f, juce::Font::bold));
    g.drawFittedText("FM Synth", 0, 15, getWidth(), 30, juce::Justification::centred, 1);

    // Section headers
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff4a9eff));

    g.drawText("Modulation", 20, 60, 200, 20, juce::Justification::left);
    g.drawText("Carrier Envelope", 20, 220, 200, 20, juce::Justification::left);
    g.drawText("Modulator Envelope", 420, 220, 200, 20, juce::Justification::left);

    // Section dividers
    g.setColour(juce::Colour(0xff333333));
    g.drawLine(20, 210, getWidth() - 20, 210, 2.0f);
}

void PluginEditor::resized()
{
    auto bounds = getLocalBounds();

    // Modulator section (top)
    int yPos = 90;
    int xPos = 40;
    int knobSize = 100;
    int spacing = 120;

    modRatioSlider.setBounds(xPos, yPos, knobSize, knobSize);
    modDepthSlider.setBounds(xPos + spacing, yPos, knobSize, knobSize);

    // Carrier ADSR section (bottom left)
    yPos = 250;
    xPos = 40;

    carrierAttackSlider.setBounds(xPos, yPos, knobSize, knobSize);
    carrierDecaySlider.setBounds(xPos + spacing, yPos, knobSize, knobSize);
    carrierSustainSlider.setBounds(xPos + spacing * 2, yPos, knobSize, knobSize);
    carrierReleaseSlider.setBounds(xPos + spacing * 3, yPos, knobSize, knobSize);

    // Modulator ADSR section (bottom right)
    xPos = 440;

    modAttackSlider.setBounds(xPos, yPos, knobSize, knobSize);
    modDecaySlider.setBounds(xPos + spacing, yPos, knobSize, knobSize);
    modSustainSlider.setBounds(xPos + spacing * 2, yPos, knobSize, knobSize);
}
