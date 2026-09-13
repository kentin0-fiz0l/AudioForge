#include "PluginEditor.h"

VocalDoublerEditor::VocalDoublerEditor(VocalDoublerProcessor& p)
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

    // Voice count slider uses discrete steps
    voicesSlider_.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    voicesSlider_.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(voicesSlider_);
    voicesLabel_.setText("Voices", juce::dontSendNotification);
    voicesLabel_.setJustificationType(juce::Justification::centred);
    voicesLabel_.attachToComponent(&voicesSlider_, false);
    addAndMakeVisible(voicesLabel_);
    voicesAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, VocalDoublerProcessor::PARAM_VOICES, voicesSlider_);

    // Continuous sliders
    setupSlider(widthSlider_, widthLabel_, "Stereo Width",
                VocalDoublerProcessor::PARAM_WIDTH, widthAttachment_);
    setupSlider(pitchVarSlider_, pitchVarLabel_, "Pitch Variation",
                VocalDoublerProcessor::PARAM_PITCH_VAR, pitchVarAttachment_);
    setupSlider(timingSlider_, timingLabel_, "Timing Shift",
                VocalDoublerProcessor::PARAM_TIMING, timingAttachment_);
    setupSlider(humanizeSlider_, humanizeLabel_, "Humanize",
                VocalDoublerProcessor::PARAM_HUMANIZE, humanizeAttachment_);
    setupSlider(highPassSlider_, highPassLabel_, "High Pass",
                VocalDoublerProcessor::PARAM_HIGHPASS, highPassAttachment_);
    setupSlider(lowPassSlider_, lowPassLabel_, "Low Pass",
                VocalDoublerProcessor::PARAM_LOWPASS, lowPassAttachment_);
    setupSlider(mixSlider_, mixLabel_, "Mix",
                VocalDoublerProcessor::PARAM_MIX, mixAttachment_);
    setupSlider(gainSlider_, gainLabel_, "Output Gain",
                VocalDoublerProcessor::PARAM_GAIN, gainAttachment_);

    setSize(900, 500);
}

VocalDoublerEditor::~VocalDoublerEditor() {}

void VocalDoublerEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);

    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawText("Vocal Doubler", getLocalBounds().removeFromTop(50),
               juce::Justification::centred);
}

void VocalDoublerEditor::resized() {
    auto bounds = getLocalBounds().reduced(20);
    bounds.removeFromTop(50); // Title
    bounds.removeFromTop(10);

    // Sliders in a grid (3 rows x 3 columns)
    auto sliderHeight = 140;
    auto sliderWidth = getWidth() / 3 - 30;

    // Row 1: Voices, Width, Pitch Variation
    auto row1 = bounds.removeFromTop(sliderHeight);
    voicesSlider_.setBounds(row1.removeFromLeft(sliderWidth));
    row1.removeFromLeft(10);
    widthSlider_.setBounds(row1.removeFromLeft(sliderWidth));
    row1.removeFromLeft(10);
    pitchVarSlider_.setBounds(row1.removeFromLeft(sliderWidth));

    bounds.removeFromTop(10);

    // Row 2: Timing, Humanize, High Pass
    auto row2 = bounds.removeFromTop(sliderHeight);
    timingSlider_.setBounds(row2.removeFromLeft(sliderWidth));
    row2.removeFromLeft(10);
    humanizeSlider_.setBounds(row2.removeFromLeft(sliderWidth));
    row2.removeFromLeft(10);
    highPassSlider_.setBounds(row2.removeFromLeft(sliderWidth));

    bounds.removeFromTop(10);

    // Row 3: Low Pass, Mix, Output Gain
    auto row3 = bounds.removeFromTop(sliderHeight);
    lowPassSlider_.setBounds(row3.removeFromLeft(sliderWidth));
    row3.removeFromLeft(10);
    mixSlider_.setBounds(row3.removeFromLeft(sliderWidth));
    row3.removeFromLeft(10);
    gainSlider_.setBounds(row3.removeFromLeft(sliderWidth));
}
