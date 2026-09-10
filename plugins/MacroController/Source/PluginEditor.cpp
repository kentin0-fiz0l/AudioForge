#include "PluginEditor.h"

MacroControllerEditor::MacroControllerEditor(MacroControllerProcessor& p)
    : AudioProcessorEditor(&p), processor_(p) {

    setSize(400, 500);

    // Macro knob
    macroLabel_.setText("Macro", juce::dontSendNotification);
    macroLabel_.setJustificationType(juce::Justification::centred);
    macroLabel_.attachToComponent(&macroSlider_, false);
    macroSlider_.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    macroSlider_.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 25);
    macroSlider_.setRange(0.0, 1.0, 0.001);
    macroSlider_.setMidiLearnManager(&processor_.getMidiLearnManager());
    macroSlider_.setParameterID("macro");
    addAndMakeVisible(macroSlider_);
    addAndMakeVisible(macroLabel_);

    // Channel slider
    channelLabel_.setText("Channel", juce::dontSendNotification);
    channelLabel_.attachToComponent(&channelSlider_, true);
    channelSlider_.setSliderStyle(juce::Slider::LinearHorizontal);
    channelSlider_.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    channelSlider_.setRange(1, 16, 1);
    addAndMakeVisible(channelSlider_);
    addAndMakeVisible(channelLabel_);

    // Target displays
    for (int i = 0; i < 8; ++i) {
        auto& display = targetDisplays_[i];

        display.nameLabel.setText("Target " + juce::String(i + 1), juce::dontSendNotification);
        display.nameLabel.setJustificationType(juce::Justification::left);
        display.nameLabel.setColour(juce::Label::textColourId, juce::Colour(0xff808080));
        addAndMakeVisible(display.nameLabel);

        display.valueLabel.setText("---", juce::dontSendNotification);
        display.valueLabel.setJustificationType(juce::Justification::right);
        display.valueLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00d4ff));
        addAndMakeVisible(display.valueLabel);
    }

    // Attachments
    macroAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getAPVTS(), "macro", macroSlider_);
    channelAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getAPVTS(), "channel", channelSlider_);

    // Start timer for value updates
    startTimer(50);  // 20 Hz updates
}

MacroControllerEditor::~MacroControllerEditor() {
    stopTimer();
}

void MacroControllerEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff1e1e1e));

    g.setColour(juce::Colour(0xff00d4ff));
    g.setFont(28.0f);
    g.drawText("Macro Controller", getLocalBounds().removeFromTop(50), juce::Justification::centred);

    g.setColour(juce::Colour(0xff404040));
    g.drawRect(getLocalBounds().reduced(10), 2);

    // Draw section separator
    g.setColour(juce::Colour(0xff303030));
    g.fillRect(20, 250, getWidth() - 40, 1);
}

void MacroControllerEditor::resized() {
    auto bounds = getLocalBounds().reduced(20);
    bounds.removeFromTop(50); // Title

    bounds.removeFromTop(20);

    // Large macro knob
    auto macroArea = bounds.removeFromTop(150);
    macroSlider_.setBounds(macroArea.reduced(50, 10));

    bounds.removeFromTop(20);

    // Channel control
    auto channelRow = bounds.removeFromTop(30);
    channelSlider_.setBounds(channelRow.removeFromRight(channelRow.getWidth() - 80).reduced(5));

    bounds.removeFromTop(20);

    // Target displays (8 rows)
    for (int i = 0; i < 8; ++i) {
        auto row = bounds.removeFromTop(25);
        auto& display = targetDisplays_[i];

        display.nameLabel.setBounds(row.removeFromLeft(120));
        display.valueLabel.setBounds(row);
    }
}

void MacroControllerEditor::timerCallback() {
    // Update target value displays
    float macroValue = processor_.getAPVTS().getRawParameterValue("macro")->load();
    auto& engine = processor_.getMacroEngine();
    auto targetValues = engine.getTargetValues(macroValue);

    for (int i = 0; i < 8; ++i) {
        const auto& target = engine.getTarget(i);
        auto& display = targetDisplays_[i];

        if (target.enabled) {
            int ccValue = (int)(targetValues[i] * 127.0f);
            display.valueLabel.setText(
                "CC" + juce::String(target.cc) + ": " + juce::String(ccValue),
                juce::dontSendNotification);
            display.valueLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00d4ff));
        } else {
            display.valueLabel.setText("---", juce::dontSendNotification);
            display.valueLabel.setColour(juce::Label::textColourId, juce::Colour(0xff404040));
        }
    }
}
