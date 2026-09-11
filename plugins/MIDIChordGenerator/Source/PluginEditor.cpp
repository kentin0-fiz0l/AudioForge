#include "PluginEditor.h"

MIDIChordGeneratorEditor::MIDIChordGeneratorEditor(MIDIChordGeneratorProcessor& p)
    : AudioProcessorEditor(&p), processor_(p),
      presetBrowser_(processor_.getPresetManager()) {

    setSize(500, 350);

    // Preset browser
    addAndMakeVisible(presetBrowser_);

    // Chord type combo
    chordTypeLabel_.setText("Chord Type", juce::dontSendNotification);
    chordTypeLabel_.attachToComponent(&chordTypeCombo_, true);
    chordTypeCombo_.addItemList(juce::StringArray{
        "Major", "Minor", "Dim", "Aug", "Sus2", "Sus4",
        "Maj7", "Min7", "Dom7", "MinMaj7", "Dim7", "m7b5",
        "Maj6", "Min6", "Add9", "Maj9", "Min9", "Dom9"
    }, 1);
    addAndMakeVisible(chordTypeCombo_);
    addAndMakeVisible(chordTypeLabel_);

    // Voicing combo
    voicingLabel_.setText("Voicing", juce::dontSendNotification);
    voicingLabel_.attachToComponent(&voicingCombo_, true);
    voicingCombo_.addItemList(juce::StringArray{
        "Root", "1st Inv", "2nd Inv", "3rd Inv", "Spread", "Drop2", "Drop3"
    }, 1);
    addAndMakeVisible(voicingCombo_);
    addAndMakeVisible(voicingLabel_);

    // Octave shift slider
    octaveShiftLabel_.setText("Octave", juce::dontSendNotification);
    octaveShiftLabel_.attachToComponent(&octaveShiftSlider_, true);
    octaveShiftSlider_.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    octaveShiftSlider_.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    octaveShiftSlider_.setRange(-2, 2, 1);
    octaveShiftSlider_.setMidiLearnManager(&processor_.getMidiLearnManager());
    octaveShiftSlider_.setParameterID("octaveShift");
    addAndMakeVisible(octaveShiftSlider_);
    addAndMakeVisible(octaveShiftLabel_);

    // Strum slider
    strumLabel_.setText("Strum", juce::dontSendNotification);
    strumLabel_.attachToComponent(&strumSlider_, true);
    strumSlider_.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    strumSlider_.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    strumSlider_.setRange(0.0, 100.0, 0.1);
    strumSlider_.setMidiLearnManager(&processor_.getMidiLearnManager());
    strumSlider_.setParameterID("strum");
    addAndMakeVisible(strumSlider_);
    addAndMakeVisible(strumLabel_);

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

    // Toggle button
    addAndMakeVisible(useInputVelButton_);

    // Create attachments
    chordTypeAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor_.getAPVTS(), "chordType", chordTypeCombo_);
    voicingAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor_.getAPVTS(), "voicing", voicingCombo_);
    octaveShiftAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getAPVTS(), "octaveShift", octaveShiftSlider_);
    strumAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getAPVTS(), "strum", strumSlider_);
    fixedVelAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getAPVTS(), "fixedVel", fixedVelSlider_);
    useInputVelAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor_.getAPVTS(), "useInputVel", useInputVelButton_);
}

MIDIChordGeneratorEditor::~MIDIChordGeneratorEditor() {}

void MIDIChordGeneratorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff1e1e1e));

    g.setColour(juce::Colour(0xff00d4ff));
    g.setFont(28.0f);
    g.drawText("MIDI Chord Generator", getLocalBounds().removeFromTop(50), juce::Justification::centred);

    g.setColour(juce::Colour(0xff404040));
    g.drawRect(getLocalBounds().reduced(10), 2);
}

void MIDIChordGeneratorEditor::resized() {
    auto bounds = getLocalBounds().reduced(20);
    bounds.removeFromTop(50); // Title space

    auto topRow = bounds.removeFromTop(60);

    // Chord type and voicing combos
    chordTypeCombo_.setBounds(topRow.removeFromLeft(250).reduced(5));
    topRow.removeFromLeft(10);
    voicingCombo_.setBounds(topRow.removeFromLeft(180).reduced(5));

    bounds.removeFromTop(20);

    // Knobs row
    auto knobsRow = bounds.removeFromTop(120);
    int knobWidth = knobsRow.getWidth() / 3;

    octaveShiftSlider_.setBounds(knobsRow.removeFromLeft(knobWidth).reduced(10));
    strumSlider_.setBounds(knobsRow.removeFromLeft(knobWidth).reduced(10));
    fixedVelSlider_.setBounds(knobsRow.removeFromLeft(knobWidth).reduced(10));

    bounds.removeFromTop(20);

    // Toggle button
    useInputVelButton_.setBounds(bounds.removeFromTop(40).reduced(5));
}
