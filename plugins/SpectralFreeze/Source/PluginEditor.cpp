#include "PluginEditor.h"

SpectralFreezeEditor::SpectralFreezeEditor(SpectralFreezeProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Set window size (expanded for Phase 4 presets)
    setSize(600, 540);

    // Freeze Button
    freezeButton.setButtonText("FREEZE");
    freezeButton.setToggleState(false, juce::dontSendNotification);
    freezeButton.setTooltip("Capture and hold the current spectrum indefinitely");
    freezeButton.onClick = [this]() {
        auto* param = audioProcessor.getParameters()[0];
        param->setValueNotifyingHost(freezeButton.getToggleState() ? 1.0f : 0.0f);
    };
    addAndMakeVisible(freezeButton);

    freezeLabel.setText("Spectral Freeze", juce::dontSendNotification);
    freezeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(freezeLabel);

    // Blur Slider
    blurSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    blurSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    blurSlider.setRange(0.0, 100.0, 1.0);
    blurSlider.setValue(0.0);
    blurSlider.setTooltip("Gaussian smoothing across frequency bins (0-100%)");
    blurSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[1];
        param->setValueNotifyingHost(blurSlider.getValue() / 100.0f);
    };
    addAndMakeVisible(blurSlider);

    blurLabel.setText("Blur", juce::dontSendNotification);
    blurLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(blurLabel);

    // Low-Cut Slider
    lowCutSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    lowCutSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    lowCutSlider.setRange(20.0, 20000.0, 1.0);
    lowCutSlider.setSkewFactor(0.3);
    lowCutSlider.setValue(20.0);
    lowCutSlider.setTooltip("High-pass filter frequency (20-20000 Hz)");
    lowCutSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[2];
        param->setValueNotifyingHost((lowCutSlider.getValue() - 20.0) / 19980.0);
    };
    addAndMakeVisible(lowCutSlider);

    lowCutLabel.setText("Low Cut", juce::dontSendNotification);
    lowCutLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(lowCutLabel);

    // High-Cut Slider
    highCutSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    highCutSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    highCutSlider.setRange(20.0, 20000.0, 1.0);
    highCutSlider.setSkewFactor(0.3);
    highCutSlider.setValue(20000.0);
    highCutSlider.setTooltip("Low-pass filter frequency (20-20000 Hz)");
    highCutSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[3];
        param->setValueNotifyingHost((highCutSlider.getValue() - 20.0) / 19980.0);
    };
    addAndMakeVisible(highCutSlider);

    highCutLabel.setText("High Cut", juce::dontSendNotification);
    highCutLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(highCutLabel);

    // Dry/Wet Slider
    dryWetSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    dryWetSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    dryWetSlider.setRange(0.0, 100.0, 1.0);
    dryWetSlider.setValue(100.0);
    dryWetSlider.setTooltip("Blend between dry (0%) and wet (100%) signal");
    dryWetSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[4];
        param->setValueNotifyingHost(dryWetSlider.getValue() / 100.0f);
    };
    addAndMakeVisible(dryWetSlider);

    dryWetLabel.setText("Dry/Wet", juce::dontSendNotification);
    dryWetLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(dryWetLabel);

    // Phase 2: Advanced Effect Controls

    // Stretch Slider
    stretchSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    stretchSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    stretchSlider.setRange(0.0, 100.0, 1.0);
    stretchSlider.setValue(0.0);
    stretchSlider.setTooltip("Time-stretch the frozen spectrum (1x to 5x slower)");
    stretchSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[5];
        param->setValueNotifyingHost(stretchSlider.getValue() / 100.0f);
    };
    addAndMakeVisible(stretchSlider);

    stretchLabel.setText("Stretch", juce::dontSendNotification);
    stretchLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(stretchLabel);

    // Shift Slider
    shiftSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    shiftSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    shiftSlider.setRange(-100.0, 100.0, 1.0);
    shiftSlider.setValue(0.0);
    shiftSlider.setTooltip("Shift frequencies up/down (±100 bins, ~2 octaves)");
    shiftSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[6];
        param->setValueNotifyingHost((shiftSlider.getValue() + 100.0) / 200.0);
    };
    addAndMakeVisible(shiftSlider);

    shiftLabel.setText("Shift", juce::dontSendNotification);
    shiftLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(shiftLabel);

    // Morph Slider
    morphSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    morphSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    morphSlider.setRange(0.0, 100.0, 1.0);
    morphSlider.setValue(0.0);
    morphSlider.setTooltip("Blend between live and frozen spectrum (0-100%)");
    morphSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[7];
        param->setValueNotifyingHost(morphSlider.getValue() / 100.0f);
    };
    addAndMakeVisible(morphSlider);

    morphLabel.setText("Morph", juce::dontSendNotification);
    morphLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(morphLabel);

    // Gate Slider
    gateSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gateSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    gateSlider.setRange(0.0, 100.0, 1.0);
    gateSlider.setValue(0.0);
    gateSlider.setTooltip("Silence bins below threshold (relative to max)");
    gateSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[8];
        param->setValueNotifyingHost(gateSlider.getValue() / 100.0f);
    };
    addAndMakeVisible(gateSlider);

    gateLabel.setText("Gate", juce::dontSendNotification);
    gateLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(gateLabel);

    // Randomize Slider
    randomizeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    randomizeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    randomizeSlider.setRange(0.0, 100.0, 1.0);
    randomizeSlider.setValue(0.0);
    randomizeSlider.setTooltip("Randomize phase for granular texture (0-100%)");
    randomizeSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[9];
        param->setValueNotifyingHost(randomizeSlider.getValue() / 100.0f);
    };
    addAndMakeVisible(randomizeSlider);

    randomizeLabel.setText("Randomize", juce::dontSendNotification);
    randomizeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(randomizeLabel);

    // Initialize spectrogram history buffer
    int fftSize = audioProcessor.spectralProcessor.getFFTSize();
    spectrogramHistory.resize(SPECTROGRAM_HISTORY_SIZE);
    for (auto& spectrum : spectrogramHistory)
        spectrum.resize(fftSize / 2 + 1, 0.0f);

    // Start 60 FPS visualization timer
    startTimerHz(60);

    // Phase 4: Preset Controls

    // Save Preset Button
    savePresetButton.setButtonText("Save");
    savePresetButton.setTooltip("Save current settings as 'quick-save' preset");
    savePresetButton.onClick = [this]() {
        audioProcessor.savePreset("quick-save");
    };
    addAndMakeVisible(savePresetButton);

    // Load Preset Button
    loadPresetButton.setButtonText("Load");
    loadPresetButton.setTooltip("Load 'quick-save' preset");
    loadPresetButton.onClick = [this]() {
        audioProcessor.loadPreset("quick-save");
    };
    addAndMakeVisible(loadPresetButton);

    // Factory Presets ComboBox
    factoryPresetsCombo.addItem("Init", 1);
    factoryPresetsCombo.addItem("Frozen Blur", 2);
    factoryPresetsCombo.addItem("Spectral Stretch", 3);
    factoryPresetsCombo.addItem("Frequency Shifter", 4);
    factoryPresetsCombo.addItem("Spectral Gate", 5);
    factoryPresetsCombo.addItem("Granular Texture", 6);
    factoryPresetsCombo.setSelectedId(1);
    factoryPresetsCombo.setTooltip("Load factory preset showcasing different effects");
    factoryPresetsCombo.onChange = [this]() {
        int index = factoryPresetsCombo.getSelectedId() - 1;
        audioProcessor.loadFactoryPreset(index);
    };
    addAndMakeVisible(factoryPresetsCombo);

    // Presets Label
    presetsLabel.setText("Presets:", juce::dontSendNotification);
    presetsLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(presetsLabel);
}

SpectralFreezeEditor::~SpectralFreezeEditor()
{
    stopTimer();
}

void SpectralFreezeEditor::paint(juce::Graphics& g)
{
    // Background
    g.fillAll(juce::Colours::darkslategrey);

    // Title
    g.setColour(juce::Colours::white);
    g.setFont(28.0f);
    g.drawText("SpectralFreeze", 0, 10, getWidth(), 40, juce::Justification::centred);

    // Subtitle
    g.setFont(14.0f);
    g.setColour(juce::Colours::lightgrey);
    g.drawText("Production-Ready Spectral Manipulation", 0, 45, getWidth(), 20, juce::Justification::centred);

    // Spectrogram visualization area
    juce::Rectangle<int> spectrogramBounds(10, 70, getWidth() - 20, 80);

    // Draw spectrogram background
    g.setColour(juce::Colours::black);
    g.fillRect(spectrogramBounds);

    // Draw spectrogram and overlays
    paintSpectrogram(g, spectrogramBounds);
    paintFrozenSpectrumOverlay(g, spectrogramBounds);
    paintFrequencyAxis(g, spectrogramBounds);

    // Freeze status indicator
    if (audioProcessor.spectralProcessor.isFrozen())
    {
        g.setColour(juce::Colours::red.withAlpha(0.3f));
        g.fillRect(spectrogramBounds);
        g.setColour(juce::Colours::red);
        g.setFont(16.0f);
        g.drawText("FROZEN", spectrogramBounds, juce::Justification::topRight);
    }
}

void SpectralFreezeEditor::resized()
{
    int y = 165;  // Move down to make room for spectrogram
    int centerX = getWidth() / 2;

    // Freeze button (large, centered)
    freezeButton.setBounds(centerX - 75, y, 150, 40);
    freezeLabel.setBounds(centerX - 75, y + 45, 150, 20);

    // Phase 4: Preset Controls (below freeze button)
    int presetY = y + 70;
    presetsLabel.setBounds(10, presetY, 60, 25);
    factoryPresetsCombo.setBounds(70, presetY, 180, 25);
    savePresetButton.setBounds(260, presetY, 80, 25);
    loadPresetButton.setBounds(350, presetY, 80, 25);

    // Phase 1 sliders (row 1)
    y = 290;  // Moved down for spectrogram + presets
    int sliderSize = 70;
    int spacing = 100;

    blurSlider.setBounds(centerX - spacing * 2, y, sliderSize, sliderSize);
    blurLabel.setBounds(centerX - spacing * 2, y + sliderSize, sliderSize, 20);

    lowCutSlider.setBounds(centerX - spacing, y, sliderSize, sliderSize);
    lowCutLabel.setBounds(centerX - spacing, y + sliderSize, sliderSize, 20);

    highCutSlider.setBounds(centerX, y, sliderSize, sliderSize);
    highCutLabel.setBounds(centerX, y + sliderSize, sliderSize, 20);

    dryWetSlider.setBounds(centerX + spacing, y, sliderSize, sliderSize);
    dryWetLabel.setBounds(centerX + spacing, y + sliderSize, sliderSize, 20);

    stretchSlider.setBounds(centerX + spacing * 2, y, sliderSize, sliderSize);
    stretchLabel.setBounds(centerX + spacing * 2, y + sliderSize, sliderSize, 20);

    // Phase 2 sliders (row 2)
    y = 400;  // Moved down for spectrogram + presets

    shiftSlider.setBounds(centerX - spacing * 2, y, sliderSize, sliderSize);
    shiftLabel.setBounds(centerX - spacing * 2, y + sliderSize, sliderSize, 20);

    morphSlider.setBounds(centerX - spacing, y, sliderSize, sliderSize);
    morphLabel.setBounds(centerX - spacing, y + sliderSize, sliderSize, 20);

    gateSlider.setBounds(centerX, y, sliderSize, sliderSize);
    gateLabel.setBounds(centerX, y + sliderSize, sliderSize, 20);

    randomizeSlider.setBounds(centerX + spacing, y, sliderSize, sliderSize);
    randomizeLabel.setBounds(centerX + spacing, y + sliderSize, sliderSize, 20);
}

//==============================================================================
// Visualization Implementation

void SpectralFreezeEditor::timerCallback()
{
    // Update spectrogram history with current spectrum
    const auto& currentSpectrum = audioProcessor.spectralProcessor.getCurrentMagnitudeSpectrum();
    if (!currentSpectrum.empty())
    {
        spectrogramHistory[spectrogramWriteIndex] = currentSpectrum;
        spectrogramWriteIndex = (spectrogramWriteIndex + 1) % SPECTROGRAM_HISTORY_SIZE;
    }

    // Trigger repaint for smooth 60 FPS visualization
    repaint();
}

void SpectralFreezeEditor::paintSpectrogram(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Draw spectrogram as rolling waterfall (time vs. frequency)
    int numBins = spectrogramHistory[0].size();
    int width = bounds.getWidth();
    int height = bounds.getHeight();

    // Draw each time slice as a vertical line
    for (int x = 0; x < width; ++x)
    {
        // Map x to history buffer index (newest on right)
        int historyIndex = (spectrogramWriteIndex - (width - x) + SPECTROGRAM_HISTORY_SIZE) % SPECTROGRAM_HISTORY_SIZE;
        const auto& spectrum = spectrogramHistory[historyIndex];

        // Draw frequency bins as pixels in this vertical line
        for (int y = 0; y < height; ++y)
        {
            // Map y to frequency bin (high frequencies at top)
            float binFraction = 1.0f - ((float)y / height);
            int bin = (int)(binFraction * numBins);
            bin = juce::jlimit(0, numBins - 1, bin);

            // Map magnitude to color intensity (log scale)
            float magnitude = spectrum[bin];
            float dB = 20.0f * std::log10(std::max(magnitude, 1e-6f));
            float intensity = juce::jmap(dB, -60.0f, 0.0f, 0.0f, 1.0f);
            intensity = juce::jlimit(0.0f, 1.0f, intensity);

            // Color gradient: blue (low) -> green -> yellow -> red (high)
            juce::Colour color;
            if (intensity < 0.33f)
                color = juce::Colours::blue.interpolatedWith(juce::Colours::green, intensity * 3.0f);
            else if (intensity < 0.66f)
                color = juce::Colours::green.interpolatedWith(juce::Colours::yellow, (intensity - 0.33f) * 3.0f);
            else
                color = juce::Colours::yellow.interpolatedWith(juce::Colours::red, (intensity - 0.66f) * 3.0f);

            // Draw pixel as 1x1 rectangle
            g.setColour(color);
            g.fillRect(bounds.getX() + x, bounds.getY() + y, 1, 1);
        }
    }
}

void SpectralFreezeEditor::paintFrozenSpectrumOverlay(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Draw frozen spectrum as overlay line
    if (!audioProcessor.spectralProcessor.isFrozen())
        return;

    const auto& frozenSpectrum = audioProcessor.spectralProcessor.getFrozenMagnitudeSpectrum();
    if (frozenSpectrum.empty())
        return;

    int numBins = frozenSpectrum.size();
    int width = bounds.getWidth();
    int height = bounds.getHeight();

    juce::Path frozenPath;
    bool firstPoint = true;

    for (int x = 0; x < width; ++x)
    {
        // Map x to frequency bin (log scale for better visualization)
        float binFraction = (float)x / width;
        binFraction = std::pow(binFraction, 0.5f);  // Square root for log-like spacing
        int bin = (int)(binFraction * numBins);
        bin = juce::jlimit(0, numBins - 1, bin);

        // Map magnitude to y position
        float magnitude = frozenSpectrum[bin];
        float dB = 20.0f * std::log10(std::max(magnitude, 1e-6f));
        float y = juce::jmap(dB, -60.0f, 0.0f, (float)height, 0.0f);
        y = juce::jlimit(0.0f, (float)height, y);

        if (firstPoint)
        {
            frozenPath.startNewSubPath(bounds.getX() + x, bounds.getY() + y);
            firstPoint = false;
        }
        else
        {
            frozenPath.lineTo(bounds.getX() + x, bounds.getY() + y);
        }
    }

    // Draw frozen spectrum overlay in bright red
    g.setColour(juce::Colours::red.withAlpha(0.7f));
    g.strokePath(frozenPath, juce::PathStrokeType(2.0f));
}

void SpectralFreezeEditor::paintFrequencyAxis(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Draw frequency axis labels (left side)
    g.setColour(juce::Colours::lightgrey.withAlpha(0.5f));
    g.setFont(10.0f);

    // Label positions (logarithmic spacing)
    std::vector<std::pair<float, juce::String>> labels = {
        {0.0f, "20k"},
        {0.33f, "2k"},
        {0.66f, "200"},
        {1.0f, "20"}
    };

    for (const auto& [fraction, text] : labels)
    {
        int y = bounds.getY() + (int)(fraction * bounds.getHeight());
        g.drawText(text, bounds.getX() - 30, y - 6, 25, 12, juce::Justification::right);

        // Draw grid line
        g.setColour(juce::Colours::white.withAlpha(0.1f));
        g.drawHorizontalLine(y, (float)bounds.getX(), (float)bounds.getRight());
    }
}
