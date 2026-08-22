#pragma once

#include "PluginProcessor.h"
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_dsp/juce_dsp.h>

/**
 * Harmonic analyzer component - displays harmonic content in real-time
 */
class HarmonicAnalyzer : public juce::Component, private juce::Timer
{
public:
    HarmonicAnalyzer(SaturationAudioProcessor& p) : processor(p)
    {
        // Start analyzer update timer (10 Hz for visual smoothness)
        startTimerHz(10);
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();

        // Background
        g.setColour(juce::Colour(0xff0a0a0a));
        g.fillRect(bounds);

        // Grid lines
        g.setColour(juce::Colour(0xff2a2a2a));
        for (int i = 1; i <= 8; ++i)
        {
            float x = bounds.getX() + (bounds.getWidth() * i / 9.0f);
            g.drawLine(x, bounds.getY(), x, bounds.getBottom(), 1.0f);
        }

        // Draw harmonic bars
        const int numHarmonics = 8;
        const float barWidth = bounds.getWidth() / (numHarmonics + 1) * 0.8f;
        const float spacing = bounds.getWidth() / (numHarmonics + 1);

        for (int i = 0; i < numHarmonics; ++i)
        {
            float level = harmonicLevels[i];
            float barHeight = level * bounds.getHeight() * 0.9f;

            float x = bounds.getX() + spacing * (i + 1) - barWidth * 0.5f;
            float y = bounds.getBottom() - barHeight;

            // Color code: Even harmonics (warm) = orange, Odd harmonics (bright) = cyan
            bool isEven = (i == 1 || i == 3 || i == 5 || i == 7);  // 2nd, 4th, 6th, 8th
            juce::Colour barColour = isEven ? juce::Colour(0xffff8800) : juce::Colour(0xff00aaff);

            // Apply alpha based on level
            barColour = barColour.withAlpha(0.3f + level * 0.7f);

            g.setColour(barColour);
            g.fillRect(x, y, barWidth, barHeight);

            // Harmonic labels
            g.setColour(juce::Colours::grey);
            g.setFont(10.0f);
            juce::String label = "H" + juce::String(i + 1);
            g.drawText(label, juce::Rectangle<float>(x, bounds.getBottom() - 15.0f, barWidth, 15.0f),
                      juce::Justification::centred);
        }

        // Title
        g.setColour(juce::Colours::white);
        g.setFont(12.0f);
        g.drawText("Harmonic Content", bounds.removeFromTop(20), juce::Justification::centred);

        // Legend
        g.setFont(9.0f);
        g.setColour(juce::Colour(0xffff8800));
        g.fillRect(bounds.getX() + 5, 25.0f, 10.0f, 10.0f);
        g.setColour(juce::Colours::white);
        g.drawText("Even (Warm)", bounds.getX() + 20, 22.0f, 80.0f, 15.0f,
                  juce::Justification::centredLeft);

        g.setColour(juce::Colour(0xff00aaff));
        g.fillRect(bounds.getX() + 110, 25.0f, 10.0f, 10.0f);
        g.setColour(juce::Colours::white);
        g.drawText("Odd (Bright)", bounds.getX() + 125, 22.0f, 80.0f, 15.0f,
                  juce::Justification::centredLeft);
    }

private:
    void timerCallback() override
    {
        // Update harmonic levels (placeholder - will be calculated from FFT later)
        // For now, simulate based on saturation mode
        int mode = processor.modeParam->getIndex();
        float drive = processor.driveParam->get() / 100.0f;

        // Simple simulation of harmonic content by mode
        switch (mode)
        {
            case 0: // Tape - even harmonics
                harmonicLevels[1] = drive * 0.7f;  // H2
                harmonicLevels[3] = drive * 0.4f;  // H4
                harmonicLevels[5] = drive * 0.2f;  // H6
                break;
            case 1: // Tube - mixed with even emphasis
                harmonicLevels[1] = drive * 0.8f;  // H2
                harmonicLevels[2] = drive * 0.5f;  // H3
                harmonicLevels[3] = drive * 0.3f;  // H4
                break;
            case 2: // Transistor - odd harmonics
                harmonicLevels[2] = drive * 0.7f;  // H3
                harmonicLevels[4] = drive * 0.5f;  // H5
                harmonicLevels[6] = drive * 0.3f;  // H7
                break;
            case 3: // Transformer - very gentle, mostly H2
                harmonicLevels[1] = drive * 0.5f;  // H2
                harmonicLevels[3] = drive * 0.2f;  // H4
                break;
            case 4: // Diode - strong odd and even
                harmonicLevels[1] = drive * 0.6f;  // H2
                harmonicLevels[2] = drive * 0.7f;  // H3
                harmonicLevels[4] = drive * 0.4f;  // H5
                break;
        }

        // Decay unused harmonics
        for (int i = 0; i < 8; ++i)
            harmonicLevels[i] *= 0.95f;  // Smooth decay

        repaint();
    }

    SaturationAudioProcessor& processor;
    float harmonicLevels[8] = {0.0f};
};

/**
 * Main plugin editor with controls and harmonic analyzer
 */
class SaturationAudioProcessorEditor : public juce::AudioProcessorEditor,
                                       private juce::Timer
{
public:
    SaturationAudioProcessorEditor (SaturationAudioProcessor&);
    ~SaturationAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    SaturationAudioProcessor& audioProcessor;

    // Parameter controls
    juce::Slider driveSlider;
    juce::Label driveLabel;

    juce::Slider mixSlider;
    juce::Label mixLabel;

    juce::Slider toneLowSlider;
    juce::Label toneLowLabel;

    juce::Slider toneHighSlider;
    juce::Label toneHighLabel;

    juce::ComboBox modeSelector;
    juce::Label modeLabel;

    juce::ComboBox oversamplingSelector;
    juce::Label oversamplingLabel;

    // Metering
    juce::Label inputMeterLabel;
    juce::Label outputMeterLabel;

    // Harmonic analyzer
    HarmonicAnalyzer harmonicAnalyzer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SaturationAudioProcessorEditor)
};
