#include "PluginEditor.h"
DelayEditor::DelayEditor(DelayProcessor& p) : AudioProcessorEditor(&p), processor_(p),
      presetBrowser_(processor_.getPresetManager()) {
    setSize(750, 350);

    // Preset browser
    addAndMakeVisible(presetBrowser_);
    auto setup = [this](juce::Label& l, const char* t, juce::Slider& s, auto& a, const char* id) {
        l.setText(t, juce::dontSendNotification); l.setJustificationType(juce::Justification::centred);
        l.setColour(juce::Label::textColourId, juce::Colour(0xffdddddd)); addAndMakeVisible(l);
        setupRotarySlider(s); addAndMakeVisible(s);
        a = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor_.getValueTreeState(), id, s);
    };
    setup(timeLabel_, "Time", timeSlider_, timeAttachment_, "time");
    setup(feedbackLabel_, "Feedback", feedbackSlider_, feedbackAttachment_, "feedback");
    setup(wowLabel_, "Wow", wowSlider_, wowAttachment_, "wow");
    setup(flutterLabel_, "Flutter", flutterSlider_, flutterAttachment_, "flutter");
    setup(saturationLabel_, "Saturation", saturationSlider_, saturationAttachment_, "saturation");
    setup(mixLabel_, "Mix", mixSlider_, mixAttachment_, "mix");
}
DelayEditor::~DelayEditor() {}
void DelayEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff1a1a1a));
    g.setColour(juce::Colour(0xffffaa55));
    g.setFont(juce::Font(38.0f, juce::Font::bold));
    g.drawText("TAPE DELAY", getLocalBounds().removeFromTop(65), juce::Justification::centred);
    g.setFont(juce::Font(15.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff222222));
    g.fillRect(20, 90, 710, 230);
    g.setColour(juce::Colour(0xffffaa55));
    g.drawText("ANALOG TAPE SIMULATION", 30, 95, 680, 22, juce::Justification::left);
}
void DelayEditor::resized() {
    int ks = 75;
    timeLabel_.setBounds(40, 140, ks, 18); timeSlider_.setBounds(40, 160, ks, ks);
    feedbackLabel_.setBounds(140, 140, ks, 18); feedbackSlider_.setBounds(140, 160, ks, ks);
    wowLabel_.setBounds(240, 140, ks, 18); wowSlider_.setBounds(240, 160, ks, ks);
    flutterLabel_.setBounds(340, 140, ks, 18); flutterSlider_.setBounds(340, 160, ks, ks);
    saturationLabel_.setBounds(440, 140, ks, 18); saturationSlider_.setBounds(440, 160, ks, ks);
    mixLabel_.setBounds(540, 140, ks, 18); mixSlider_.setBounds(540, 160, ks, ks);
}
void DelayEditor::setupRotarySlider(juce::Slider& s) {
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 75, 18);
    s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffffaa55));
    s.setColour(juce::Slider::thumbColourId, juce::Colour(0xffdd8833));
    s.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffdddddd));
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}
