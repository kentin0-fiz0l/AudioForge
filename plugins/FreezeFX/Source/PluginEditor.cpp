#include "PluginEditor.h"
#include "../../shared/ui/AudioForgeTheme.h"
#include <cmath>

FreezeFXEditor::FreezeFXEditor(FreezeFXProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Set window size
    setSize(500, 600);

    // Freeze Button
    freezeButton.setButtonText("FREEZE");
    freezeButton.setToggleable(true);
    addAndMakeVisible(freezeButton);

    freezeLabel.setText("Freeze", juce::dontSendNotification);
    freezeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(freezeLabel);

    // Freeze Mix Slider
    freezeMixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    freezeMixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(freezeMixSlider);

    freezeMixLabel.setText("Mix", juce::dontSendNotification);
    freezeMixLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(freezeMixLabel);

    // FFT Size Selector
    fftSizeSelector.addItem("1024", 1);
    fftSizeSelector.addItem("2048", 2);
    fftSizeSelector.addItem("4096", 3);
    fftSizeSelector.addItem("8192", 4);
    addAndMakeVisible(fftSizeSelector);

    fftSizeLabel.setText("FFT Size", juce::dontSendNotification);
    fftSizeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(fftSizeLabel);

    // Overlap Selector
    overlapSelector.addItem("2x", 1);
    overlapSelector.addItem("4x", 2);
    overlapSelector.addItem("8x", 3);
    addAndMakeVisible(overlapSelector);

    overlapLabel.setText("Overlap", juce::dontSendNotification);
    overlapLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(overlapLabel);

    // Phase Random Slider
    phaseRandomSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    phaseRandomSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(phaseRandomSlider);

    phaseRandomLabel.setText("Phase Random", juce::dontSendNotification);
    phaseRandomLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(phaseRandomLabel);

    // Phase Speed Slider
    phaseSpeedSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    phaseSpeedSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(phaseSpeedSlider);

    phaseSpeedLabel.setText("Phase Speed", juce::dontSendNotification);
    phaseSpeedLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(phaseSpeedLabel);

    // Spectral Blur Slider
    spectralBlurSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    spectralBlurSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(spectralBlurSlider);

    spectralBlurLabel.setText("Spectral Blur", juce::dontSendNotification);
    spectralBlurLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(spectralBlurLabel);

    // High Pass Slider
    highPassSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    highPassSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    addAndMakeVisible(highPassSlider);

    highPassLabel.setText("High Pass", juce::dontSendNotification);
    highPassLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(highPassLabel);

    // Low Pass Slider
    lowPassSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    lowPassSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    addAndMakeVisible(lowPassSlider);

    lowPassLabel.setText("Low Pass", juce::dontSendNotification);
    lowPassLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(lowPassLabel);

    // Configure slider ranges
    freezeMixSlider.setRange(0.0, 100.0, 1.0);
    phaseRandomSlider.setRange(0.0, 100.0, 1.0);
    phaseSpeedSlider.setRange(0.1, 10.0, 0.1);
    spectralBlurSlider.setRange(0.0, 100.0, 1.0);
    highPassSlider.setRange(20.0, 20000.0, 1.0);
    highPassSlider.setSkewFactor(0.3);
    lowPassSlider.setRange(20.0, 20000.0, 1.0);
    lowPassSlider.setSkewFactor(0.3);

    // Create THREAD-SAFE parameter attachments (this fixes the crash!)
    auto& apvts = audioProcessor.getAPVTS();

    freezeAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(
        apvts, FreezeFXProcessor::PARAM_FREEZE, freezeButton));

    freezeMixAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        apvts, FreezeFXProcessor::PARAM_FREEZE_MIX, freezeMixSlider));

    fftSizeAttachment.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(
        apvts, FreezeFXProcessor::PARAM_FFT_SIZE, fftSizeSelector));

    overlapAttachment.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(
        apvts, FreezeFXProcessor::PARAM_OVERLAP, overlapSelector));

    phaseRandomAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        apvts, FreezeFXProcessor::PARAM_PHASE_RANDOM, phaseRandomSlider));

    phaseSpeedAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        apvts, FreezeFXProcessor::PARAM_PHASE_SPEED, phaseSpeedSlider));

    spectralBlurAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        apvts, FreezeFXProcessor::PARAM_SPECTRAL_BLUR, spectralBlurSlider));

    highPassAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        apvts, FreezeFXProcessor::PARAM_HIGH_PASS, highPassSlider));

    lowPassAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        apvts, FreezeFXProcessor::PARAM_LOW_PASS, lowPassSlider));

    // Timer disabled for stability during rapid editor create/destroy cycles
    // (Caused hangs during PluginVal's "Open editor whilst processing" test)
    // startTimerHz(10);
}

FreezeFXEditor::~FreezeFXEditor()
{
    // No timer to stop (startTimerHz is disabled above)
}

void FreezeFXEditor::paint(juce::Graphics& g)
{
    using namespace AudioForge;

    // Background
    g.fillAll(Colors::Background);

    // Title bar (standardized)
    Layout::drawTitleBar(g, "AudioForge FreezeFX", Categories::Synthesizer, getWidth());

    // Section headers
    g.setFont(Typography::Subtitle);
    g.setColour(Colors::TextSecondary);
    g.drawText("FREEZE CONTROL", 20, 50, 200, 20, juce::Justification::centredLeft);
    g.drawText("PHASE EVOLUTION", 20, 200, 200, 20, juce::Justification::centredLeft);
    g.drawText("SPECTRAL PROCESSING", 20, 350, 200, 20, juce::Justification::centredLeft);
    g.drawText("FREQUENCY RANGE", 20, 480, 200, 20, juce::Justification::centredLeft);

    // Spectrum visualizer
    paintSpectrum(g, spectrumArea);
}

void FreezeFXEditor::resized()
{
    int y = 80;
    int sliderHeight = 100;
    int margin = 20;

    // Freeze button (large, centered)
    freezeButton.setBounds(150, y, 200, 60);
    y += 70;

    // Row 1: Freeze Mix, FFT Size, Overlap
    freezeMixSlider.setBounds(margin, y, sliderHeight, sliderHeight);
    freezeMixLabel.setBounds(margin, y + sliderHeight, sliderHeight, 20);

    fftSizeSelector.setBounds(margin + 150, y + 30, 100, 30);
    fftSizeLabel.setBounds(margin + 150, y, 100, 25);

    overlapSelector.setBounds(margin + 300, y + 30, 100, 30);
    overlapLabel.setBounds(margin + 300, y, 100, 25);

    y += 130;

    // Row 2: Phase Random, Phase Speed
    phaseRandomSlider.setBounds(margin, y, sliderHeight, sliderHeight);
    phaseRandomLabel.setBounds(margin, y + sliderHeight, sliderHeight, 20);

    phaseSpeedSlider.setBounds(margin + 150, y, sliderHeight, sliderHeight);
    phaseSpeedLabel.setBounds(margin + 150, y + sliderHeight, sliderHeight, 20);

    spectralBlurSlider.setBounds(margin + 300, y, sliderHeight, sliderHeight);
    spectralBlurLabel.setBounds(margin + 300, y + sliderHeight, sliderHeight, 20);

    y += 150;

    // Frequency range sliders (horizontal)
    highPassLabel.setBounds(margin, y, 80, 20);
    highPassSlider.setBounds(margin + 90, y, 350, 20);
    y += 30;

    lowPassLabel.setBounds(margin, y, 80, 20);
    lowPassSlider.setBounds(margin + 90, y, 350, 20);

    // Spectrum visualizer area (top right)
    spectrumArea = juce::Rectangle<int>(getWidth() - 240, 50, 220, 150);
}

void FreezeFXEditor::timerCallback()
{
    // REMOVED: All UI updates now handled automatically by APVTS parameter attachments
    // (Unsafe parameter access via getParameters()[index] caused race conditions)
    //
    // Parameter attachments provide automatic, thread-safe UI ↔ processor synchronization:
    // - freezeAttachment syncs freezeButton ↔ PARAM_FREEZE
    // - freezeMixAttachment syncs freezeMixSlider ↔ PARAM_FREEZE_MIX
    // - etc. for all 9 parameters
}

void FreezeFXEditor::paintSpectrum(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Background
    g.setColour(juce::Colours::black.withAlpha(0.8f));
    g.fillRect(bounds);

    // Border
    g.setColour(juce::Colours::white.withAlpha(0.3f));
    g.drawRect(bounds, 1);

    // Show freeze status instead of spectrum visualization
    bool isFrozen = false;
    try {
        isFrozen = audioProcessor.isCurrentlyFrozen();
    } catch (...) {
        // Ignore if processor not ready
    }

    // Draw status indicator
    g.setFont(14.0f);
    if (isFrozen)
    {
        g.setColour(juce::Colours::cyan);
        g.drawText("FROZEN", bounds, juce::Justification::centred, true);
    }
    else
    {
        g.setColour(juce::Colours::green.withAlpha(0.7f));
        g.drawText("PROCESSING", bounds, juce::Justification::centred, true);
    }
}
