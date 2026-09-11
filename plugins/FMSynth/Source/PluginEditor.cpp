#include "PluginEditor.h"

FMEditor::FMEditor(FMProcessor& p)
    : AudioProcessorEditor(&p), processor_(p),
      presetBrowser_(processor_.getPresetManager())
{
    setSize(1100, 550);

    // Preset browser
    addAndMakeVisible(presetBrowser_);

    auto setupControl = [this](juce::Label& label, const char* text, juce::Slider& slider, auto& attachment, const char* paramId) {
        label.setText(text, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, juce::Colour(0xffdddddd));
        addAndMakeVisible(label);
        setupRotarySlider(slider);
        addAndMakeVisible(slider);
        attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor_.getValueTreeState(), paramId, slider);
    };

    setupControl(algorithmLabel_, "Algorithm", algorithmSlider_, algorithmAttachment_, "algorithm");
    setupControl(feedbackLabel_, "Feedback", feedbackSlider_, feedbackAttachment_, "feedback");

    for (int op = 0; op < 4; ++op)
    {
        juce::String prefix = "op" + juce::String(op + 1);
        opLabel_[op].setText("OP" + juce::String(op + 1), juce::dontSendNotification);
        opLabel_[op].setFont(juce::Font(18.0f, juce::Font::bold));
        opLabel_[op].setJustificationType(juce::Justification::centred);
        opLabel_[op].setColour(juce::Label::textColourId, juce::Colour(0xffff6600));
        addAndMakeVisible(opLabel_[op]);

        setupControl(levelLabel_[op], "Level", levelSlider_[op], levelAttachment_[op], (prefix + "Level").toRawUTF8());
        setupControl(ratioLabel_[op], "Ratio", ratioSlider_[op], ratioAttachment_[op], (prefix + "Ratio").toRawUTF8());
        setupControl(attackLabel_[op], "A", attackSlider_[op], attackAttachment_[op], (prefix + "Attack").toRawUTF8());
        setupControl(decayLabel_[op], "D", decaySlider_[op], decayAttachment_[op], (prefix + "Decay").toRawUTF8());
        setupControl(sustainLabel_[op], "S", sustainSlider_[op], sustainAttachment_[op], (prefix + "Sustain").toRawUTF8());
        setupControl(releaseLabel_[op], "R", releaseSlider_[op], releaseAttachment_[op], (prefix + "Release").toRawUTF8());
    }
}

FMEditor::~FMEditor() {}

void FMEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1a1a));
    g.setColour(juce::Colour(0xffff6600));
    g.setFont(juce::Font(36.0f, juce::Font::bold));
    g.drawText("FM SYNTH", getLocalBounds().removeFromTop(70), juce::Justification::centred);

    g.setFont(juce::Font(14.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff222222));
    g.fillRect(20, 80, 200, 200);
    g.fillRect(240, 80, 200, 450);
    g.fillRect(460, 80, 200, 450);
    g.fillRect(680, 80, 200, 450);
    g.fillRect(900, 80, 200, 450);

    g.setColour(juce::Colour(0xffff6600));
    g.drawText("GLOBAL", 30, 85, 180, 25, juce::Justification::left);
}

void FMEditor::resized()
{
    int knobSize = 55;
    algorithmLabel_.setBounds(40, 120, knobSize, 20); algorithmSlider_.setBounds(40, 145, knobSize, knobSize);
    feedbackLabel_.setBounds(120, 120, knobSize, 20); feedbackSlider_.setBounds(120, 145, knobSize, knobSize);

    for (int op = 0; op < 4; ++op)
    {
        int x = 240 + op * 220;
        opLabel_[op].setBounds(x + 60, 90, 80, 30);
        levelLabel_[op].setBounds(x + 20, 130, knobSize, 18); levelSlider_[op].setBounds(x + 20, 150, knobSize, knobSize);
        ratioLabel_[op].setBounds(x + 95, 130, knobSize, 18); ratioSlider_[op].setBounds(x + 95, 150, knobSize, knobSize);
        attackLabel_[op].setBounds(x + 20, 240, knobSize, 18); attackSlider_[op].setBounds(x + 20, 260, knobSize, knobSize);
        decayLabel_[op].setBounds(x + 95, 240, knobSize, 18); decaySlider_[op].setBounds(x + 95, 260, knobSize, knobSize);
        sustainLabel_[op].setBounds(x + 20, 350, knobSize, 18); sustainSlider_[op].setBounds(x + 20, 370, knobSize, knobSize);
        releaseLabel_[op].setBounds(x + 95, 350, knobSize, 18); releaseSlider_[op].setBounds(x + 95, 370, knobSize, knobSize);
    }
}

void FMEditor::setupRotarySlider(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 55, 18);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffff6600));
    slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffdd5500));
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffdddddd));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}
