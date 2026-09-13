#include "PluginEditor.h"

VocalDeEsserEditor::VocalDeEsserEditor(VocalDeEsserProcessor& p)
    : AudioProcessorEditor(&p), processor_(p) {

    auto& apvts = processor_.getAPVTS();

    // Setup sliders and labels
    auto setupSlider = [&](juce::Slider& slider, juce::Label& label,
                          const juce::String& labelText, const juce::String& paramID,
                          std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment) {
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
        addAndMakeVisible(slider);

        label.setText(labelText, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.attachToComponent(&slider, false);
        addAndMakeVisible(label);

        attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, paramID, slider);
    };

    setupSlider(frequencySlider_, frequencyLabel_, "Frequency",
                VocalDeEsserProcessor::PARAM_FREQUENCY, frequencyAttachment_);
    setupSlider(bandwidthSlider_, bandwidthLabel_, "Bandwidth",
                VocalDeEsserProcessor::PARAM_BANDWIDTH, bandwidthAttachment_);
    setupSlider(thresholdSlider_, thresholdLabel_, "Threshold",
                VocalDeEsserProcessor::PARAM_THRESHOLD, thresholdAttachment_);
    setupSlider(ratioSlider_, ratioLabel_, "Ratio",
                VocalDeEsserProcessor::PARAM_RATIO, ratioAttachment_);
    setupSlider(attackSlider_, attackLabel_, "Attack",
                VocalDeEsserProcessor::PARAM_ATTACK, attackAttachment_);
    setupSlider(releaseSlider_, releaseLabel_, "Release",
                VocalDeEsserProcessor::PARAM_RELEASE, releaseAttachment_);
    setupSlider(mixSlider_, mixLabel_, "Mix",
                VocalDeEsserProcessor::PARAM_MIX, mixAttachment_);

    // Gain reduction label
    grLabel_.setText("GR: 0.0 dB", juce::dontSendNotification);
    grLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(grLabel_);

    startTimerHz(30);
    setSize(800, 400);
}

VocalDeEsserEditor::~VocalDeEsserEditor() {
    stopTimer();
}

void VocalDeEsserEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);

    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawText("Vocal DeEsser", getLocalBounds().removeFromTop(50),
               juce::Justification::centred);
}

void VocalDeEsserEditor::resized() {
    auto bounds = getLocalBounds().reduced(20);
    bounds.removeFromTop(50); // Title

    // Gain reduction meter
    grLabel_.setBounds(bounds.removeFromTop(30));

    // Sliders in a grid
    auto sliderHeight = 150;
    auto sliderWidth = getWidth() / 4 - 30;

    auto row1 = bounds.removeFromTop(sliderHeight);
    frequencySlider_.setBounds(row1.removeFromLeft(sliderWidth));
    row1.removeFromLeft(10);
    bandwidthSlider_.setBounds(row1.removeFromLeft(sliderWidth));
    row1.removeFromLeft(10);
    thresholdSlider_.setBounds(row1.removeFromLeft(sliderWidth));
    row1.removeFromLeft(10);
    ratioSlider_.setBounds(row1.removeFromLeft(sliderWidth));

    bounds.removeFromTop(20);
    auto row2 = bounds.removeFromTop(sliderHeight);
    attackSlider_.setBounds(row2.removeFromLeft(sliderWidth));
    row2.removeFromLeft(10);
    releaseSlider_.setBounds(row2.removeFromLeft(sliderWidth));
    row2.removeFromLeft(10);
    mixSlider_.setBounds(row2.removeFromLeft(sliderWidth));
}

void VocalDeEsserEditor::timerCallback() {
    displayGainReduction_ = processor_.getGainReduction();
    grLabel_.setText("GR: " + juce::String(displayGainReduction_ * 100.0f, 1) + " %",
                    juce::dontSendNotification);
}
