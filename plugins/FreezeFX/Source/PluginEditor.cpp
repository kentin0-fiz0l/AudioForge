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

    // Connect sliders to parameters using lambda listeners
    freezeButton.onClick = [this]() {
        auto* param = audioProcessor.getParameters()[0];
        param->setValueNotifyingHost(freezeButton.getToggleState() ? 1.0f : 0.0f);
    };

    freezeMixSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[1];
        param->setValueNotifyingHost(freezeMixSlider.getValue() / 100.0f);
    };
    freezeMixSlider.setRange(0.0, 100.0, 1.0);
    freezeMixSlider.setValue(100.0);

    fftSizeSelector.onChange = [this]() {
        auto* param = audioProcessor.getParameters()[2];
        param->setValueNotifyingHost((fftSizeSelector.getSelectedId() - 1) / 3.0f);
    };

    overlapSelector.onChange = [this]() {
        auto* param = audioProcessor.getParameters()[3];
        param->setValueNotifyingHost((overlapSelector.getSelectedId() - 1) / 2.0f);
    };

    phaseRandomSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[4];
        param->setValueNotifyingHost(phaseRandomSlider.getValue() / 100.0f);
    };
    phaseRandomSlider.setRange(0.0, 100.0, 1.0);
    phaseRandomSlider.setValue(50.0);

    phaseSpeedSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[5];
        param->setValueNotifyingHost((phaseSpeedSlider.getValue() - 0.1f) / 9.9f);
    };
    phaseSpeedSlider.setRange(0.1, 10.0, 0.1);
    phaseSpeedSlider.setValue(1.0);

    spectralBlurSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[6];
        param->setValueNotifyingHost(spectralBlurSlider.getValue() / 100.0f);
    };
    spectralBlurSlider.setRange(0.0, 100.0, 1.0);
    spectralBlurSlider.setValue(0.0);

    highPassSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[7];
        // Log scale for frequency: 20Hz to 20kHz
        float value = highPassSlider.getValue();
        param->setValueNotifyingHost((std::log(value / 20.0f) / std::log(1000.0f)));
    };
    highPassSlider.setRange(20.0, 20000.0, 1.0);
    highPassSlider.setSkewFactor(0.3);
    highPassSlider.setValue(20.0);

    lowPassSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[8];
        // Log scale for frequency: 20Hz to 20kHz
        float value = lowPassSlider.getValue();
        param->setValueNotifyingHost((std::log(value / 20.0f) / std::log(1000.0f)));
    };
    lowPassSlider.setRange(20.0, 20000.0, 1.0);
    lowPassSlider.setSkewFactor(0.3);
    lowPassSlider.setValue(20000.0);

    // Timer disabled for stability during rapid editor create/destroy cycles
    // (Caused hangs during PluginVal's "Open editor whilst processing" test)
    // startTimerHz(10);
}

FreezeFXEditor::~FreezeFXEditor()
{
    stopTimer();
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
    // Update UI from parameters
    auto& params = audioProcessor.getParameters();

    // Update freeze button
    auto* freezeParam = dynamic_cast<juce::AudioParameterBool*>(params[0]);
    if (freezeParam && freezeButton.getToggleState() != freezeParam->get())
        freezeButton.setToggleState(freezeParam->get(), juce::dontSendNotification);

    // Update sliders from parameters
    auto* freezeMixParam = dynamic_cast<juce::AudioParameterFloat*>(params[1]);
    if (freezeMixParam)
        freezeMixSlider.setValue(freezeMixParam->get() * 100.0f, juce::dontSendNotification);

    // Spectrum visualization disabled for stability
    // (Real-time spectrum updates caused threading conflicts during validation)
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
