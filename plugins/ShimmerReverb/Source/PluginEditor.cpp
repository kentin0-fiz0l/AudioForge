#include "PluginEditor.h"
ShimmerEditor::ShimmerEditor(ShimmerProcessor& p) : AudioProcessorEditor(&p), processor_(p),
      presetBrowser_(processor_.getPresetManager()) {
    setSize(750, 350);

    // Preset browser
    addAndMakeVisible(presetBrowser_);
    pitchModeBox_.addItem("Octave Up", 1); pitchModeBox_.addItem("Octave Down", 2); pitchModeBox_.addItem("Both", 3);
    addAndMakeVisible(pitchModeBox_);
    pitchModeAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor_.getValueTreeState(), "pitchMode", pitchModeBox_);
    auto setup = [this](juce::Label& l, const char* t, juce::Slider& s, auto& a, const char* id) {
        l.setText(t, juce::dontSendNotification); l.setJustificationType(juce::Justification::centred);
        l.setColour(juce::Label::textColourId, juce::Colour(0xffdddddd)); addAndMakeVisible(l);
        setupRotarySlider(s); addAndMakeVisible(s);
        a = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor_.getValueTreeState(), id, s);
    };
    setup(sizeLabel_, "Size", sizeSlider_, sizeAttachment_, "size");
    setup(dampingLabel_, "Damping", dampingSlider_, dampingAttachment_, "damping");
    setup(shimmerLabel_, "Shimmer", shimmerSlider_, shimmerAttachment_, "shimmer");
    setup(feedbackLabel_, "Feedback", feedbackSlider_, feedbackAttachment_, "feedback");
    setup(mixLabel_, "Mix", mixSlider_, mixAttachment_, "mix");
}
ShimmerEditor::~ShimmerEditor() {}
void ShimmerEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff1a1a1a));
    g.setColour(juce::Colour(0xff88ddff));
    g.setFont(juce::Font(38.0f, juce::Font::bold));
    g.drawText("SHIMMER REVERB", getLocalBounds().removeFromTop(65), juce::Justification::centred);
    g.setFont(juce::Font(15.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff222222));
    g.fillRect(20, 90, 710, 230);
    g.setColour(juce::Colour(0xff88ddff));
    g.drawText("PITCH-SHIFTING REVERB", 30, 95, 680, 22, juce::Justification::left);
}
void ShimmerEditor::resized() {
    pitchModeBox_.setBounds(300, 25, 150, 30);
    int ks = 75;
    sizeLabel_.setBounds(40, 140, ks, 18); sizeSlider_.setBounds(40, 160, ks, ks);
    dampingLabel_.setBounds(140, 140, ks, 18); dampingSlider_.setBounds(140, 160, ks, ks);
    shimmerLabel_.setBounds(240, 140, ks, 18); shimmerSlider_.setBounds(240, 160, ks, ks);
    feedbackLabel_.setBounds(340, 140, ks, 18); feedbackSlider_.setBounds(340, 160, ks, ks);
    mixLabel_.setBounds(440, 140, ks, 18); mixSlider_.setBounds(440, 160, ks, ks);
}
void ShimmerEditor::setupRotarySlider(juce::Slider& s) {
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 75, 18);
    s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff88ddff));
    s.setColour(juce::Slider::thumbColourId, juce::Colour(0xff66bbdd));
    s.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffdddddd));
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}
