#include "PluginEditor.h"
TomEditor::TomEditor(TomProcessor& p) : AudioProcessorEditor(&p), processor_(p) {
    setSize(700, 350);
    auto setup = [this](juce::Label& l, const char* t, juce::Slider& s, auto& a, const char* id) {
        l.setText(t, juce::dontSendNotification); l.setJustificationType(juce::Justification::centred);
        l.setColour(juce::Label::textColourId, juce::Colour(0xffdddddd)); addAndMakeVisible(l);
        setupRotarySlider(s); addAndMakeVisible(s);
        a = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor_.getValueTreeState(), id, s);
    };
    setup(pitchLabel_, "Pitch", pitchSlider_, pitchAttachment_, "pitch");
    setup(pitchEnvLabel_, "Pitch Env", pitchEnvSlider_, pitchEnvAttachment_, "pitchEnv");
    setup(decayLabel_, "Decay", decaySlider_, decayAttachment_, "decay");
    setup(bendLabel_, "Bend", bendSlider_, bendAttachment_, "bend");
    setup(noiseLabel_, "Noise", noiseSlider_, noiseAttachment_, "noise");
}
TomEditor::~TomEditor() {}
void TomEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff1a1a1a));
    g.setColour(juce::Colour(0xffff6699)); // Pink
    g.setFont(juce::Font(42.0f, juce::Font::bold));
    g.drawText("TOM SYNTH", getLocalBounds().removeFromTop(70), juce::Justification::centred);
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff222222));
    g.fillRect(20, 90, 660, 230);
    g.setColour(juce::Colour(0xffff6699));
    g.drawText("TUNED TOMS", 30, 95, 640, 25, juce::Justification::left);
}
void TomEditor::resized() {
    int ks = 80;
    pitchLabel_.setBounds(40, 140, ks, 20); pitchSlider_.setBounds(40, 165, ks, ks);
    pitchEnvLabel_.setBounds(150, 140, ks, 20); pitchEnvSlider_.setBounds(150, 165, ks, ks);
    decayLabel_.setBounds(260, 140, ks, 20); decaySlider_.setBounds(260, 165, ks, ks);
    bendLabel_.setBounds(370, 140, ks, 20); bendSlider_.setBounds(370, 165, ks, ks);
    noiseLabel_.setBounds(480, 140, ks, 20); noiseSlider_.setBounds(480, 165, ks, ks);
}
void TomEditor::setupRotarySlider(juce::Slider& s) {
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffff6699));
    s.setColour(juce::Slider::thumbColourId, juce::Colour(0xffdd4477));
    s.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffdddddd));
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}
