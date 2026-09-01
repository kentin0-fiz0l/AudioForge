#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include "Sample.h"
#include <memory>

/**
 * Waveform Display Component
 *
 * Displays audio waveform with editable loop markers.
 * Shows zoomed view of loaded sample with drag handles for loop points.
 *
 * Features:
 * - Audio waveform visualization
 * - Loop start/end markers (draggable)
 * - Zoom controls (horizontal + vertical)
 * - Sample info display (length, sample rate, channels)
 */
class WaveformDisplay : public juce::Component
{
public:
    WaveformDisplay();
    ~WaveformDisplay() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override;

    //==============================================================================
    // Sample management
    void setSample(std::shared_ptr<Sample> sample);
    std::shared_ptr<Sample> getSample() const { return currentSample; }

    //==============================================================================
    // Loop point editing
    void setLoopStart(double seconds);
    void setLoopEnd(double seconds);
    double getLoopStart() const;
    double getLoopEnd() const;

    //==============================================================================
    // Listener for loop point changes
    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual void loopPointsChanged(double loopStartSec, double loopEndSec) = 0;
    };

    void addListener(Listener* listener) { listeners.add(listener); }
    void removeListener(Listener* listener) { listeners.remove(listener); }

private:
    std::shared_ptr<Sample> currentSample;
    juce::ListenerList<Listener> listeners;

    // Zoom state
    float zoomLevel = 1.0f;          // Horizontal zoom (1.0 = fit to view)
    float verticalZoom = 1.0f;       // Vertical zoom (1.0 = auto-scale)
    float scrollPosition = 0.0f;     // Scroll offset (0.0-1.0)

    // Loop marker dragging
    bool isDraggingLoopStart = false;
    bool isDraggingLoopEnd = false;

    //==============================================================================
    // Drawing helpers
    void drawWaveform(juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawLoopMarkers(juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawSampleInfo(juce::Graphics& g, juce::Rectangle<float> bounds);

    //==============================================================================
    // Coordinate conversion
    float secondsToX(double seconds, juce::Rectangle<float> bounds) const;
    double xToSeconds(float x, juce::Rectangle<float> bounds) const;
    juce::Rectangle<float> getLoopMarkerRect(double seconds, juce::Rectangle<float> bounds, bool isStart) const;

    //==============================================================================
    // Hit testing
    bool isPointInLoopMarker(juce::Point<float> point, juce::Rectangle<float> bounds,
                            bool& isStart) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay)
};
