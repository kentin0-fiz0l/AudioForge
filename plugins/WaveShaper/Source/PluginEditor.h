#pragma once

#include "PluginProcessor.h"
#include "dsp/WaveShaping.h"

class WaveShapingCurveDisplay : public juce::Component
{
public:
    WaveShapingCurveDisplay() : currentMode(0) {}

    void setShapeMode(int mode)
    {
        if (currentMode != mode)
        {
            currentMode = mode;
            repaint();
        }
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();

        // Background
        g.setColour(juce::Colour(0xff0a0a0a));
        g.fillRect(bounds);

        // Grid
        g.setColour(juce::Colour(0xff2a2a2a));
        const int gridLines = 5;
        for (int i = 0; i <= gridLines; ++i)
        {
            float x = bounds.getX() + (bounds.getWidth() * i / gridLines);
            float y = bounds.getY() + (bounds.getHeight() * i / gridLines);
            g.drawLine(x, bounds.getY(), x, bounds.getBottom(), 1.0f);
            g.drawLine(bounds.getX(), y, bounds.getRight(), y, 1.0f);
        }

        // Center cross
        g.setColour(juce::Colour(0xff4a4a4a));
        g.drawLine(bounds.getCentreX(), bounds.getY(), bounds.getCentreX(), bounds.getBottom(), 1.5f);
        g.drawLine(bounds.getX(), bounds.getCentreY(), bounds.getRight(), bounds.getCentreY(), 1.5f);

        // Plot waveshaping curve
        juce::Path curvePath;
        const int numPoints = 200;

        for (int i = 0; i < numPoints; ++i)
        {
            // Input from -3 to +3 for better visualization
            float input = -3.0f + (i * 6.0f / (numPoints - 1));
            float output = AudioForge::DSP::WaveShaping::shape(input, currentMode);

            // Map to display coordinates
            float x = bounds.getCentreX() + (input / 3.0f) * (bounds.getWidth() * 0.45f);
            float y = bounds.getCentreY() - (output / 1.5f) * (bounds.getHeight() * 0.45f);

            if (i == 0)
                curvePath.startNewSubPath(x, y);
            else
                curvePath.lineTo(x, y);
        }

        // Draw curve
        g.setColour(juce::Colour(0xff00aaff));
        g.strokePath(curvePath, juce::PathStrokeType(2.0f));

        // Draw reference line (unity gain)
        juce::Path refPath;
        refPath.startNewSubPath(bounds.getX(), bounds.getBottom());
        refPath.lineTo(bounds.getRight(), bounds.getY());
        g.setColour(juce::Colour(0xff666666));
        g.strokePath(refPath, juce::PathStrokeType(1.0f, juce::PathStrokeType::PathStrokeType::mitered,
                                                     juce::PathStrokeType::EndCapStyle::butt));
    }

private:
    int currentMode;
};

class WaveShaperAudioProcessorEditor : public juce::AudioProcessorEditor,
                                       private juce::Timer
{
public:
    WaveShaperAudioProcessorEditor (WaveShaperAudioProcessor&);
    ~WaveShaperAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    WaveShaperAudioProcessor& audioProcessor;

    // Parameter controls
    juce::Slider driveSlider;
    juce::Label driveLabel;

    juce::Slider mixSlider;
    juce::Label mixLabel;

    juce::Slider toneSlider;
    juce::Label toneLabel;

    juce::ComboBox shapeSelector;
    juce::Label shapeLabel;

    juce::Slider outputGainSlider;
    juce::Label outputGainLabel;

    // Metering
    juce::Label inputMeterLabel;
    juce::Label outputMeterLabel;

    // Curve visualization
    WaveShapingCurveDisplay curveDisplay;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveShaperAudioProcessorEditor)
};
