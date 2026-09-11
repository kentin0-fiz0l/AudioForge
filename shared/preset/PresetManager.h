#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_data_structures/juce_data_structures.h>

namespace AudioForge {

class PresetManager {
public:
    struct Preset {
        juce::String name;
        juce::String category;
        juce::String author;
        juce::File file;
        bool isFactory;

        Preset() : isFactory(false) {}

        bool operator==(const Preset& other) const {
            return file == other.file;
        }
    };

    explicit PresetManager(juce::AudioProcessorValueTreeState& apvts,
                          const juce::String& pluginName);
    ~PresetManager();

    // Preset operations
    void savePreset(const juce::String& name,
                   const juce::String& category = "User",
                   const juce::String& author = "");
    bool loadPreset(const juce::File& presetFile);
    bool loadPreset(const juce::String& presetName);
    void deletePreset(const juce::File& presetFile);

    // Preset discovery
    void scanPresets();
    juce::Array<Preset> getPresets() const { return presets_; }
    juce::Array<Preset> getPresetsByCategory(const juce::String& category) const;
    juce::StringArray getCategories() const;

    // Current preset tracking
    const Preset* getCurrentPreset() const { return currentPreset_; }
    juce::String getCurrentPresetName() const;
    bool isModified() const { return isModified_; }
    void markModified() { isModified_ = true; }

    // Factory preset installation
    void installFactoryPresets(const juce::File& resourceDir);

    // Serialization (includes MIDI Learn mappings if available)
    std::unique_ptr<juce::XmlElement> saveToXml() const;
    void loadFromXml(const juce::XmlElement& xml);

    // Get preset directories
    juce::File getUserPresetDirectory() const;
    juce::File getFactoryPresetDirectory() const;

private:
    juce::AudioProcessorValueTreeState& apvts_;
    juce::String pluginName_;
    juce::Array<Preset> presets_;
    const Preset* currentPreset_;
    bool isModified_;

    juce::File getUserPresetsPath() const;
    juce::File getFactoryPresetsPath() const;

    void loadPresetFromFile(const juce::File& file);
    void scanDirectory(const juce::File& directory, bool isFactory);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManager)
};

} // namespace AudioForge
