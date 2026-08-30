#include "PluginEditor.h"

GranularEngineEditor::GranularEngineEditor(GranularEngineProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Set window size
    setSize(800, 600);

    // Start timer for UI updates (60 FPS)
    startTimerHz(60);

    // Grain Size Slider
    grainSizeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    grainSizeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    grainSizeSlider.setRange(10.0, 500.0, 1.0);
    grainSizeSlider.setValue(50.0);
    addAndMakeVisible(grainSizeSlider);

    grainSizeLabel.setText("Grain Size (ms)", juce::dontSendNotification);
    grainSizeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(grainSizeLabel);

    // Dry/Wet Slider
    dryWetSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    dryWetSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    dryWetSlider.setRange(0.0, 100.0, 1.0);
    dryWetSlider.setValue(100.0);
    addAndMakeVisible(dryWetSlider);

    dryWetLabel.setText("Dry/Wet (%)", juce::dontSendNotification);
    dryWetLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(dryWetLabel);

    // Window Type ComboBox
    windowTypeCombo.addItem("Hann", 1);
    windowTypeCombo.addItem("Gaussian", 2);
    windowTypeCombo.addItem("Triangle", 3);
    windowTypeCombo.addItem("Tukey", 4);
    windowTypeCombo.addItem("Blackman", 5);
    windowTypeCombo.addItem("Kaiser", 6);
    windowTypeCombo.setSelectedId(1);
    addAndMakeVisible(windowTypeCombo);

    windowTypeLabel.setText("Window", juce::dontSendNotification);
    windowTypeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(windowTypeLabel);

    // Window Shape Slider
    windowShapeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    windowShapeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    windowShapeSlider.setRange(0.0, 1.0, 0.01);
    windowShapeSlider.setValue(0.5);
    addAndMakeVisible(windowShapeSlider);

    windowShapeLabel.setText("Shape", juce::dontSendNotification);
    windowShapeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(windowShapeLabel);

    // Grain Density Slider
    grainDensitySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    grainDensitySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    grainDensitySlider.setRange(1.0, 100.0, 0.1);
    grainDensitySlider.setValue(10.0);
    addAndMakeVisible(grainDensitySlider);

    grainDensityLabel.setText("Density (g/s)", juce::dontSendNotification);
    grainDensityLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(grainDensityLabel);

    // Time Stretch Slider
    timeStretchSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    timeStretchSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    timeStretchSlider.setRange(0.25, 4.0, 0.01);
    timeStretchSlider.setSkewFactor(0.5);
    timeStretchSlider.setValue(1.0);
    addAndMakeVisible(timeStretchSlider);

    timeStretchLabel.setText("Time Stretch", juce::dontSendNotification);
    timeStretchLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(timeStretchLabel);

    // Position Slider
    positionSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    positionSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    positionSlider.setRange(0.0, 1.0, 0.01);
    positionSlider.setValue(0.5);
    addAndMakeVisible(positionSlider);

    positionLabel.setText("Position", juce::dontSendNotification);
    positionLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(positionLabel);

    // Pitch Shift Slider
    pitchShiftSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    pitchShiftSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    pitchShiftSlider.setRange(-24.0, 24.0, 0.1);
    pitchShiftSlider.setValue(0.0);
    addAndMakeVisible(pitchShiftSlider);

    pitchShiftLabel.setText("Pitch (st)", juce::dontSendNotification);
    pitchShiftLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(pitchShiftLabel);

    // Spray Slider
    spraySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    spraySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    spraySlider.setRange(0.0, 100.0, 1.0);
    spraySlider.setValue(0.0);
    addAndMakeVisible(spraySlider);

    sprayLabel.setText("Spray (%)", juce::dontSendNotification);
    sprayLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(sprayLabel);

    // Reverse Slider
    reverseSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    reverseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    reverseSlider.setRange(0.0, 100.0, 1.0);
    reverseSlider.setValue(0.0);
    addAndMakeVisible(reverseSlider);

    reverseLabel.setText("Reverse (%)", juce::dontSendNotification);
    reverseLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(reverseLabel);

    // Stereo Width Slider
    stereoWidthSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    stereoWidthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    stereoWidthSlider.setRange(0.0, 200.0, 1.0);
    stereoWidthSlider.setValue(100.0);
    addAndMakeVisible(stereoWidthSlider);

    stereoWidthLabel.setText("Width (%)", juce::dontSendNotification);
    stereoWidthLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(stereoWidthLabel);

    // Parameter connections
    grainSizeSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[0];
        param->setValueNotifyingHost((grainSizeSlider.getValue() - 10.0f) / 490.0f);
    };

    grainDensitySlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[1];
        param->setValueNotifyingHost((grainDensitySlider.getValue() - 1.0f) / 99.0f);
    };

    timeStretchSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[2];
        param->setValueNotifyingHost((timeStretchSlider.getValue() - 0.25f) / 3.75f);
    };

    positionSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[3];
        param->setValueNotifyingHost(positionSlider.getValue());
    };

    pitchShiftSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[4];
        param->setValueNotifyingHost((pitchShiftSlider.getValue() + 24.0f) / 48.0f);
    };

    spraySlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[5];
        param->setValueNotifyingHost(spraySlider.getValue() / 100.0f);
    };

    reverseSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[6];
        param->setValueNotifyingHost(reverseSlider.getValue() / 100.0f);
    };

    stereoWidthSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[7];
        param->setValueNotifyingHost(stereoWidthSlider.getValue() / 200.0f);
    };

    dryWetSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[8];
        param->setValueNotifyingHost(dryWetSlider.getValue() / 100.0f);
    };

    windowTypeCombo.onChange = [this]() {
        auto* param = audioProcessor.getParameters()[9];
        param->setValueNotifyingHost((windowTypeCombo.getSelectedId() - 1) / 5.0f);
    };

    windowShapeSlider.onValueChange = [this]() {
        auto* param = audioProcessor.getParameters()[10];
        param->setValueNotifyingHost(windowShapeSlider.getValue());
    };
}

GranularEngineEditor::~GranularEngineEditor()
{
    stopTimer();
}

void GranularEngineEditor::paint(juce::Graphics& g)
{
    // Background
    g.fillAll(juce::Colours::darkslategrey);

    // Title
    g.setColour(juce::Colours::white);
    g.setFont(28.0f);
    g.drawText("GranularEngine", 0, 10, getWidth(), 40, juce::Justification::centred);

    // Subtitle
    g.setFont(14.0f);
    g.setColour(juce::Colours::lightgrey);
    g.drawText("Phase 5: UI & Visualization", 0, 45, getWidth(), 20, juce::Justification::centred);

    // Paint visualizations
    paintWaveform(g, waveformBounds);
    paintWindowPreview(g, windowPreviewBounds);
    paintGrainActivity(g, activityBounds);

    // Section headers
    g.setFont(16.0f);
    g.setColour(juce::Colours::white);
    g.drawText("GRAIN PARAMETERS", 20, 320, 300, 30, juce::Justification::centredLeft);
    g.drawText("CREATIVE CONTROLS", 20, 460, 300, 30, juce::Justification::centredLeft);
}

void GranularEngineEditor::resized()
{
    // Visualization bounds
    int margin = 20;
    waveformBounds = juce::Rectangle<int>(margin, 70, 560, 120);
    windowPreviewBounds = juce::Rectangle<int>(600, 70, 180, 120);
    activityBounds = juce::Rectangle<int>(margin, 200, 760, 100);

    // Control bounds
    int y = 350;
    int sliderSize = 85;
    int spacing = 110;

    // Row 1: Grain parameters
    grainSizeSlider.setBounds(margin, y, sliderSize, sliderSize);
    grainSizeLabel.setBounds(margin, y + sliderSize, sliderSize, 20);

    grainDensitySlider.setBounds(margin + spacing, y, sliderSize, sliderSize);
    grainDensityLabel.setBounds(margin + spacing, y + sliderSize, sliderSize, 20);

    timeStretchSlider.setBounds(margin + spacing * 2, y, sliderSize, sliderSize);
    timeStretchLabel.setBounds(margin + spacing * 2, y + sliderSize, sliderSize, 20);

    dryWetSlider.setBounds(margin + spacing * 3, y, sliderSize, sliderSize);
    dryWetLabel.setBounds(margin + spacing * 3, y + sliderSize, sliderSize, 20);

    // Row 2: Creative controls
    y = 490;

    pitchShiftSlider.setBounds(margin, y, sliderSize, sliderSize);
    pitchShiftLabel.setBounds(margin, y + sliderSize, sliderSize, 20);

    spraySlider.setBounds(margin + spacing, y, sliderSize, sliderSize);
    sprayLabel.setBounds(margin + spacing, y + sliderSize, sliderSize, 20);

    reverseSlider.setBounds(margin + spacing * 2, y, sliderSize, sliderSize);
    reverseLabel.setBounds(margin + spacing * 2, y + sliderSize, sliderSize, 20);

    stereoWidthSlider.setBounds(margin + spacing * 3, y, sliderSize, sliderSize);
    stereoWidthLabel.setBounds(margin + spacing * 3, y + sliderSize, sliderSize, 20);

    // Row 3: Window controls (smaller section)
    int windowY = 350;
    int rightMargin = 590;

    windowTypeCombo.setBounds(rightMargin, windowY, 100, 25);
    windowTypeLabel.setBounds(rightMargin, windowY - 20, 100, 20);

    windowShapeSlider.setBounds(rightMargin, windowY + 35, sliderSize, sliderSize);
    windowShapeLabel.setBounds(rightMargin, windowY + 35 + sliderSize, sliderSize, 20);

    // Position slider (horizontal at bottom)
    y = 570;
    positionLabel.setBounds(margin, y, 80, 20);
    positionSlider.setBounds(margin + 90, y, 560, 20);
}

//==============================================================================
// Timer Callback

void GranularEngineEditor::timerCallback()
{
    // Repaint visualization areas only
    repaint(waveformBounds);
    repaint(windowPreviewBounds);
    repaint(activityBounds);
}

//==============================================================================
// Visualization Methods

void GranularEngineEditor::paintWaveform(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    if (bounds.isEmpty())
        return;

    // Background
    g.setColour(juce::Colour(0xff1a1a1a));
    g.fillRect(bounds);

    // Border
    g.setColour(juce::Colours::grey);
    g.drawRect(bounds, 1);

    // Title
    g.setColour(juce::Colours::white);
    g.setFont(12.0f);
    g.drawText("WAVEFORM (10s Buffer)", bounds.getX() + 5, bounds.getY() + 5,
               bounds.getWidth() - 10, 20, juce::Justification::centredLeft);

    // Draw waveform placeholder (we'll add actual audio data later)
    g.setColour(juce::Colour(0xff00ff88));
    int centerY = bounds.getCentreY();

    // Simple sine wave visualization for now
    juce::Path waveformPath;
    for (int x = 0; x < bounds.getWidth(); ++x)
    {
        float phase = (float)x / bounds.getWidth() * 4.0f * juce::MathConstants<float>::pi;
        float sample = std::sin(phase) * 0.3f;
        float y = centerY + sample * bounds.getHeight() * 0.4f;

        if (x == 0)
            waveformPath.startNewSubPath(bounds.getX() + x, y);
        else
            waveformPath.lineTo(bounds.getX() + x, y);
    }
    g.strokePath(waveformPath, juce::PathStrokeType(1.0f));

    // Draw grain position marker
    float position = positionSlider.getValue();
    int markerX = bounds.getX() + (int)(position * bounds.getWidth());

    g.setColour(juce::Colour(0xffff6600));
    g.drawLine((float)markerX, (float)bounds.getY(),
               (float)markerX, (float)bounds.getBottom(), 2.0f);

    // Draw spray zone
    float spray = spraySlider.getValue() / 100.0f;
    int sprayWidth = (int)(spray * bounds.getWidth() * 0.5f);

    g.setColour(juce::Colour(0x40ff6600));
    g.fillRect(markerX - sprayWidth, bounds.getY(),
               sprayWidth * 2, bounds.getHeight());
}

void GranularEngineEditor::paintWindowPreview(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    if (bounds.isEmpty())
        return;

    // Background
    g.setColour(juce::Colour(0xff1a1a1a));
    g.fillRect(bounds);

    // Border
    g.setColour(juce::Colours::grey);
    g.drawRect(bounds, 1);

    // Title
    g.setColour(juce::Colours::white);
    g.setFont(12.0f);
    g.drawText("WINDOW SHAPE", bounds.getX() + 5, bounds.getY() + 5,
               bounds.getWidth() - 10, 20, juce::Justification::centredLeft);

    // Draw window envelope preview
    g.setColour(juce::Colour(0xff00aaff));

    juce::Path windowPath;
    int numPoints = 200;
    int windowType = windowTypeCombo.getSelectedId() - 1;
    float windowShape = windowShapeSlider.getValue();

    for (int i = 0; i < numPoints; ++i)
    {
        float x = (float)i / (numPoints - 1);
        float y = 0.0f;

        // Calculate window value based on type
        if (windowType == 0)  // Hann
        {
            y = 0.5f * (1.0f - std::cos(2.0f * juce::MathConstants<float>::pi * x));
        }
        else if (windowType == 1)  // Gaussian
        {
            float center = 0.5f;
            float widthFactor = 0.1f + windowShape * 0.9f;
            float sigma = 1.0f / (6.0f / widthFactor);
            float distance = (x - center) / sigma;
            y = std::exp(-0.5f * distance * distance);
        }
        else if (windowType == 2)  // Triangle
        {
            if (x < 0.5f)
                y = x * 2.0f;
            else
                y = 2.0f - x * 2.0f;
        }
        else if (windowType == 3)  // Tukey
        {
            float alpha = windowShape;
            if (x < alpha / 2.0f)
            {
                float phase = 2.0f * x / alpha;
                y = 0.5f * (1.0f - std::cos(juce::MathConstants<float>::pi * phase));
            }
            else if (x > 1.0f - alpha / 2.0f)
            {
                float phase = 2.0f * (1.0f - x) / alpha;
                y = 0.5f * (1.0f - std::cos(juce::MathConstants<float>::pi * phase));
            }
            else
            {
                y = 1.0f;
            }
        }
        else if (windowType == 4)  // Blackman
        {
            const float a0 = 0.42f;
            const float a1 = 0.5f;
            const float a2 = 0.08f;
            y = a0 - a1 * std::cos(2.0f * juce::MathConstants<float>::pi * x)
                   + a2 * std::cos(4.0f * juce::MathConstants<float>::pi * x);
        }
        else if (windowType == 5)  // Kaiser
        {
            float beta = windowShape * 10.0f;

            // Modified Bessel I0 approximation
            auto besselI0 = [](float x) -> float
            {
                float sum = 1.0f;
                float term = 1.0f;
                for (int k = 1; k < 20; ++k)
                {
                    float kf = (float)k;
                    term *= (x * x) / (4.0f * kf * kf);
                    sum += term;
                    if (term < 1e-6f) break;
                }
                return sum;
            };

            float denominator = besselI0(beta);
            float n = 2.0f * x - 1.0f;  // Map x from [0,1] to [-1,1]
            float arg = beta * std::sqrt(1.0f - n * n);
            y = besselI0(arg) / denominator;
        }

        // Convert to screen coordinates
        float screenX = bounds.getX() + 10 + x * (bounds.getWidth() - 20);
        float screenY = bounds.getBottom() - 10 - y * (bounds.getHeight() - 30);

        if (i == 0)
            windowPath.startNewSubPath(screenX, screenY);
        else
            windowPath.lineTo(screenX, screenY);
    }

    g.strokePath(windowPath, juce::PathStrokeType(2.0f));
}

void GranularEngineEditor::paintGrainActivity(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    if (bounds.isEmpty())
        return;

    // Background
    g.setColour(juce::Colour(0xff1a1a1a));
    g.fillRect(bounds);

    // Border
    g.setColour(juce::Colours::grey);
    g.drawRect(bounds, 1);

    // Title
    g.setColour(juce::Colours::white);
    g.setFont(12.0f);
    g.drawText("GRAIN ACTIVITY", bounds.getX() + 5, bounds.getY() + 5,
               bounds.getWidth() - 10, 20, juce::Justification::centredLeft);

    // Draw grain density meter
    float density = grainDensitySlider.getValue();
    int meterWidth = (int)((density / 100.0f) * (bounds.getWidth() - 20));

    g.setColour(juce::Colour(0xff00ff88));
    g.fillRect(bounds.getX() + 10, bounds.getY() + 30, meterWidth, 20);

    // Density text
    g.setColour(juce::Colours::white);
    g.setFont(10.0f);
    g.drawText(juce::String(density, 1) + " grains/sec",
               bounds.getX() + 10, bounds.getY() + 30, 200, 20,
               juce::Justification::centredLeft);

    // Draw parameter info
    int infoY = bounds.getY() + 60;
    g.setFont(10.0f);

    // Grain size
    g.setColour(juce::Colours::lightgrey);
    g.drawText("Grain Size: " + juce::String(grainSizeSlider.getValue(), 1) + " ms",
               bounds.getX() + 10, infoY, 200, 15, juce::Justification::centredLeft);

    // Pitch shift
    g.drawText("Pitch: " + juce::String(pitchShiftSlider.getValue(), 1) + " st",
               bounds.getX() + 220, infoY, 200, 15, juce::Justification::centredLeft);

    // Time stretch
    g.drawText("Time: " + juce::String(timeStretchSlider.getValue(), 2) + "x",
               bounds.getX() + 430, infoY, 200, 15, juce::Justification::centredLeft);

    // Reverse probability
    infoY += 20;
    g.drawText("Reverse: " + juce::String(reverseSlider.getValue(), 0) + "%",
               bounds.getX() + 10, infoY, 200, 15, juce::Justification::centredLeft);

    // Stereo width
    g.drawText("Width: " + juce::String(stereoWidthSlider.getValue(), 0) + "%",
               bounds.getX() + 220, infoY, 200, 15, juce::Justification::centredLeft);

    // Dry/Wet
    g.drawText("Dry/Wet: " + juce::String(dryWetSlider.getValue(), 0) + "%",
               bounds.getX() + 430, infoY, 200, 15, juce::Justification::centredLeft);
}

//==============================================================================
// Mouse Interaction

void GranularEngineEditor::mouseDown(const juce::MouseEvent& event)
{
    // Check if click is in waveform bounds
    if (waveformBounds.contains(event.getPosition()))
    {
        // Set position based on click location
        float newPosition = (event.x - waveformBounds.getX()) / (float)waveformBounds.getWidth();
        newPosition = juce::jlimit(0.0f, 1.0f, newPosition);

        positionSlider.setValue(newPosition);

        // Trigger parameter update
        auto* param = audioProcessor.getParameters()[3];
        param->setValueNotifyingHost(newPosition);
    }
}

void GranularEngineEditor::mouseDrag(const juce::MouseEvent& event)
{
    // Same as mouseDown - allow dragging to adjust position
    if (waveformBounds.contains(event.getPosition()))
    {
        float newPosition = (event.x - waveformBounds.getX()) / (float)waveformBounds.getWidth();
        newPosition = juce::jlimit(0.0f, 1.0f, newPosition);

        positionSlider.setValue(newPosition);

        auto* param = audioProcessor.getParameters()[3];
        param->setValueNotifyingHost(newPosition);
    }
}
