#include "PluginEditor.h"
HiHatEditor::HiHatEditor(HiHatProcessor& p) : AudioProcessorEditor(&p), processor_(p),
      presetBrowser_(processor_.getPresetManager()) {
    setSize(600, 350);

    // Preset browser
    addAndMakeVisible(presetBrowser_);
    auto setup = [this](juce::Label& l, const char* t, juce::Slider& s, auto& a, const char* id) {
        l.setText(t, juce::dontSendNotification); l.setJustificationType(juce::Justification::centred);
        l.setColour(juce::Label::textColourId, juce::Colour(0xffdddddd)); addAndMakeVisible(l);
        setupRotarySlider(s); addAndMakeVisible(s);
        a = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor_.getValueTreeState(), id, s);
    };
    setup(toneLabel_, "Tone", toneSlider_, toneAttachment_, "tone");
    setup(decayLabel_, "Decay", decaySlider_, decayAttachment_, "decay");
    setup(metallicLabel_, "Metallic", metallicSlider_, metallicAttachment_, "metallic");
    setup(opennessLabel_, "Openness", opennessSlider_, opennessAttachment_, "openness");
}
HiHatEditor::~HiHatEditor() {}
void HiHatEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff1a1a1a));
    g.setColour(juce::Colour(0xff66ccff)); // Cyan
    g.setFont(juce::Font(42.0f, juce::Font::bold));
    g.drawText("HI-HAT", getLocalBounds().removeFromTop(70), juce::Justification::centred);
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff222222));
    g.fillRect(20, 90, 560, 230);
    g.setColour(juce::Colour(0xff66ccff));
    g.drawText("METALLIC PERCUSSION", 30, 95, 540, 25, juce::Justification::left);
}
void HiHatEditor::resized() {
    int ks = 80;
    toneLabel_.setBounds(60, 140, ks, 20); toneSlider_.setBounds(60, 165, ks, ks);
    decayLabel_.setBounds(180, 140, ks, 20); decaySlider_.setBounds(180, 165, ks, ks);
    metallicLabel_.setBounds(300, 140, ks, 20); metallicSlider_.setBounds(300, 165, ks, ks);
    opennessLabel_.setBounds(420, 140, ks, 20); opennessSlider_.setBounds(420, 165, ks, ks);
}
void HiHatEditor::setupRotarySlider(juce::Slider& s) {
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff66ccff));
    s.setColour(juce::Slider::thumbColourId, juce::Colour(0xff44aadd));
    s.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffdddddd));
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}
