#pragma once

#include "PluginProcessor.h"

//==============================================================================
/**
 * LFO Visualization Component
 *
 * Displays the current LFO waveform and phase positions for left and right channels
 */
class LFOVisualizer : public juce::Component, private juce::Timer
{
public:
    LFOVisualizer(ChorusFlangerAudioProcessor& p)
        : processor(p)
    {
        startTimerHz(60); // 60 FPS
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();

        // Draw background
        g.setColour(juce::Colour(0xff2a2a2a));
        g.fillRoundedRectangle(bounds, 4.0f);

        // Draw grid
        g.setColour(juce::Colour(0xff404040));
        g.drawHorizontalLine(static_cast<int>(bounds.getCentreY()),
                           bounds.getX(), bounds.getRight());

        // Draw LFO waveform
        juce::Path waveform;
        const int numPoints = 200;
        const float width = bounds.getWidth();
        const float height = bounds.getHeight();
        const float centerY = bounds.getCentreY();

        for (int i = 0; i < numPoints; ++i)
        {
            float phase = static_cast<float>(i) / numPoints;
            float value = getLFOValue(phase);

            float x = bounds.getX() + phase * width;
            float y = centerY - value * (height * 0.4f);

            if (i == 0)
                waveform.startNewSubPath(x, y);
            else
                waveform.lineTo(x, y);
        }

        g.setColour(juce::Colour(0xff00aaff));
        g.strokePath(waveform, juce::PathStrokeType(2.0f));

        // Draw phase indicators
        float leftPhase = processor.getCurrentLFOPhase();
        float rightPhase = processor.getStereoLFOPhase();

        // Left channel (orange)
        float leftX = bounds.getX() + leftPhase * width;
        float leftY = centerY - getLFOValue(leftPhase) * (height * 0.4f);
        g.setColour(juce::Colour(0xffff8800));
        g.fillEllipse(leftX - 5.0f, leftY - 5.0f, 10.0f, 10.0f);

        // Right channel (cyan)
        float rightX = bounds.getX() + rightPhase * width;
        float rightY = centerY - getLFOValue(rightPhase) * (height * 0.4f);
        g.setColour(juce::Colour(0xff00ffff));
        g.fillEllipse(rightX - 5.0f, rightY - 5.0f, 10.0f, 10.0f);
    }

private:
    ChorusFlangerAudioProcessor& processor;

    void timerCallback() override
    {
        repaint();
    }

    float getLFOValue(float phase)
    {
        int waveform = processor.waveformParam->getIndex();
        if (waveform == 0) // Sine
        {
            return std::sin(phase * 2.0f * juce::MathConstants<float>::pi);
        }
        else // Triangle
        {
            return 4.0f * std::abs(phase - 0.5f) - 1.0f;
        }
    }
};

//==============================================================================
/**
 * Chorus/Flanger Plugin Editor
 */
class ChorusFlangerAudioProcessorEditor : public juce::AudioProcessorEditor,
                                           private juce::Timer
{
public:
    ChorusFlangerAudioProcessorEditor (ChorusFlangerAudioProcessor&);
    ~ChorusFlangerAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    ChorusFlangerAudioProcessor& audioProcessor;

    // UI Components
    juce::Slider rateSlider;
    juce::Label rateLabel;

    juce::Slider depthSlider;
    juce::Label depthLabel;

    juce::Slider feedbackSlider;
    juce::Label feedbackLabel;

    juce::Slider mixSlider;
    juce::Label mixLabel;

    juce::Slider stereoWidthSlider;
    juce::Label stereoWidthLabel;

    juce::ComboBox modeSelector;
    juce::Label modeLabel;

    juce::ComboBox waveformSelector;
    juce::Label waveformLabel;

    juce::Label inputMeterLabel;
    juce::Label outputMeterLabel;

    LFOVisualizer lfoVisualizer;

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChorusFlangerAudioProcessorEditor)
};
