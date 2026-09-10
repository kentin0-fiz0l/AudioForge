#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <functional>

class XYPadComponent : public juce::Component {
public:
    XYPadComponent(juce::AudioProcessorValueTreeState& apvts);
    ~XYPadComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;

    // Callbacks for MIDI CC generation
    std::function<void(int cc, float value)> onCCChange;

    // Get current normalized positions (0.0 - 1.0)
    float getXPosition() const { return xPosition_; }
    float getYPosition() const { return yPosition_; }

private:
    juce::AudioProcessorValueTreeState& apvts_;

    float xPosition_ = 0.5f;  // Normalized 0.0-1.0
    float yPosition_ = 0.5f;  // Normalized 0.0-1.0
    bool isDragging_ = false;

    void updatePosition(const juce::MouseEvent& e);
    void sendCCMessages();
    juce::Point<float> getNormalizedPosition(const juce::MouseEvent& e) const;
};
