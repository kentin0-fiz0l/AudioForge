#include "PluginProcessor.h"
#include "PluginEditor.h"

PluginEditor::PluginEditor(PluginProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Setup all sliders - Kick section
    setupSlider(kickPitchSlider, kickPitchLabel, "Pitch", kickPitchAttachment, "kickPitch");
    setupSlider(kickDecaySlider, kickDecayLabel, "Decay", kickDecayAttachment, "kickDecay");
    setupSlider(kickClickSlider, kickClickLabel, "Click", kickClickAttachment, "kickClick");
    setupSlider(kickToneSlider, kickToneLabel, "Tone", kickToneAttachment, "kickTone");
    setupSlider(kickDriveSlider, kickDriveLabel, "Drive", kickDriveAttachment, "kickDrive");

    // Snare section
    setupSlider(snareTuneSlider, snareTuneLabel, "Tune", snareTuneAttachment, "snareTune");
    setupSlider(snareSnapSlider, snareSnapLabel, "Snap", snareSnapAttachment, "snareSnap");
    setupSlider(snareToneSlider, snareToneLabel, "Tone", snareToneAttachment, "snareTone");
    setupSlider(snareDecaySlider, snareDecayLabel, "Decay", snareDecayAttachment, "snareDecay");
    setupSlider(snareMixSlider, snareMixLabel, "Mix", snareMixAttachment, "snareMix");

    // Hi-Hat section
    setupSlider(hihatTuneSlider, hihatTuneLabel, "Tune", hihatTuneAttachment, "hihatTune");
    setupSlider(hihatDecaySlider, hihatDecayLabel, "Decay", hihatDecayAttachment, "hihatDecay");
    setupSlider(hihatToneSlider, hihatToneLabel, "Tone", hihatToneAttachment, "hihatTone");
    setupSlider(hihatClickSlider, hihatClickLabel, "Click", hihatClickAttachment, "hihatClick");

    setSize(900, 450);
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
    g.drawFittedText("Drum Synth", 10, 10, getWidth() - 20, 40, juce::Justification::centred, 1);

    // Section backgrounds
    auto kickArea = juce::Rectangle<int>(20, 70, 440, 180);
    auto snareArea = juce::Rectangle<int>(480, 70, 400, 180);
    auto hihatArea = juce::Rectangle<int>(220, 270, 460, 160);

    g.setColour(juce::Colour(0xff2a2a2a));
    g.fillRoundedRectangle(kickArea.toFloat(), 8.0f);
    g.fillRoundedRectangle(snareArea.toFloat(), 8.0f);
    g.fillRoundedRectangle(hihatArea.toFloat(), 8.0f);

    // Section labels with MIDI note info
    g.setColour(juce::Colours::lightblue);
    g.setFont(18.0f);
    g.drawText("Kick (C1)", kickArea.getX(), kickArea.getY() + 5, kickArea.getWidth(), 25, juce::Justification::centred);
    g.drawText("Snare (D1)", snareArea.getX(), snareArea.getY() + 5, snareArea.getWidth(), 25, juce::Justification::centred);
    g.drawText("Hi-Hat (F#1)", hihatArea.getX(), hihatArea.getY() + 5, hihatArea.getWidth(), 25, juce::Justification::centred);
}

void PluginEditor::resized()
{
    // Kick section (5 controls)
    kickPitchSlider.setBounds(40, 120, 80, 110);
    kickDecaySlider.setBounds(135, 120, 80, 110);
    kickClickSlider.setBounds(230, 120, 80, 110);
    kickToneSlider.setBounds(325, 120, 80, 110);
    kickDriveSlider.setBounds(420, 120, 80, 110);

    // Snare section (5 controls)
    snareTuneSlider.setBounds(500, 120, 70, 110);
    snareSnapSlider.setBounds(585, 120, 70, 110);
    snareToneSlider.setBounds(670, 120, 70, 110);
    snareDecaySlider.setBounds(755, 120, 70, 110);
    snareMixSlider.setBounds(840, 120, 70, 110);

    // Hi-Hat section (4 controls)
    hihatTuneSlider.setBounds(260, 320, 90, 90);
    hihatDecaySlider.setBounds(370, 320, 90, 90);
    hihatToneSlider.setBounds(480, 320, 90, 90);
    hihatClickSlider.setBounds(590, 320, 90, 90);
}
