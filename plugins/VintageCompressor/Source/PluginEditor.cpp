#include "PluginEditor.h"

CompressorEditor::CompressorEditor(CompressorProcessor& p)
    : AudioProcessorEditor(&p), processor_(p) {
    setSize(850, 450);

    modeBox_.addItem("VCA (Clean)", 1);
    modeBox_.addItem("FET (Aggressive)", 2);
    modeBox_.addItem("Opto (Smooth)", 3);
    addAndMakeVisible(modeBox_);
    modeAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor_.getValueTreeState(), "mode", modeBox_);

    auto setup = [this](juce::Label& l, const char* t, juce::Slider& s, auto& a, const char* id) {
        l.setText(t, juce::dontSendNotification);
        l.setJustificationType(juce::Justification::centred);
        l.setColour(juce::Label::textColourId, juce::Colour(0xffdddddd));
        addAndMakeVisible(l);
        setupRotarySlider(s);
        addAndMakeVisible(s);
        a = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor_.getValueTreeState(), id, s);
    };

    setup(thresholdLabel_, "Threshold", thresholdSlider_, thresholdAttachment_, "threshold");
    setup(ratioLabel_, "Ratio", ratioSlider_, ratioAttachment_, "ratio");
    setup(attackLabel_, "Attack", attackSlider_, attackAttachment_, "attack");
    setup(releaseLabel_, "Release", releaseSlider_, releaseAttachment_, "release");
    setup(kneeLabel_, "Knee", kneeSlider_, kneeAttachment_, "knee");
    setup(makeupLabel_, "Makeup", makeupSlider_, makeupAttachment_, "makeup");
    setup(mixLabel_, "Mix", mixSlider_, mixAttachment_, "mix");

    startTimerHz(30);
}

CompressorEditor::~CompressorEditor() { stopTimer(); }

void CompressorEditor::timerCallback() {
    currentGainReduction_ = processor_.getGainReduction();
    repaint();
}

void CompressorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff1a1a1a));
    g.setColour(juce::Colour(0xff44aa88));
    g.setFont(juce::Font(38.0f, juce::Font::bold));
    g.drawText("VINTAGE COMPRESSOR", getLocalBounds().removeFromTop(65), juce::Justification::centred);

    g.setFont(juce::Font(15.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff222222));
    g.fillRect(20, 90, 810, 330);

    g.setColour(juce::Colour(0xff44aa88));
    g.drawText("DYNAMICS", 30, 95, 200, 22, juce::Justification::left);
    g.drawText("TIMING", 330, 95, 200, 22, juce::Justification::left);
    g.drawText("OUTPUT", 630, 95, 170, 22, juce::Justification::left);

    // Gain Reduction Meter
    g.setColour(juce::Colour(0xff333333));
    g.fillRect(690, 220, 120, 180);
    if (currentGainReduction_ > 0.1f) {
        float height = juce::jmin(currentGainReduction_ / 20.0f, 1.0f) * 170.0f;
        g.setColour(juce::Colour(0xffff6633));
        g.fillRect(695.0f, 220.0f + (170.0f - height), 110.0f, height);
    }
    g.setColour(juce::Colour(0xff888888));
    g.drawText("GR", 690, 195, 120, 20, juce::Justification::centred);
    g.setFont(juce::Font(12.0f));
    juce::String grText = juce::String(currentGainReduction_, 1) + " dB";
    g.drawText(grText, 690, 405, 120, 15, juce::Justification::centred);
}

void CompressorEditor::resized() {
    modeBox_.setBounds(350, 25, 150, 30);

    int ks = 75;
    thresholdLabel_.setBounds(40, 130, ks, 18); thresholdSlider_.setBounds(40, 150, ks, ks);
    ratioLabel_.setBounds(135, 130, ks, 18); ratioSlider_.setBounds(135, 150, ks, ks);
    kneeLabel_.setBounds(230, 130, ks, 18); kneeSlider_.setBounds(230, 150, ks, ks);

    attackLabel_.setBounds(340, 130, ks, 18); attackSlider_.setBounds(340, 150, ks, ks);
    releaseLabel_.setBounds(435, 130, ks, 18); releaseSlider_.setBounds(435, 150, ks, ks);

    makeupLabel_.setBounds(540, 130, ks, 18); makeupSlider_.setBounds(540, 150, ks, ks);
    mixLabel_.setBounds(40, 270, ks, 18); mixSlider_.setBounds(40, 290, ks, ks);
}

void CompressorEditor::setupRotarySlider(juce::Slider& s) {
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 75, 18);
    s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff44aa88));
    s.setColour(juce::Slider::thumbColourId, juce::Colour(0xff338866));
    s.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffdddddd));
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}
