#include "PluginEditor.h"

MIDIGrooveQuantizerEditor::MIDIGrooveQuantizerEditor(MIDIGrooveQuantizerProcessor& p)
    : AudioProcessorEditor(&p), processor_(p) {

    setSize(500, 350);

    // Grid resolution combo
    gridLabel_.setText("Grid", juce::dontSendNotification);
    gridLabel_.attachToComponent(&gridCombo_, true);
    gridCombo_.addItemList(juce::StringArray{"1/4", "1/8", "1/16", "1/32", "1/8T", "1/16T"}, 1);
    addAndMakeVisible(gridCombo_);
    addAndMakeVisible(gridLabel_);

    // Strength slider
    strengthLabel_.setText("Strength", juce::dontSendNotification);
    strengthLabel_.attachToComponent(&strengthSlider_, true);
    strengthSlider_.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    strengthSlider_.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    strengthSlider_.setRange(0.0, 1.0, 0.01);
    strengthSlider_.setMidiLearnManager(&processor_.getMidiLearnManager());
    strengthSlider_.setParameterID("quantizeStrength");
    addAndMakeVisible(strengthSlider_);
    addAndMakeVisible(strengthLabel_);

    // Swing slider
    swingLabel_.setText("Swing", juce::dontSendNotification);
    swingLabel_.attachToComponent(&swingSlider_, true);
    swingSlider_.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    swingSlider_.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    swingSlider_.setRange(0.0, 1.0, 0.01);
    swingSlider_.setMidiLearnManager(&processor_.getMidiLearnManager());
    swingSlider_.setParameterID("swing");
    addAndMakeVisible(swingSlider_);
    addAndMakeVisible(swingLabel_);

    // Humanize timing slider
    humanizeTimeLabel_.setText("Human Time", juce::dontSendNotification);
    humanizeTimeLabel_.attachToComponent(&humanizeTimeSlider_, true);
    humanizeTimeSlider_.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    humanizeTimeSlider_.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    humanizeTimeSlider_.setRange(0.0, 1.0, 0.01);
    humanizeTimeSlider_.setMidiLearnManager(&processor_.getMidiLearnManager());
    humanizeTimeSlider_.setParameterID("humanizeTiming");
    addAndMakeVisible(humanizeTimeSlider_);
    addAndMakeVisible(humanizeTimeLabel_);

    // Humanize velocity slider
    humanizeVelLabel_.setText("Human Vel", juce::dontSendNotification);
    humanizeVelLabel_.attachToComponent(&humanizeVelSlider_, true);
    humanizeVelSlider_.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    humanizeVelSlider_.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    humanizeVelSlider_.setRange(0.0, 1.0, 0.01);
    humanizeVelSlider_.setMidiLearnManager(&processor_.getMidiLearnManager());
    humanizeVelSlider_.setParameterID("humanizeVelocity");
    addAndMakeVisible(humanizeVelSlider_);
    addAndMakeVisible(humanizeVelLabel_);

    // Create attachments
    gridAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor_.getAPVTS(), "gridResolution", gridCombo_);
    strengthAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getAPVTS(), "quantizeStrength", strengthSlider_);
    swingAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getAPVTS(), "swing", swingSlider_);
    humanizeTimeAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getAPVTS(), "humanizeTiming", humanizeTimeSlider_);
    humanizeVelAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getAPVTS(), "humanizeVelocity", humanizeVelSlider_);
}

MIDIGrooveQuantizerEditor::~MIDIGrooveQuantizerEditor() {}

void MIDIGrooveQuantizerEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff1e1e1e));

    g.setColour(juce::Colour(0xff00d4ff));
    g.setFont(28.0f);
    g.drawText("MIDI Groove Quantizer", getLocalBounds().removeFromTop(50), juce::Justification::centred);

    g.setColour(juce::Colour(0xff404040));
    g.drawRect(getLocalBounds().reduced(10), 2);
}

void MIDIGrooveQuantizerEditor::resized() {
    auto bounds = getLocalBounds().reduced(20);
    bounds.removeFromTop(50); // Title space

    // Top row: Grid combo
    auto topRow = bounds.removeFromTop(60);
    gridCombo_.setBounds(topRow.removeFromLeft(200).reduced(5));

    bounds.removeFromTop(20);

    // Knobs row
    auto knobsRow = bounds.removeFromTop(140);
    int knobWidth = knobsRow.getWidth() / 4;

    strengthSlider_.setBounds(knobsRow.removeFromLeft(knobWidth).reduced(10));
    swingSlider_.setBounds(knobsRow.removeFromLeft(knobWidth).reduced(10));
    humanizeTimeSlider_.setBounds(knobsRow.removeFromLeft(knobWidth).reduced(10));
    humanizeVelSlider_.setBounds(knobsRow.removeFromLeft(knobWidth).reduced(10));
}
