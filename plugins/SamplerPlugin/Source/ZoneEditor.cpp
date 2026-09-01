#include "ZoneEditor.h"
#include <juce_audio_basics/juce_audio_basics.h>

// Zone colors (8 distinct colors that look good)
const juce::Colour ZoneEditor::ZONE_COLORS[] = {
    juce::Colour(0xff4a9eff),  // Blue
    juce::Colour(0xff50c878),  // Green
    juce::Colour(0xffff6b6b),  // Red
    juce::Colour(0xffffb347),  // Orange
    juce::Colour(0xffc77dff),  // Purple
    juce::Colour(0xff00d4aa),  // Teal
    juce::Colour(0xffffde7d),  // Yellow
    juce::Colour(0xffff85a1)   // Pink
};

ZoneEditor::ZoneEditor(KeyZoneMap& zoneMap)
    : keyZoneMap(zoneMap)
{
    setSize(800, 120);
}

ZoneEditor::~ZoneEditor()
{
}

void ZoneEditor::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Background
    g.fillAll(juce::Colour(0xff2a2a2a));

    // Draw piano keys
    for (int note = LOWEST_NOTE; note <= HIGHEST_NOTE; ++note)
    {
        auto keyRect = getNoteRectangle(note);
        bool isBlack = isBlackKey(note);

        // White keys
        if (!isBlack)
        {
            g.setColour(juce::Colours::white);
            g.fillRect(keyRect);
            g.setColour(juce::Colours::black);
            g.drawRect(keyRect, 0.5f);
        }
    }

    // Draw zones (behind black keys)
    for (int i = 0; i < keyZoneMap.getNumZones(); ++i)
    {
        const auto& zone = keyZoneMap.getZones()[i];
        auto zoneColor = getZoneColor(i).withAlpha(0.6f);

        if (i == selectedZoneIndex)
            zoneColor = zoneColor.brighter(0.3f);

        // Draw zone rectangle
        float startX = getNotePosition(zone.noteMin);
        float endX = getNotePosition(zone.noteMax + 1);
        juce::Rectangle<float> zoneRect(startX, 0.0f, endX - startX, bounds.getHeight());

        g.setColour(zoneColor);
        g.fillRect(zoneRect);

        // Draw zone border
        g.setColour(zoneColor.brighter(0.5f));
        g.drawRect(zoneRect, 2.0f);

        // Draw root note marker
        if (zone.rootNote >= zone.noteMin && zone.rootNote <= zone.noteMax)
        {
            float rootX = getNotePosition(zone.rootNote);
            float rootWidth = getNotePosition(zone.rootNote + 1) - rootX;
            juce::Rectangle<float> rootRect(rootX, bounds.getHeight() - 8.0f, rootWidth, 8.0f);

            g.setColour(juce::Colours::yellow);
            g.fillRect(rootRect);
        }
    }

    // Draw black keys (on top)
    for (int note = LOWEST_NOTE; note <= HIGHEST_NOTE; ++note)
    {
        if (isBlackKey(note))
        {
            auto keyRect = getNoteRectangle(note);
            g.setColour(juce::Colours::black);
            g.fillRect(keyRect);
        }
    }

    // Draw hover highlight
    if (hoveredNote >= LOWEST_NOTE && hoveredNote <= HIGHEST_NOTE)
    {
        auto hoverRect = getNoteRectangle(hoveredNote);
        g.setColour(juce::Colours::white.withAlpha(0.2f));
        g.fillRect(hoverRect);
    }

    // Draw labels (note names at octave boundaries)
    g.setColour(juce::Colours::grey);
    g.setFont(10.0f);
    for (int note = LOWEST_NOTE; note <= HIGHEST_NOTE; ++note)
    {
        // Draw label at C notes
        if ((note % 12) == 0)
        {
            int octave = (note / 12) - 1;
            juce::String label = "C" + juce::String(octave);
            float x = getNotePosition(note);
            g.drawText(label, x, bounds.getHeight() - 15.0f, 20.0f, 12.0f,
                      juce::Justification::centredLeft, false);
        }
    }
}

void ZoneEditor::resized()
{
}

void ZoneEditor::mouseDown(const juce::MouseEvent& e)
{
    int clickedNote = getNoteFromPosition(e.position.x);
    if (clickedNote < LOWEST_NOTE || clickedNote > HIGHEST_NOTE)
        return;

    // Right-click: show menu
    if (e.mods.isPopupMenu())
    {
        juce::PopupMenu menu;
        menu.addItem(1, "Add Zone at Note " + juce::MidiMessage::getMidiNoteName(clickedNote, true, true, 3));

        int zoneAtNote = findZoneAtNote(clickedNote);
        if (zoneAtNote >= 0)
        {
            menu.addItem(2, "Delete Zone");
            menu.addItem(3, "Set Root Note to " + juce::MidiMessage::getMidiNoteName(clickedNote, true, true, 3));
        }

        menu.showMenuAsync(juce::PopupMenu::Options(),
                          [this, clickedNote, zoneAtNote](int result)
        {
            if (result == 1)
                addZoneAtNote(clickedNote);
            else if (result == 2 && zoneAtNote >= 0)
                deleteSelectedZone();
            else if (result == 3 && zoneAtNote >= 0)
                setRootNoteForSelectedZone(clickedNote);
        });

        return;
    }

    // Left-click: select zone or start dragging edge
    int zoneIndex = findZoneAtNote(clickedNote);
    if (zoneIndex >= 0)
    {
        setSelectedZone(zoneIndex);

        // Check if clicking near edge for resizing
        bool isLeftEdge;
        if (isNearZoneEdge(zoneIndex, clickedNote, isLeftEdge))
        {
            isDraggingEdge = true;
            isDraggingLeftEdge = isLeftEdge;
            dragStartNote = clickedNote;
        }
    }
    else
    {
        setSelectedZone(-1);
    }
}

void ZoneEditor::mouseDrag(const juce::MouseEvent& e)
{
    int currentNote = getNoteFromPosition(e.position.x);
    if (currentNote < LOWEST_NOTE || currentNote > HIGHEST_NOTE)
        return;

    if (isDraggingEdge && selectedZoneIndex >= 0)
    {
        auto& zone = keyZoneMap.getZones()[selectedZoneIndex];
        int newMin = zone.noteMin;
        int newMax = zone.noteMax;

        if (isDraggingLeftEdge)
        {
            newMin = juce::jlimit(LOWEST_NOTE, zone.noteMax - 1, currentNote);
        }
        else
        {
            newMax = juce::jlimit(zone.noteMin + 1, HIGHEST_NOTE, currentNote);
        }

        // Update zone range
        zone.noteMin = newMin;
        zone.noteMax = newMax;

        // Clamp root note to new range
        zone.rootNote = juce::jlimit(newMin, newMax, zone.rootNote);

        listeners.call([this, newMin, newMax](Listener& l)
        {
            l.zoneRangeChanged(selectedZoneIndex, newMin, newMax);
        });

        repaint();
    }
}

void ZoneEditor::mouseUp(const juce::MouseEvent& e)
{
    isDraggingEdge = false;
    dragStartNote = -1;
}

void ZoneEditor::mouseMove(const juce::MouseEvent& e)
{
    int note = getNoteFromPosition(e.position.x);
    if (note != hoveredNote)
    {
        hoveredNote = note;
        repaint();
    }

    // Update cursor for edge resizing
    if (selectedZoneIndex >= 0)
    {
        bool isLeftEdge;
        if (isNearZoneEdge(selectedZoneIndex, note, isLeftEdge))
        {
            setMouseCursor(juce::MouseCursor::LeftRightResizeCursor);
            return;
        }
    }

    setMouseCursor(juce::MouseCursor::NormalCursor);
}

void ZoneEditor::setSelectedZone(int index)
{
    if (selectedZoneIndex != index)
    {
        selectedZoneIndex = index;
        listeners.call([this, index](Listener& l)
        {
            l.zoneSelectionChanged(index);
        });
        repaint();
    }
}

float ZoneEditor::getNotePosition(int midiNote) const
{
    float width = static_cast<float>(getWidth());
    float noteWidth = width / NUM_KEYS;
    return (midiNote - LOWEST_NOTE) * noteWidth;
}

int ZoneEditor::getNoteFromPosition(float x) const
{
    float width = static_cast<float>(getWidth());
    float noteWidth = width / NUM_KEYS;
    int note = LOWEST_NOTE + static_cast<int>(x / noteWidth);
    return juce::jlimit(LOWEST_NOTE, HIGHEST_NOTE, note);
}

juce::Rectangle<float> ZoneEditor::getNoteRectangle(int midiNote) const
{
    float startX = getNotePosition(midiNote);
    float endX = getNotePosition(midiNote + 1);
    float height = static_cast<float>(getHeight());

    if (isBlackKey(midiNote))
    {
        // Black keys are shorter and thinner
        float width = (endX - startX) * 0.6f;
        float blackKeyHeight = height * 0.6f;
        return juce::Rectangle<float>(startX + (endX - startX - width) * 0.5f,
                                     0.0f, width, blackKeyHeight);
    }
    else
    {
        // White keys full height
        return juce::Rectangle<float>(startX, 0.0f, endX - startX, height);
    }
}

bool ZoneEditor::isBlackKey(int midiNote) const
{
    int noteInOctave = midiNote % 12;
    return (noteInOctave == 1 || noteInOctave == 3 || noteInOctave == 6 ||
            noteInOctave == 8 || noteInOctave == 10);
}

juce::Colour ZoneEditor::getZoneColor(int zoneIndex) const
{
    return ZONE_COLORS[zoneIndex % NUM_ZONE_COLORS];
}

void ZoneEditor::addZoneAtNote(int midiNote)
{
    // Create new zone (single note initially)
    KeyZone newZone;
    newZone.noteMin = midiNote;
    newZone.noteMax = midiNote;
    newZone.rootNote = midiNote;

    keyZoneMap.addZone(newZone);

    int newIndex = keyZoneMap.getNumZones() - 1;
    setSelectedZone(newIndex);

    listeners.call([this, newIndex](Listener& l)
    {
        l.zoneAdded(newIndex);
    });

    repaint();
}

void ZoneEditor::deleteSelectedZone()
{
    if (selectedZoneIndex >= 0 && selectedZoneIndex < keyZoneMap.getNumZones())
    {
        int deletedIndex = selectedZoneIndex;
        keyZoneMap.removeZone(selectedZoneIndex);
        setSelectedZone(-1);

        listeners.call([deletedIndex](Listener& l)
        {
            l.zoneDeleted(deletedIndex);
        });

        repaint();
    }
}

void ZoneEditor::setRootNoteForSelectedZone(int midiNote)
{
    if (selectedZoneIndex >= 0 && selectedZoneIndex < keyZoneMap.getNumZones())
    {
        auto& zone = keyZoneMap.getZones()[selectedZoneIndex];
        if (midiNote >= zone.noteMin && midiNote <= zone.noteMax)
        {
            zone.rootNote = midiNote;
            repaint();
        }
    }
}

int ZoneEditor::findZoneAtNote(int midiNote) const
{
    for (int i = 0; i < keyZoneMap.getNumZones(); ++i)
    {
        const auto& zone = keyZoneMap.getZones()[i];
        if (midiNote >= zone.noteMin && midiNote <= zone.noteMax)
        {
            return i;
        }
    }
    return -1;
}

bool ZoneEditor::isNearZoneEdge(int zoneIndex, int midiNote, bool& isLeftEdge) const
{
    if (zoneIndex < 0 || zoneIndex >= keyZoneMap.getNumZones())
        return false;

    const auto& zone = keyZoneMap.getZones()[zoneIndex];

    // Check if within 2 notes of either edge
    if (std::abs(midiNote - zone.noteMin) <= 1)
    {
        isLeftEdge = true;
        return true;
    }

    if (std::abs(midiNote - zone.noteMax) <= 1)
    {
        isLeftEdge = false;
        return true;
    }

    return false;
}
