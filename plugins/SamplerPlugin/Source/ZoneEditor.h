#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "KeyZone.h"

/**
 * Zone Editor Component
 *
 * Visual piano keyboard for editing key zones.
 * Shows colored rectangles for each zone, allows clicking to select,
 * dragging edges to resize, and right-click menu for operations.
 *
 * Features:
 * - 88-key piano keyboard (A0 to C8)
 * - Visual zone representation with colors
 * - Click to select zone
 * - Drag edges to resize zone
 * - Right-click menu (Add Zone, Delete Zone, Set Root Note)
 * - Snap to note grid
 */
class ZoneEditor : public juce::Component
{
public:
    ZoneEditor(KeyZoneMap& zoneMap);
    ~ZoneEditor() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseMove(const juce::MouseEvent& e) override;

    //==============================================================================
    // Zone selection
    int getSelectedZoneIndex() const { return selectedZoneIndex; }
    void setSelectedZone(int index);

    //==============================================================================
    // Listener for zone changes
    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual void zoneSelectionChanged(int zoneIndex) = 0;
        virtual void zoneRangeChanged(int zoneIndex, int newMin, int newMax) = 0;
        virtual void zoneAdded(int zoneIndex) = 0;
        virtual void zoneDeleted(int zoneIndex) = 0;
    };

    void addListener(Listener* listener) { listeners.add(listener); }
    void removeListener(Listener* listener) { listeners.remove(listener); }

private:
    KeyZoneMap& keyZoneMap;
    juce::ListenerList<Listener> listeners;

    // UI state
    int selectedZoneIndex = -1;
    int hoveredNote = -1;
    bool isDraggingEdge = false;
    bool isDraggingLeftEdge = false;
    int dragStartNote = -1;

    // Piano keyboard layout
    static constexpr int LOWEST_NOTE = 21;   // A0
    static constexpr int HIGHEST_NOTE = 108; // C8
    static constexpr int NUM_KEYS = HIGHEST_NOTE - LOWEST_NOTE + 1;

    // Colors for zones (cycle through)
    static const juce::Colour ZONE_COLORS[];
    static constexpr int NUM_ZONE_COLORS = 8;

    //==============================================================================
    // Layout calculations
    float getNotePosition(int midiNote) const;
    int getNoteFromPosition(float x) const;
    juce::Rectangle<float> getNoteRectangle(int midiNote) const;
    bool isBlackKey(int midiNote) const;
    juce::Colour getZoneColor(int zoneIndex) const;

    //==============================================================================
    // Zone operations
    void addZoneAtNote(int midiNote);
    void deleteSelectedZone();
    void setRootNoteForSelectedZone(int midiNote);

    //==============================================================================
    // Hit testing
    int findZoneAtNote(int midiNote) const;
    bool isNearZoneEdge(int zoneIndex, int midiNote, bool& isLeftEdge) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ZoneEditor)
};
