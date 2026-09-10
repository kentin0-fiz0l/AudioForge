#include "PluginEditor.h"

MIDIHarmonizerEditor::MIDIHarmonizerEditor(MIDIHarmonizerProcessor& p)
    : AudioProcessorEditor(&p), processor_(p) {

    setSize(550, 400);

    // Key combo
    keyLabel_.setText("Key", juce::dontSendNotification);
    keyLabel_.attachToComponent(&keyCombo_, true);
    keyCombo_.addItemList(juce::StringArray{"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"}, 1);
    addAndMakeVisible(keyCombo_);
    addAndMakeVisible(keyLabel_);

    // Scale combo
    scaleLabel_.setText("Scale", juce::dontSendNotification);
    scaleLabel_.attachToComponent(&scaleCombo_, true);
    scaleCombo_.addItemList(juce::StringArray{
        "Chromatic", "Major", "Minor", "Dorian", "Phrygian", "Lydian",
        "Mixolydian", "Aeolian", "Locrian", "Harmonic Minor", "Melodic Minor",
        "Pentatonic", "Blues"
    }, 1);
    addAndMakeVisible(scaleCombo_);
    addAndMakeVisible(scaleLabel_);

    // Harmony toggles
    addAndMakeVisible(thirdAboveButton_);
    addAndMakeVisible(fifthAboveButton_);
    addAndMakeVisible(octaveAboveButton_);
    addAndMakeVisible(thirdBelowButton_);
    addAndMakeVisible(fifthBelowButton_);
    addAndMakeVisible(octaveBelowButton_);

    // Mix slider
    mixLabel_.setText("Mix", juce::dontSendNotification);
    mixLabel_.attachToComponent(&mixSlider_, true);
    mixSlider_.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    mixSlider_.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    mixSlider_.setRange(0.0, 1.0, 0.01);
    mixSlider_.setMidiLearnManager(&processor_.getMidiLearnManager());
    mixSlider_.setParameterID("mix");
    addAndMakeVisible(mixSlider_);
    addAndMakeVisible(mixLabel_);

    // Fixed velocity slider
    fixedVelLabel_.setText("Fixed Vel", juce::dontSendNotification);
    fixedVelLabel_.attachToComponent(&fixedVelSlider_, true);
    fixedVelSlider_.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    fixedVelSlider_.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    fixedVelSlider_.setRange(1, 127, 1);
    fixedVelSlider_.setMidiLearnManager(&processor_.getMidiLearnManager());
    fixedVelSlider_.setParameterID("fixedVel");
    addAndMakeVisible(fixedVelSlider_);
    addAndMakeVisible(fixedVelLabel_);

    // Velocity toggle
    addAndMakeVisible(useInputVelButton_);

    // Create attachments
    keyAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor_.getAPVTS(), "key", keyCombo_);
    scaleAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor_.getAPVTS(), "scale", scaleCombo_);
    thirdAboveAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor_.getAPVTS(), "thirdAbove", thirdAboveButton_);
    fifthAboveAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor_.getAPVTS(), "fifthAbove", fifthAboveButton_);
    octaveAboveAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor_.getAPVTS(), "octaveAbove", octaveAboveButton_);
    thirdBelowAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor_.getAPVTS(), "thirdBelow", thirdBelowButton_);
    fifthBelowAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor_.getAPVTS(), "fifthBelow", fifthBelowButton_);
    octaveBelowAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor_.getAPVTS(), "octaveBelow", octaveBelowButton_);
    mixAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getAPVTS(), "mix", mixSlider_);
    fixedVelAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getAPVTS(), "fixedVel", fixedVelSlider_);
    useInputVelAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor_.getAPVTS(), "useInputVel", useInputVelButton_);
}

MIDIHarmonizerEditor::~MIDIHarmonizerEditor() {}

void MIDIHarmonizerEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff1e1e1e));

    g.setColour(juce::Colour(0xff00d4ff));
    g.setFont(28.0f);
    g.drawText("MIDI Harmonizer", getLocalBounds().removeFromTop(50), juce::Justification::centred);

    g.setColour(juce::Colour(0xff404040));
    g.drawRect(getLocalBounds().reduced(10), 2);
}

void MIDIHarmonizerEditor::resized() {
    auto bounds = getLocalBounds().reduced(20);
    bounds.removeFromTop(50); // Title space

    // Top row: Key and Scale
    auto topRow = bounds.removeFromTop(60);
    keyCombo_.setBounds(topRow.removeFromLeft(150).reduced(5));
    topRow.removeFromLeft(10);
    scaleCombo_.setBounds(topRow.removeFromLeft(300).reduced(5));

    bounds.removeFromTop(10);

    // Harmony toggles section
    auto toggleSection = bounds.removeFromTop(100);

    auto aboveRow = toggleSection.removeFromTop(30);
    thirdAboveButton_.setBounds(aboveRow.removeFromLeft(150).reduced(2));
    fifthAboveButton_.setBounds(aboveRow.removeFromLeft(150).reduced(2));
    octaveAboveButton_.setBounds(aboveRow.removeFromLeft(180).reduced(2));

    toggleSection.removeFromTop(10);
    auto belowRow = toggleSection.removeFromTop(30);
    thirdBelowButton_.setBounds(belowRow.removeFromLeft(150).reduced(2));
    fifthBelowButton_.setBounds(belowRow.removeFromLeft(150).reduced(2));
    octaveBelowButton_.setBounds(belowRow.removeFromLeft(180).reduced(2));

    bounds.removeFromTop(10);

    // Knobs row
    auto knobsRow = bounds.removeFromTop(120);
    int knobWidth = knobsRow.getWidth() / 2;
    mixSlider_.setBounds(knobsRow.removeFromLeft(knobWidth).reduced(10));
    fixedVelSlider_.setBounds(knobsRow.removeFromLeft(knobWidth).reduced(10));

    bounds.removeFromTop(10);

    // Use input velocity toggle
    useInputVelButton_.setBounds(bounds.removeFromTop(40).reduced(5));
}
