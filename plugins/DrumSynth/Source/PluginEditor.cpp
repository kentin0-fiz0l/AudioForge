#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "../../shared/ui/AudioForgeTheme.h"

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

    // Clap controls
    setupSlider(clapToneSlider, clapToneLabel, "Tone", clapToneAttachment, "clapTone");
    setupSlider(clapDecaySlider, clapDecayLabel, "Decay", clapDecayAttachment, "clapDecay");

    setSize(900, 450);

    // Preset browser
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
    using namespace AudioForge;

    // Background
    g.fillAll(Colors::Background);

    // Title bar (standardized)
    Layout::drawTitleBar(g, "AudioForge DrumSynth", Categories::Synthesizer, getWidth());

    // Section backgrounds
    auto kickArea = juce::Rectangle<int>(20, 70, 440, 180);
    auto snareArea = juce::Rectangle<int>(480, 70, 400, 180);
    auto hihatArea = juce::Rectangle<int>(20, 270, 580, 160);
    auto clapArea = juce::Rectangle<int>(620, 270, 260, 160);

    g.setColour(juce::Colour(0xff2a2a2a));
    g.fillRoundedRectangle(kickArea.toFloat(), 8.0f);
    g.fillRoundedRectangle(snareArea.toFloat(), 8.0f);
    g.fillRoundedRectangle(hihatArea.toFloat(), 8.0f);
    g.fillRoundedRectangle(clapArea.toFloat(), 8.0f);

    // Section labels with MIDI note info
    g.setColour(juce::Colours::lightblue);
    g.setFont(18.0f);
    g.drawText("Kick (C1)", kickArea.getX(), kickArea.getY() + 5, kickArea.getWidth(), 25, juce::Justification::centred);
    g.drawText("Snare (D1)", snareArea.getX(), snareArea.getY() + 5, snareArea.getWidth(), 25, juce::Justification::centred);
    g.drawText("Hi-Hat (F#1 closed, A#1 open)", hihatArea.getX(), hihatArea.getY() + 5, hihatArea.getWidth(), 25, juce::Justification::centred);
    g.drawText("Clap (D#1)", clapArea.getX(), clapArea.getY() + 5, clapArea.getWidth(), 25, juce::Justification::centred);
}

void PluginEditor::resized()
{
    // Kick section (5 controls)
    kickPitchSlider.setBounds(28, 120, 80, 110);
    kickDecaySlider.setBounds(114, 120, 80, 110);
    kickClickSlider.setBounds(200, 120, 80, 110);
    kickToneSlider.setBounds(286, 120, 80, 110);
    kickDriveSlider.setBounds(372, 120, 80, 110);

    // Snare section (5 controls)
    snareTuneSlider.setBounds(486, 120, 72, 110);
    snareSnapSlider.setBounds(564, 120, 72, 110);
    snareToneSlider.setBounds(642, 120, 72, 110);
    snareDecaySlider.setBounds(720, 120, 72, 110);
    snareMixSlider.setBounds(798, 120, 72, 110);

    // Hi-Hat section (4 controls)
    hihatTuneSlider.setBounds(95, 320, 90, 90);
    hihatDecaySlider.setBounds(210, 320, 90, 90);
    hihatToneSlider.setBounds(325, 320, 90, 90);
    hihatClickSlider.setBounds(440, 320, 90, 90);

    // Clap section, to its right
    clapToneSlider.setBounds(650, 320, 90, 90);
    clapDecaySlider.setBounds(760, 320, 90, 90);
}
