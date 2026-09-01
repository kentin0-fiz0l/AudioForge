#pragma once

#include <juce_core/juce_core.h>
#include <vector>

/**
 * Velocity Layer
 *
 * Associates a sample index with a velocity range.
 */
struct VelocityLayer
{
    int sampleIndex = -1;           // Index into SampleLibrary
    float velocityMin = 0.0f;       // Minimum velocity (0.0-1.0)
    float velocityMax = 1.0f;       // Maximum velocity (0.0-1.0)

    VelocityLayer() = default;

    VelocityLayer(int sample, float vMin, float vMax)
        : sampleIndex(sample)
        , velocityMin(vMin)
        , velocityMax(vMax)
    {
    }

    bool matchesVelocity(float velocity) const
    {
        return velocity >= velocityMin && velocity <= velocityMax;
    }
};

/**
 * Key Zone
 *
 * Maps a MIDI note range to one or more velocity layers.
 * Supports round-robin playback for multiple samples at the same velocity.
 */
struct KeyZone
{
    int noteMin = 0;                       // Minimum MIDI note (0-127)
    int noteMax = 127;                     // Maximum MIDI note (0-127)
    int rootNote = 60;                     // Pitch reference (middle C by default)
    std::vector<VelocityLayer> velocityLayers;

    // Round-robin state (mutable for iteration)
    mutable int currentRoundRobin = 0;

    KeyZone() = default;

    KeyZone(int nMin, int nMax, int root = 60)
        : noteMin(nMin)
        , noteMax(nMax)
        , rootNote(root)
    {
    }

    /**
     * Check if this zone contains the given MIDI note.
     */
    bool containsNote(int midiNote) const
    {
        return midiNote >= noteMin && midiNote <= noteMax;
    }

    /**
     * Get the sample index for the given velocity.
     * Returns -1 if no layer matches the velocity.
     *
     * @param velocity Note velocity (0.0-1.0)
     * @return Sample index, or -1 if no match
     */
    int getSampleForVelocity(float velocity) const
    {
        // Find all matching layers
        std::vector<int> matchingIndices;

        for (size_t i = 0; i < velocityLayers.size(); ++i)
        {
            if (velocityLayers[i].matchesVelocity(velocity))
            {
                matchingIndices.push_back(static_cast<int>(i));
            }
        }

        if (matchingIndices.empty())
        {
            return -1;
        }

        // Round-robin through matching layers
        int layerIndex = matchingIndices[currentRoundRobin % matchingIndices.size()];
        currentRoundRobin = (currentRoundRobin + 1) % matchingIndices.size();

        return velocityLayers[layerIndex].sampleIndex;
    }
};

/**
 * Key Zone Map
 *
 * Manages all key zones for the sampler.
 */
class KeyZoneMap
{
public:
    KeyZoneMap() = default;

    /**
     * Add a zone to the map.
     */
    void addZone(const KeyZone& zone)
    {
        zones.push_back(zone);
    }

    /**
     * Remove a zone by index.
     */
    void removeZone(int index)
    {
        if (index >= 0 && index < static_cast<int>(zones.size()))
        {
            zones.erase(zones.begin() + index);
        }
    }

    /**
     * Clear all zones.
     */
    void clearAllZones()
    {
        zones.clear();
    }

    /**
     * Find the zone containing the given MIDI note.
     * Returns nullptr if no zone matches.
     */
    const KeyZone* getZoneForNote(int midiNote) const
    {
        for (const auto& zone : zones)
        {
            if (zone.containsNote(midiNote))
            {
                return &zone;
            }
        }
        return nullptr;
    }

    /**
     * Get mutable reference to zones (for editing).
     */
    std::vector<KeyZone>& getZones() { return zones; }

    /**
     * Get const reference to zones.
     */
    const std::vector<KeyZone>& getZones() const { return zones; }

    /**
     * Get number of zones.
     */
    int getNumZones() const { return static_cast<int>(zones.size()); }

private:
    std::vector<KeyZone> zones;
};
