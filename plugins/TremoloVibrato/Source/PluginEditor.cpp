#include "PluginEditor.h"

TremoloVibratoEditor::TremoloVibratoEditor(TremoloVibratoProcessor& p) : AudioProcessorEditor(&p), processor_(p) {
    setSize(650, 350);
    modeBox_.addItem("Tremolo", 1); modeBox_.addItem("Vibrato", 2);
    addAndMakeVisible(modeBox_);
    modeAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor_.getValueTreeState(), "mode", modeBox_);

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
    setup(mixLabel_, "Mix", mixSlider_, mixAttachment_, "mix");
}

TremoloVibratoEditor::~TremoloVibratoEditor() {}

void TremoloVibratoEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff1a1a1a));
    g.setColour(juce::Colour(0xffdd8855));
    g.setFont(juce::Font(38.0f, juce::Font::bold));
    g.drawText("TREMOLO / VIBRATO", getLocalBounds().removeFromTop(65), juce::Justification::centred);
    g.setFont(juce::Font(15.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff222222));
    g.fillRect(20, 90, 610, 230);
    g.setColour(juce::Colour(0xffdd8855));
    g.drawText("AMPLITUDE & PITCH MODULATION", 30, 95, 680, 22, juce::Justification::left);
}

void TremoloVibratoEditor::resized() {
    modeBox_.setBounds(200, 25, 120, 30);
    waveformBox_.setBounds(340, 25, 120, 30);
    int ks = 80;
    rateLabel_.setBounds(80, 140, ks, 18); rateSlider_.setBounds(80, 160, ks, ks);
    depthLabel_.setBounds(210, 140, ks, 18); depthSlider_.setBounds(210, 160, ks, ks);
    mixLabel_.setBounds(340, 140, ks, 18); mixSlider_.setBounds(340, 160, ks, ks);
}

void TremoloVibratoEditor::setupRotarySlider(juce::Slider& s) {
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 18);
    s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffdd8855));
    s.setColour(juce::Slider::thumbColourId, juce::Colour(0xffbb6633));
    s.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffdddddd));
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}
