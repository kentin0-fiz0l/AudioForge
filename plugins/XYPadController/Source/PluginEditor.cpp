#include "PluginEditor.h"

XYPadControllerEditor::XYPadControllerEditor(XYPadControllerProcessor& p)
    : AudioProcessorEditor(&p), processor_(p), xyPad_(p.getAPVTS()),
      presetBrowser_(processor_.getPresetManager()) {

    setSize(500, 550);

    // Preset browser
    addAndMakeVisible(presetBrowser_);

    // XY Pad
    addAndMakeVisible(xyPad_);

    // Set up callback to send CC messages
    xyPad_.onCCChange = [this](int cc, float value) {
        processor_.queueCCMessage(cc, value);
    };

    // X CC control
    xCCLabel_.setText("X CC", juce::dontSendNotification);
    xCCLabel_.attachToComponent(&xCCSlider_, true);
    xCCSlider_.setSliderStyle(juce::Slider::LinearHorizontal);
    xCCSlider_.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    xCCSlider_.setRange(1, 127, 1);
    addAndMakeVisible(xCCSlider_);
    addAndMakeVisible(xCCLabel_);

    // Y CC control
    yCCLabel_.setText("Y CC", juce::dontSendNotification);
    yCCLabel_.attachToComponent(&yCCSlider_, true);
    yCCSlider_.setSliderStyle(juce::Slider::LinearHorizontal);
    yCCSlider_.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    yCCSlider_.setRange(1, 127, 1);
    addAndMakeVisible(yCCSlider_);
    addAndMakeVisible(yCCLabel_);

    // MIDI Channel control
    channelLabel_.setText("Channel", juce::dontSendNotification);
    channelLabel_.attachToComponent(&channelSlider_, true);
    channelSlider_.setSliderStyle(juce::Slider::LinearHorizontal);
    channelSlider_.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    channelSlider_.setRange(1, 16, 1);
    addAndMakeVisible(channelSlider_);
    addAndMakeVisible(channelLabel_);

    // Create attachments
    xCCAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getAPVTS(), "xCC", xCCSlider_);
    yCCAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getAPVTS(), "yCC", yCCSlider_);
    channelAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor_.getAPVTS(), "channel", channelSlider_);
}

XYPadControllerEditor::~XYPadControllerEditor() {}

void XYPadControllerEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff1e1e1e));

    g.setColour(juce::Colour(0xff00d4ff));
    g.setFont(28.0f);
    g.drawText("XY Pad Controller", getLocalBounds().removeFromTop(50), juce::Justification::centred);

    g.setColour(juce::Colour(0xff404040));
    g.drawRect(getLocalBounds().reduced(10), 2);
}

void XYPadControllerEditor::resized() {
    auto bounds = getLocalBounds().reduced(20);
    bounds.removeFromTop(50); // Title space

    bounds.removeFromTop(10);

    // XY Pad (main area)
    auto padArea = bounds.removeFromTop(380);
    xyPad_.setBounds(padArea);

    bounds.removeFromTop(20);

    // Controls at bottom
    auto xCCRow = bounds.removeFromTop(30);
    xCCSlider_.setBounds(xCCRow.removeFromRight(xCCRow.getWidth() - 80).reduced(5));

    bounds.removeFromTop(10);

    auto yCCRow = bounds.removeFromTop(30);
    yCCSlider_.setBounds(yCCRow.removeFromRight(yCCRow.getWidth() - 80).reduced(5));

    bounds.removeFromTop(10);

    auto channelRow = bounds.removeFromTop(30);
    channelSlider_.setBounds(channelRow.removeFromRight(channelRow.getWidth() - 80).reduced(5));
}
