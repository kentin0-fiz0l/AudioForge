#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "../../shared/ui/AudioForgeTheme.h"

PluginEditor::PluginEditor(PluginProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Setup all sliders
    setupSlider(wtPositionSlider, wtPositionLabel, "Position", wtPositionAttachment, "wtPosition");

    setupSlider(filterCutoffSlider, filterCutoffLabel, "Cutoff", filterCutoffAttachment, "filterCutoff");
    setupSlider(filterResSlider, filterResLabel, "Resonance", filterResAttachment, "filterRes");
    setupSlider(filterTypeSlider, filterTypeLabel, "Type", filterTypeAttachment, "filterType");
    setupSlider(filterEnvSlider, filterEnvLabel, "Env Amount", filterEnvAttachment, "filterEnv");

    setupSlider(ampAttackSlider, ampAttackLabel, "Attack", ampAttackAttachment, "ampAttack");
    setupSlider(ampDecaySlider, ampDecayLabel, "Decay", ampDecayAttachment, "ampDecay");
    setupSlider(ampSustainSlider, ampSustainLabel, "Sustain", ampSustainAttachment, "ampSustain");
    setupSlider(ampReleaseSlider, ampReleaseLabel, "Release", ampReleaseAttachment, "ampRelease");

    setupSlider(filtAttackSlider, filtAttackLabel, "Attack", filtAttackAttachment, "filtAttack");
    setupSlider(filtDecaySlider, filtDecayLabel, "Decay", filtDecayAttachment, "filtDecay");
    setupSlider(filtSustainSlider, filtSustainLabel, "Sustain", filtSustainAttachment, "filtSustain");
    setupSlider(filtReleaseSlider, filtReleaseLabel, "Release", filtReleaseAttachment, "filtRelease");

    // Customize filter type slider to show labels
    filterTypeSlider.textFromValueFunction = [](double value) {
        int type = static_cast<int>(value);
        switch (type) {
            case 0: return juce::String("LP");
            case 1: return juce::String("HP");
            case 2: return juce::String("BP");
            case 3: return juce::String("Notch");
            default: return juce::String(type);
        }
    };

    setSize(900, 550);
}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::setupSlider(juce::Slider& slider, juce::Label& label,
                               const juce::String& labelText,
                               std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment,
                               const juce::String& parameterID)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(slider);

    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.attachToComponent(&slider, false);
    addAndMakeVisible(label);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), parameterID, slider);
}

void PluginEditor::paint(juce::Graphics& g)
{
    using namespace AudioForge;

    // Background
    g.fillAll(Colors::Background);

    // Title bar (standardized)
    Layout::drawTitleBar(g, "AudioForge Wavetable Synth", Categories::Synthesizer, getWidth());

    // Section backgrounds
    auto wavetableArea = juce::Rectangle<int>(20, 70, 200, 180);
    auto filterArea = juce::Rectangle<int>(240, 70, 640, 180);
    auto ampEnvArea = juce::Rectangle<int>(20, 270, 430, 260);
    auto filtEnvArea = juce::Rectangle<int>(470, 270, 410, 260);

    g.setColour(juce::Colour(0xff2a2a2a));
    g.fillRoundedRectangle(wavetableArea.toFloat(), 8.0f);
    g.fillRoundedRectangle(filterArea.toFloat(), 8.0f);
    g.fillRoundedRectangle(ampEnvArea.toFloat(), 8.0f);
    g.fillRoundedRectangle(filtEnvArea.toFloat(), 8.0f);

    // Section labels
    g.setColour(juce::Colours::lightblue);
    g.setFont(18.0f);
    g.drawText("Wavetable", wavetableArea.getX(), wavetableArea.getY() + 5, wavetableArea.getWidth(), 25, juce::Justification::centred);
    g.drawText("Filter", filterArea.getX(), filterArea.getY() + 5, filterArea.getWidth(), 25, juce::Justification::centred);
    g.drawText("Amplitude Envelope", ampEnvArea.getX(), ampEnvArea.getY() + 5, ampEnvArea.getWidth(), 25, juce::Justification::centred);
    g.drawText("Filter Envelope", filtEnvArea.getX(), filtEnvArea.getY() + 5, filtEnvArea.getWidth(), 25, juce::Justification::centred);
}

void PluginEditor::resized()
{
    // Wavetable section
    wtPositionSlider.setBounds(60, 120, 100, 120);

    // Filter section
    filterCutoffSlider.setBounds(260, 120, 100, 120);
    filterResSlider.setBounds(380, 120, 100, 120);
    filterTypeSlider.setBounds(500, 120, 100, 120);
    filterEnvSlider.setBounds(620, 120, 100, 120);

    // Amplitude envelope
    ampAttackSlider.setBounds(40, 320, 90, 110);
    ampDecaySlider.setBounds(145, 320, 90, 110);
    ampSustainSlider.setBounds(250, 320, 90, 110);
    ampReleaseSlider.setBounds(355, 320, 90, 110);

    // Filter envelope
    filtAttackSlider.setBounds(485, 320, 90, 110);
    filtDecaySlider.setBounds(585, 320, 90, 110);
    filtSustainSlider.setBounds(685, 320, 90, 110);
    filtReleaseSlider.setBounds(785, 320, 90, 110);
}
