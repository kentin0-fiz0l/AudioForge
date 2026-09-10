#include "PluginEditor.h"

MIDIArpeggiatorEditor::MIDIArpeggiatorEditor(MIDIArpeggiatorProcessor& p)
    : AudioProcessorEditor(&p), processor_(p) {

    setSize(500, 400);

    // Pattern combo
    patternLabel_.setText("Pattern", juce::dontSendNotification);
    patternLabel_.attachToComponent(&patternCombo_, true);
    patternCombo_.addItemList(juce::StringArray{"Up", "Down", "Up-Down", "Down-Up", "Random", "Played Order"}, 1);
    addAndMakeVisible(patternCombo_);
    addAndMakeVisible(patternLabel_);

    // Rate combo
    rateLabel_.setText("Rate", juce::dontSendNotification);
    rateLabel_.attachToComponent(&rateCombo_, true);
    rateCombo_.addItemList(juce::StringArray{"1/1", "1/2", "1/4", "1/8", "1/16", "1/32", "1/4T", "1/8T", "1/16T"}, 1);
    addAndMakeVisible(rateCombo_);
    addAndMakeVisible(rateLabel_);

    // Octaves slider
    octavesLabel_.setText("Octaves", juce::dontSendNotification);
    octavesLabel_.attachToComponent(&octavesSlider_, true);
    octavesSlider_.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    octavesSlider_.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    octavesSlider_.setRange(1, 4, 1);
    octavesSlider_.setMidiLearnManager(&processor_.getMidiLearnManager());
    octavesSlider_.setParameterID("octaves");
    addAndMakeVisible(octavesSlider_);
    addAndMakeVisible(octavesLabel_);

    // Gate slider
    gateLabel_.setText("Gate", juce::dontSendNotification);
    gateLabel_.attachToComponent(&gateSlider_, true);
    gateSlider_.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gateSlider_.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    gateSlider_.setRange(0.0, 1.0, 0.01);
    gateSlider_.setMidiLearnManager(&processor_.getMidiLearnManager());
    gateSlider_.setParameterID("gate");
    addAndMakeVisible(gateSlider_);
    addAndMakeVisible(gateLabel_);

    // Swing slider
    swingLabel_.setText("Swing", juce::dontSendNotification);
    swingLabel_.attachToComponent(&swingSlider_, true);
    swingSlider_.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    swingSlider_.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    swingSlider_.setRange(0.0, 1.0, 0.01);
    swingSlider_.setMidiLearnManager(&processor_.getMidiLearnManager());
    swingSlider_.setParameterID("swing");
    addAndMakeVisible(swingSlider_);
    addAndMakeVisible(swingLabel_);

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

    // Toggle buttons
    addAndMakeVisible(latchButton_);
    addAndMakeVisible(useInputVelButton_);

    // Create attachments
    patternAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor_.getAPVTS(), "pattern", patternCombo_);
    rateAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor_.getAPVTS(), "rate", rateCombo_);
    octavesAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getAPVTS(), "octaves", octavesSlider_);
    gateAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getAPVTS(), "gate", gateSlider_);
    swingAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getAPVTS(), "swing", swingSlider_);
    fixedVelAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getAPVTS(), "fixedVel", fixedVelSlider_);
    latchAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor_.getAPVTS(), "latch", latchButton_);
    useInputVelAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor_.getAPVTS(), "useInputVel", useInputVelButton_);
}

MIDIArpeggiatorEditor::~MIDIArpeggiatorEditor() {}

void MIDIArpeggiatorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff1e1e1e));

    g.setColour(juce::Colour(0xff00d4ff));
    g.setFont(28.0f);
    g.drawText("MIDI Arpeggiator", getLocalBounds().removeFromTop(50), juce::Justification::centred);

    g.setColour(juce::Colour(0xff404040));
    g.drawRect(getLocalBounds().reduced(10), 2);
}

void MIDIArpeggiatorEditor::resized() {
    auto bounds = getLocalBounds().reduced(20);
    bounds.removeFromTop(50); // Title space

    auto topRow = bounds.removeFromTop(60);

    // Pattern and Rate combos
    patternCombo_.setBounds(topRow.removeFromLeft(200).reduced(5));
    topRow.removeFromLeft(20);
    rateCombo_.setBounds(topRow.removeFromLeft(180).reduced(5));

    bounds.removeFromTop(20);

    // Knobs row
    auto knobsRow = bounds.removeFromTop(120);
    int knobWidth = knobsRow.getWidth() / 4;

    octavesSlider_.setBounds(knobsRow.removeFromLeft(knobWidth).reduced(10));
    gateSlider_.setBounds(knobsRow.removeFromLeft(knobWidth).reduced(10));
    swingSlider_.setBounds(knobsRow.removeFromLeft(knobWidth).reduced(10));
    fixedVelSlider_.setBounds(knobsRow.removeFromLeft(knobWidth).reduced(10));

    bounds.removeFromTop(20);

    // Toggle buttons
    auto toggleRow = bounds.removeFromTop(40);
    latchButton_.setBounds(toggleRow.removeFromLeft(200).reduced(5));
    useInputVelButton_.setBounds(toggleRow.removeFromLeft(250).reduced(5));
}
