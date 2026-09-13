#include "PluginEditor.h"

VocalHarmonizerEditor::VocalHarmonizerEditor(VocalHarmonizerProcessor& p)
    : AudioProcessorEditor(&p), processor_(p) {

    auto& apvts = processor_.getAPVTS();

    // Setup slider helper
    auto setupSlider = [&](juce::Slider& slider, juce::Label& label,
                          const juce::String& labelText, const juce::String& paramID,
                          std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment) {
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
        addAndMakeVisible(slider);

        label.setText(labelText, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.attachToComponent(&slider, false);
        addAndMakeVisible(label);

        attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, paramID, slider);
    };

    // Voice count (discrete)
    voicesSlider_.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    voicesSlider_.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    addAndMakeVisible(voicesSlider_);
    voicesLabel_.setText("Voices", juce::dontSendNotification);
    voicesLabel_.setJustificationType(juce::Justification::centred);
    voicesLabel_.attachToComponent(&voicesSlider_, false);
    addAndMakeVisible(voicesLabel_);
    voicesAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, VocalHarmonizerProcessor::PARAM_VOICES, voicesSlider_);

    // Voice pitch sliders
    setupSlider(voice1PitchSlider_, voice1PitchLabel_, "Voice 1 Pitch",
                VocalHarmonizerProcessor::PARAM_VOICE1_PITCH, voice1PitchAttachment_);
    setupSlider(voice2PitchSlider_, voice2PitchLabel_, "Voice 2 Pitch",
                VocalHarmonizerProcessor::PARAM_VOICE2_PITCH, voice2PitchAttachment_);
    setupSlider(voice3PitchSlider_, voice3PitchLabel_, "Voice 3 Pitch",
                VocalHarmonizerProcessor::PARAM_VOICE3_PITCH, voice3PitchAttachment_);
    setupSlider(voice4PitchSlider_, voice4PitchLabel_, "Voice 4 Pitch",
                VocalHarmonizerProcessor::PARAM_VOICE4_PITCH, voice4PitchAttachment_);

    // Voice pan sliders
    setupSlider(voice1PanSlider_, voice1PanLabel_, "Voice 1 Pan",
                VocalHarmonizerProcessor::PARAM_VOICE1_PAN, voice1PanAttachment_);
    setupSlider(voice2PanSlider_, voice2PanLabel_, "Voice 2 Pan",
                VocalHarmonizerProcessor::PARAM_VOICE2_PAN, voice2PanAttachment_);
    setupSlider(voice3PanSlider_, voice3PanLabel_, "Voice 3 Pan",
                VocalHarmonizerProcessor::PARAM_VOICE3_PAN, voice3PanAttachment_);
    setupSlider(voice4PanSlider_, voice4PanLabel_, "Voice 4 Pan",
                VocalHarmonizerProcessor::PARAM_VOICE4_PAN, voice4PanAttachment_);

    // Scale selector
    scaleBox_.addItem("Chromatic", 1);
    scaleBox_.addItem("Major", 2);
    scaleBox_.addItem("Minor", 3);
    scaleBox_.addItem("Dorian", 4);
    scaleBox_.addItem("Phrygian", 5);
    scaleBox_.addItem("Lydian", 6);
    scaleBox_.addItem("Mixolydian", 7);
    scaleBox_.addItem("Aeolian", 8);
    scaleBox_.addItem("Locrian", 9);
    scaleBox_.addItem("Harmonic Minor", 10);
    scaleBox_.addItem("Melodic Minor", 11);
    scaleBox_.addItem("Pentatonic", 12);
    addAndMakeVisible(scaleBox_);

    scaleLabel_.setText("Scale", juce::dontSendNotification);
    scaleLabel_.setJustificationType(juce::Justification::centred);
    scaleLabel_.attachToComponent(&scaleBox_, false);
    addAndMakeVisible(scaleLabel_);

    scaleAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts, VocalHarmonizerProcessor::PARAM_SCALE, scaleBox_);

    // Other controls
    setupSlider(formantSlider_, formantLabel_, "Formant Shift",
                VocalHarmonizerProcessor::PARAM_FORMANT, formantAttachment_);
    setupSlider(highPassSlider_, highPassLabel_, "High Pass",
                VocalHarmonizerProcessor::PARAM_HIGHPASS, highPassAttachment_);
    setupSlider(lowPassSlider_, lowPassLabel_, "Low Pass",
                VocalHarmonizerProcessor::PARAM_LOWPASS, lowPassAttachment_);
    setupSlider(mixSlider_, mixLabel_, "Mix",
                VocalHarmonizerProcessor::PARAM_MIX, mixAttachment_);
    setupSlider(gainSlider_, gainLabel_, "Output Gain",
                VocalHarmonizerProcessor::PARAM_GAIN, gainAttachment_);

    setSize(1000, 600);
}

VocalHarmonizerEditor::~VocalHarmonizerEditor() {}

void VocalHarmonizerEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);

    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawText("Vocal Harmonizer", getLocalBounds().removeFromTop(50),
               juce::Justification::centred);
}

void VocalHarmonizerEditor::resized() {
    auto bounds = getLocalBounds().reduced(20);
    bounds.removeFromTop(50); // Title
    bounds.removeFromTop(10);

    auto sliderHeight = 120;
    auto sliderWidth = getWidth() / 5 - 25;

    // Row 1: Voices + Voice Pitch controls (5 controls)
    auto row1 = bounds.removeFromTop(sliderHeight);
    voicesSlider_.setBounds(row1.removeFromLeft(sliderWidth));
    row1.removeFromLeft(10);
    voice1PitchSlider_.setBounds(row1.removeFromLeft(sliderWidth));
    row1.removeFromLeft(10);
    voice2PitchSlider_.setBounds(row1.removeFromLeft(sliderWidth));
    row1.removeFromLeft(10);
    voice3PitchSlider_.setBounds(row1.removeFromLeft(sliderWidth));
    row1.removeFromLeft(10);
    voice4PitchSlider_.setBounds(row1.removeFromLeft(sliderWidth));

    bounds.removeFromTop(10);

    // Row 2: Voice Pan controls (4 controls) + Scale
    auto row2 = bounds.removeFromTop(sliderHeight);
    voice1PanSlider_.setBounds(row2.removeFromLeft(sliderWidth));
    row2.removeFromLeft(10);
    voice2PanSlider_.setBounds(row2.removeFromLeft(sliderWidth));
    row2.removeFromLeft(10);
    voice3PanSlider_.setBounds(row2.removeFromLeft(sliderWidth));
    row2.removeFromLeft(10);
    voice4PanSlider_.setBounds(row2.removeFromLeft(sliderWidth));
    row2.removeFromLeft(10);

    // Scale combo box (smaller, in remaining space)
    auto scaleArea = row2.removeFromLeft(sliderWidth);
    scaleArea.removeFromTop(30); // Space for label
    scaleBox_.setBounds(scaleArea.removeFromTop(30));

    bounds.removeFromTop(10);

    // Row 3: Formant, Filters, Mix, Gain (5 controls)
    auto row3 = bounds.removeFromTop(sliderHeight);
    formantSlider_.setBounds(row3.removeFromLeft(sliderWidth));
    row3.removeFromLeft(10);
    highPassSlider_.setBounds(row3.removeFromLeft(sliderWidth));
    row3.removeFromLeft(10);
    lowPassSlider_.setBounds(row3.removeFromLeft(sliderWidth));
    row3.removeFromLeft(10);
    mixSlider_.setBounds(row3.removeFromLeft(sliderWidth));
    row3.removeFromLeft(10);
    gainSlider_.setBounds(row3.removeFromLeft(sliderWidth));
}
