#include "PluginEditor.h"

VocalCompressorEditor::VocalCompressorEditor(VocalCompressorProcessor& p)
    : AudioProcessorEditor(&p), processor_(p) {

    auto& apvts = processor_.getAPVTS();

    // Setup sliders
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

    setupSlider(thresholdSlider_, thresholdLabel_, "Threshold",
                VocalCompressorProcessor::PARAM_THRESHOLD, thresholdAttachment_);
    setupSlider(ratioSlider_, ratioLabel_, "Ratio",
                VocalCompressorProcessor::PARAM_RATIO, ratioAttachment_);
    setupSlider(attackSlider_, attackLabel_, "Attack",
                VocalCompressorProcessor::PARAM_ATTACK, attackAttachment_);
    setupSlider(releaseSlider_, releaseLabel_, "Release",
                VocalCompressorProcessor::PARAM_RELEASE, releaseAttachment_);
    setupSlider(kneeSlider_, kneeLabel_, "Knee",
                VocalCompressorProcessor::PARAM_KNEE, kneeAttachment_);
    setupSlider(makeupSlider_, makeupLabel_, "Makeup Gain",
                VocalCompressorProcessor::PARAM_MAKEUP, makeupAttachment_);
    setupSlider(parallelMixSlider_, parallelMixLabel_, "Parallel Mix",
                VocalCompressorProcessor::PARAM_PARALLEL_MIX, parallelMixAttachment_);

    // Detection mode combo box
    detectionBox_.addItem("RMS", 1);
    detectionBox_.addItem("Peak", 2);
    addAndMakeVisible(detectionBox_);

    detectionLabel_.setText("Detection", juce::dontSendNotification);
    detectionLabel_.setJustificationType(juce::Justification::centred);
    detectionLabel_.attachToComponent(&detectionBox_, false);
    addAndMakeVisible(detectionLabel_);

    detectionAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts, VocalCompressorProcessor::PARAM_DETECTION, detectionBox_);

    // Metering labels
    grLabel_.setText("GR: 0.0 dB", juce::dontSendNotification);
    grLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(grLabel_);

    outputLabel_.setText("Out: 0.0 dB", juce::dontSendNotification);
    outputLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(outputLabel_);

    startTimerHz(30);
    setSize(900, 450);
}

VocalCompressorEditor::~VocalCompressorEditor() {
    stopTimer();
}

void VocalCompressorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);

    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawText("Vocal Compressor", getLocalBounds().removeFromTop(50),
               juce::Justification::centred);
}

void VocalCompressorEditor::resized() {
    auto bounds = getLocalBounds().reduced(20);
    bounds.removeFromTop(50); // Title

    // Metering row
    auto meterRow = bounds.removeFromTop(40);
    grLabel_.setBounds(meterRow.removeFromLeft(getWidth() / 2 - 20));
    outputLabel_.setBounds(meterRow);

    bounds.removeFromTop(10);

    // Sliders in a grid (2 rows x 4 columns)
    auto sliderHeight = 150;
    auto sliderWidth = getWidth() / 4 - 30;

    // Row 1: Threshold, Ratio, Attack, Release
    auto row1 = bounds.removeFromTop(sliderHeight);
    thresholdSlider_.setBounds(row1.removeFromLeft(sliderWidth));
    row1.removeFromLeft(10);
    ratioSlider_.setBounds(row1.removeFromLeft(sliderWidth));
    row1.removeFromLeft(10);
    attackSlider_.setBounds(row1.removeFromLeft(sliderWidth));
    row1.removeFromLeft(10);
    releaseSlider_.setBounds(row1.removeFromLeft(sliderWidth));

    bounds.removeFromTop(20);

    // Row 2: Knee, Makeup, Parallel Mix, Detection
    auto row2 = bounds.removeFromTop(sliderHeight);
    kneeSlider_.setBounds(row2.removeFromLeft(sliderWidth));
    row2.removeFromLeft(10);
    makeupSlider_.setBounds(row2.removeFromLeft(sliderWidth));
    row2.removeFromLeft(10);
    parallelMixSlider_.setBounds(row2.removeFromLeft(sliderWidth));
    row2.removeFromLeft(10);

    // Detection box (smaller, centered in its space)
    auto detectionArea = row2.removeFromLeft(sliderWidth);
    detectionArea.removeFromTop(30); // Space for label
    detectionBox_.setBounds(detectionArea.removeFromTop(30));
}

void VocalCompressorEditor::timerCallback() {
    displayGainReduction_ = processor_.getGainReduction();
    displayOutputLevel_ = processor_.getOutputLevel();

    grLabel_.setText("GR: " + juce::String(displayGainReduction_ * 100.0f, 1) + " %",
                    juce::dontSendNotification);

    float outputDb = juce::Decibels::gainToDecibels(displayOutputLevel_, -60.0f);
    outputLabel_.setText("Out: " + juce::String(outputDb, 1) + " dB",
                        juce::dontSendNotification);
}
