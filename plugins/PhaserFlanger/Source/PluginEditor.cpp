#include "PluginEditor.h"

PhaserFlangerEditor::PhaserFlangerEditor(PhaserFlangerProcessor& p) : AudioProcessorEditor(&p), processor_(p) {
    setSize(750, 350);
    modeBox_.addItem("Phaser", 1); modeBox_.addItem("Flanger", 2);
    addAndMakeVisible(modeBox_);
    modeAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor_.getValueTreeState(), "mode", modeBox_);

    auto setup = [this](juce::Label& l, const char* t, juce::Slider& s, auto& a, const char* id) {
        l.setText(t, juce::dontSendNotification); l.setJustificationType(juce::Justification::centred);
        l.setColour(juce::Label::textColourId, juce::Colour(0xffdddddd)); addAndMakeVisible(l);
        setupRotarySlider(s); addAndMakeVisible(s);
        a = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor_.getValueTreeState(), id, s);
    };
    setup(rateLabel_, "Rate", rateSlider_, rateAttachment_, "rate");
    setup(depthLabel_, "Depth", depthSlider_, depthAttachment_, "depth");
    setup(feedbackLabel_, "Feedback", feedbackSlider_, feedbackAttachment_, "feedback");
    setup(centreFreqLabel_, "Centre Freq", centreFreqSlider_, centreFreqAttachment_, "centreFreq");
    setup(mixLabel_, "Mix", mixSlider_, mixAttachment_, "mix");
}

PhaserFlangerEditor::~PhaserFlangerEditor() {}

void PhaserFlangerEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff1a1a1a));
    g.setColour(juce::Colour(0xff77cc88));
    g.setFont(juce::Font(38.0f, juce::Font::bold));
    g.drawText("PHASER / FLANGER", getLocalBounds().removeFromTop(65), juce::Justification::centred);
    g.setFont(juce::Font(15.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff222222));
    g.fillRect(20, 90, 710, 230);
    g.setColour(juce::Colour(0xff77cc88));
    g.drawText("DUAL-MODE MODULATION", 30, 95, 680, 22, juce::Justification::left);
}

void PhaserFlangerEditor::resized() {
    modeBox_.setBounds(300, 25, 150, 30);
    int ks = 70;
    rateLabel_.setBounds(40, 140, ks, 18); rateSlider_.setBounds(40, 160, ks, ks);
    depthLabel_.setBounds(130, 140, ks, 18); depthSlider_.setBounds(130, 160, ks, ks);
    feedbackLabel_.setBounds(220, 140, ks, 18); feedbackSlider_.setBounds(220, 160, ks, ks);
    centreFreqLabel_.setBounds(310, 140, ks, 18); centreFreqSlider_.setBounds(310, 160, ks, ks);
    mixLabel_.setBounds(400, 140, ks, 18); mixSlider_.setBounds(400, 160, ks, ks);
}

void PhaserFlangerEditor::setupRotarySlider(juce::Slider& s) {
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff77cc88));
    s.setColour(juce::Slider::thumbColourId, juce::Colour(0xff55aa66));
    s.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffdddddd));
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}
