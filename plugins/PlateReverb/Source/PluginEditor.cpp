#include "PluginEditor.h"
ReverbEditor::ReverbEditor(ReverbProcessor& p) : AudioProcessorEditor(&p), processor_(p),
      presetBrowser_(processor_.getPresetManager()) {
    setSize(700, 350);

    // Preset browser
    addAndMakeVisible(presetBrowser_);
    auto setup = [this](juce::Label& l, const char* t, juce::Slider& s, auto& a, const char* id) {
        l.setText(t, juce::dontSendNotification); l.setJustificationType(juce::Justification::centred);
        l.setColour(juce::Label::textColourId, juce::Colour(0xffdddddd)); addAndMakeVisible(l);
        setupRotarySlider(s); addAndMakeVisible(s);
        a = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor_.getValueTreeState(), id, s);
    };
    setup(sizeLabel_, "Size", sizeSlider_, sizeAttachment_, "size");
    setup(dampingLabel_, "Damping", dampingSlider_, dampingAttachment_, "damping");
    setup(widthLabel_, "Width", widthSlider_, widthAttachment_, "width");
    setup(predelayLabel_, "PreDelay", predelaySlider_, predelayAttachment_, "predelay");
    setup(mixLabel_, "Mix", mixSlider_, mixAttachment_, "mix");
}
ReverbEditor::~ReverbEditor() {}
void ReverbEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff1a1a1a));
    g.setColour(juce::Colour(0xff6699ff));
    g.setFont(juce::Font(38.0f, juce::Font::bold));
    g.drawText("PLATE REVERB", getLocalBounds().removeFromTop(65), juce::Justification::centred);
    g.setFont(juce::Font(15.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff222222));
    g.fillRect(20, 90, 660, 230);
    g.setColour(juce::Colour(0xff6699ff));
    g.drawText("PLATE SIMULATION", 30, 95, 630, 22, juce::Justification::left);
}
void ReverbEditor::resized() {
    int ks = 80;
    sizeLabel_.setBounds(40, 140, ks, 18); sizeSlider_.setBounds(40, 160, ks, ks);
    dampingLabel_.setBounds(150, 140, ks, 18); dampingSlider_.setBounds(150, 160, ks, ks);
    widthLabel_.setBounds(260, 140, ks, 18); widthSlider_.setBounds(260, 160, ks, ks);
    predelayLabel_.setBounds(370, 140, ks, 18); predelaySlider_.setBounds(370, 160, ks, ks);
    mixLabel_.setBounds(480, 140, ks, 18); mixSlider_.setBounds(480, 160, ks, ks);
}
void ReverbEditor::setupRotarySlider(juce::Slider& s) {
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 18);
    s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff6699ff));
    s.setColour(juce::Slider::thumbColourId, juce::Colour(0xff4477dd));
    s.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffdddddd));
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}
