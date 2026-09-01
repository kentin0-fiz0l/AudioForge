#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "KeyZone.h"
#include "SampleLibrary.h"

/**
 * Velocity Layer Editor Component
 *
 * Displays and edits velocity layers for a selected zone.
 * Shows velocity ranges with sample assignments, allows adding/removing layers.
 *
 * Features:
 * - Visual velocity range display (0-127 vertical scale)
 * - Layer rectangles showing sample assignment
 * - Drag boundaries to adjust velocity splits
 * - Drag samples from browser onto layers
 * - Add/remove layer buttons
 */
class VelocityLayerEditor : public juce::Component
{
public:
    VelocityLayerEditor();
    ~VelocityLayerEditor() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;

    //==============================================================================
    // Zone selection
    void setZone(KeyZone* zone, SampleLibrary* library);
    void clearZone();

    //==============================================================================
    // Layer management
    void addLayer(int sampleIndex);
    void removeLayer(int layerIndex);

    //==============================================================================
    // Listener for layer changes
    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual void velocityLayerChanged(int layerIndex) = 0;
    };

    void addListener(Listener* listener) { listeners.add(listener); }
    void removeListener(Listener* listener) { listeners.remove(listener); }

private:
    KeyZone* currentZone = nullptr;
    SampleLibrary* sampleLibrary = nullptr;
    juce::ListenerList<Listener> listeners;

    // UI state
    int selectedLayerIndex = -1;
    bool isDraggingBoundary = false;
    int draggedBoundaryIndex = -1;

    //==============================================================================
    // Layout calculations
    float velocityToY(float velocity) const;
    float yToVelocity(float y) const;
    juce::Rectangle<float> getLayerRect(int layerIndex) const;

    //==============================================================================
    // Hit testing
    int findLayerAtY(float y) const;
    bool isNearBoundary(float y, int& boundaryIndex) const;

    //==============================================================================
    // Drawing helpers
    void drawVelocityScale(juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawLayers(juce::Graphics& g, juce::Rectangle<float> bounds);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VelocityLayerEditor)
};
