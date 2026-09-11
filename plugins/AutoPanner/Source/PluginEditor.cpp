#include "PluginEditor.h"

AutoPannerEditor::AutoPannerEditor(AutoPannerProcessor& p) : AudioProcessorEditor(&p), processor_(p),
      presetBrowser_(processor_.getPresetManager()) {
    setSize(700, 350);

    // Preset browser
    addAndMakeVisible(presetBrowser_);
    waveformBox_.addItem("Sine", 1); waveformBox_.addItem("Triangle", 2); waveformBox_.addItem("Square", 3);
    addAndMakeVisible(waveformBox_);
    waveformAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor_.getValueTreeState(), "waveform", waveformBox_);

    auto setup = [this](juce::Label& l, const char* t, juce::Slider& s, auto& a, const char* id) {
        l.setText(t, juce::dontSendNotification); l.setJustificationType(juce::Justification::centred);
        l.setColour(juce::Label::textColourId, juce::Colour(0xffdddddd)); addAndMakeVisible(l);
        setupRotarySlider(s); addAndMakeVisible(s);
        a = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor_.getValueTreeState(), id, s);
    };
    setup(rateLabel_, "Rate", rateSlider_, rateAttachment_, "rate");
    setup(depthLabel_, "Depth", depthSlider_, depthAttachment_, "depth");
    setup(phaseLabel_, "Phase Offset", phaseSlider_, phaseAttachment_, "phaseOffset");
    setup(widthLabel_, "Stereo Width", widthSlider_, widthAttachment_, "stereoWidth");
}

AutoPannerEditor::~AutoPannerEditor() {}

void AutoPannerEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff1a1a1a));
    g.setColour(juce::Colour(0xffaa66ff));
    g.setFont(juce::Font(38.0f, juce::Font::bold));
    g.drawText("AUTO-PANNER", getLocalBounds().removeFromTop(65), juce::Justification::centred);
    g.setFont(juce::Font(15.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff222222));
    g.fillRect(20, 90, 660, 230);
    g.setColour(juce::Colour(0xffaa66ff));
    g.drawText("LFO-DRIVEN STEREO MOVEMENT", 30, 95, 680, 22, juce::Justification::left);
}

void AutoPannerEditor::resized() {
    waveformBox_.setBounds(275, 25, 150, 30);
    int ks = 75;
    rateLabel_.setBounds(50, 140, ks, 18); rateSlider_.setBounds(50, 160, ks, ks);
    depthLabel_.setBounds(160, 140, ks, 18); depthSlider_.setBounds(160, 160, ks, ks);
    phaseLabel_.setBounds(270, 140, ks, 18); phaseSlider_.setBounds(270, 160, ks, ks);
    widthLabel_.setBounds(380, 140, ks, 18); widthSlider_.setBounds(380, 160, ks, ks);
}

void AutoPannerEditor::setupRotarySlider(juce::Slider& s) {
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 75, 18);
    s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffaa66ff));
    s.setColour(juce::Slider::thumbColourId, juce::Colour(0xff8844dd));
    s.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffdddddd));
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}
