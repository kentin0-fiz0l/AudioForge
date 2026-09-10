#include "PluginEditor.h"
ChorusEditor::ChorusEditor(ChorusProcessor& p) : AudioProcessorEditor(&p), processor_(p) {
    setSize(700, 350);
    auto setup = [this](juce::Label& l, const char* t, juce::Slider& s, auto& a, const char* id) {
        l.setText(t, juce::dontSendNotification); l.setJustificationType(juce::Justification::centred);
        l.setColour(juce::Label::textColourId, juce::Colour(0xffdddddd)); addAndMakeVisible(l);
        setupRotarySlider(s); addAndMakeVisible(s);
        a = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor_.getValueTreeState(), id, s);
    };
    setup(rateLabel_, "Rate", rateSlider_, rateAttachment_, "rate");
    setup(depthLabel_, "Depth", depthSlider_, depthAttachment_, "depth");
    setup(centreDelayLabel_, "Delay", centreDelaySlider_, centreDelayAttachment_, "centreDelay");
    setup(feedbackLabel_, "Feedback", feedbackSlider_, feedbackAttachment_, "feedback");
    setup(mixLabel_, "Mix", mixSlider_, mixAttachment_, "mix");
}
ChorusEditor::~ChorusEditor() {}
void ChorusEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff1a1a1a));
    g.setColour(juce::Colour(0xffaa66ff));
    g.setFont(juce::Font(38.0f, juce::Font::bold));
    g.drawText("STEREO CHORUS", getLocalBounds().removeFromTop(65), juce::Justification::centred);
    g.setFont(juce::Font(15.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff222222));
    g.fillRect(20, 90, 660, 230);
    g.setColour(juce::Colour(0xffaa66ff));
    g.drawText("MULTI-VOICE MODULATION", 30, 95, 630, 22, juce::Justification::left);
}
void ChorusEditor::resized() {
    int ks = 80;
    rateLabel_.setBounds(40, 140, ks, 18); rateSlider_.setBounds(40, 160, ks, ks);
    depthLabel_.setBounds(150, 140, ks, 18); depthSlider_.setBounds(150, 160, ks, ks);
    centreDelayLabel_.setBounds(260, 140, ks, 18); centreDelaySlider_.setBounds(260, 160, ks, ks);
    feedbackLabel_.setBounds(370, 140, ks, 18); feedbackSlider_.setBounds(370, 160, ks, ks);
    mixLabel_.setBounds(480, 140, ks, 18); mixSlider_.setBounds(480, 160, ks, ks);
}
void ChorusEditor::setupRotarySlider(juce::Slider& s) {
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 18);
    s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffaa66ff));
    s.setColour(juce::Slider::thumbColourId, juce::Colour(0xff8844dd));
    s.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffdddddd));
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}
