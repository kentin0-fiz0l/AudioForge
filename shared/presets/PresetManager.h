#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_data_structures/juce_data_structures.h>
#include <vector>
#include <string>

namespace AudioForge {

/**
 * Preset Manager
 *
 * Handles loading, saving, and managing presets for AudioForge plugins.
 * Supports both factory presets (embedded in plugin) and user presets (saved to disk).
 *
 * Features:
 * - XML-based preset format
 * - Factory presets embedded as static strings
 * - User preset directory management (~Documents/AudioForge/{PluginName}/Presets/)
 * - Preset metadata (name, author, category, description)
 * - State serialization/deserialization
 */
class PresetManager
{
public:
    struct PresetInfo
    {
        juce::String name;
        juce::String author;
        juce::String category;
        juce::String description;
        bool isFactory;  // True for factory presets, false for user presets
        juce::File file; // File path (empty for factory presets)

        PresetInfo() : isFactory(false) {}
    };

    /**
     * Constructor.
     *
     * @param pluginName Name of the plugin (e.g., "FMSynth")
     * @param processor  Reference to the plugin processor (for state management)
     */
    PresetManager(const juce::String& pluginName,
                  juce::AudioProcessor& processor);

    /**
     * Load all available presets (factory + user).
     * Call this during plugin initialization.
     */
    void scanPresets();

    /**
     * Get all available presets.
     */
    const std::vector<PresetInfo>& getPresets() const { return presets; }

    /**
     * Get presets in a specific category.
     */
    std::vector<PresetInfo> getPresetsInCategory(const juce::String& category) const;

    /**
     * Get all categories.
     */
    std::vector<juce::String> getCategories() const;

    /**
     * Load a preset by index.
     *
     * @param index Preset index (from getPresets())
     * @return True if loaded successfully
     */
    bool loadPreset(size_t index);

    /**
     * Load a preset from XML data.
     *
     * @param xmlData XML string containing preset data
     * @return True if loaded successfully
     */
    bool loadPresetFromXML(const juce::String& xmlData);

    /**
     * Save current state as a user preset.
     *
     * @param name Preset name
     * @param author Author name
     * @param category Category (e.g., "Bass", "Lead", "Pad")
     * @param description Optional description
     * @return True if saved successfully
     */
    bool savePreset(const juce::String& name,
                   const juce::String& author,
                   const juce::String& category,
                   const juce::String& description = "");

    /**
     * Delete a user preset.
     *
     * @param index Preset index (must be a user preset, not factory)
     * @return True if deleted successfully
     */
    bool deletePreset(size_t index);

    /**
     * Get current preset index (-1 if no preset loaded).
     */
    int getCurrentPresetIndex() const { return currentPresetIndex; }

    /**
     * Set factory presets.
     * Call this in plugin constructor to embed factory presets.
     *
     * @param factoryPresetsXML Vector of XML strings (one per preset)
     */
    void setFactoryPresets(const std::vector<juce::String>& factoryPresetsXML);

    /**
     * Get the user presets directory.
     * Creates it if it doesn't exist.
     */
    juce::File getUserPresetsDirectory();

private:
    juce::String pluginName;
    juce::AudioProcessor& processor;
    std::vector<PresetInfo> presets;
    std::vector<juce::String> factoryPresetsXML;
    int currentPresetIndex = -1;

    /**
     * Load factory presets from embedded XML strings.
     */
    void loadFactoryPresets();

    /**
     * Load user presets from disk.
     */
    void loadUserPresets();

    /**
     * Parse preset metadata from XML.
     */
    PresetInfo parsePresetInfo(const juce::XmlElement& xml, bool isFactory);

    /**
     * Create preset XML from current processor state.
     */
    std::unique_ptr<juce::XmlElement> createPresetXML(const juce::String& name,
                                                       const juce::String& author,
                                                       const juce::String& category,
                                                       const juce::String& description);
};

} // namespace AudioForge
