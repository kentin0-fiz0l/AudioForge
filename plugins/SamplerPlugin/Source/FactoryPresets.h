#pragma once

#include <juce_core/juce_core.h>
#include <vector>

/**
 * Factory Presets
 *
 * Built-in presets for the sampler.
 * Each preset defines parameter values and zone/layer configuration.
 *
 * Note: Phase 3 includes preset metadata only.
 * Actual sample files will be bundled in Phase 5.
 */

struct PresetData
{
    juce::String name;
    juce::String category;
    juce::String description;

    // Parameter values (normalized 0.0-1.0)
    struct Parameters
    {
        float ampAttack = 0.01f;
        float ampDecay = 0.1f;
        float ampSustain = 0.7f;
        float ampRelease = 0.3f;

        float filterCutoff = 1.0f;      // 20000 Hz (wide open)
        float filterResonance = 0.707f;
        int filterType = 0;             // Low-pass
        float filterEnvAmount = 0.0f;

        float filtAttack = 0.01f;
        float filtDecay = 0.1f;
        float filtSustain = 0.7f;
        float filtRelease = 0.3f;

        float masterVolume = 0.7f;
        float masterTune = 0.0f;
        int loopMode = 0;  // None
    };

    Parameters params;

    PresetData(const juce::String& n, const juce::String& cat, const juce::String& desc)
        : name(n), category(cat), description(desc)
    {
    }
};

/**
 * Factory Preset Library
 */
class FactoryPresets
{
public:
    static std::vector<PresetData> getAllPresets()
    {
        std::vector<PresetData> presets;

        // ==================== DRUM KITS ====================

        // 1. Acoustic Kit
        {
            PresetData preset("Acoustic Kit", "Drums", "Classic acoustic drum kit");
            preset.params.ampAttack = 0.001f;
            preset.params.ampRelease = 0.2f;
            preset.params.filterCutoff = 1.0f;
            presets.push_back(preset);
        }

        // 2. Electronic Kit
        {
            PresetData preset("Electronic Kit", "Drums", "Punchy electronic drums");
            preset.params.ampAttack = 0.001f;
            preset.params.ampRelease = 0.15f;
            preset.params.filterCutoff = 0.8f;
            presets.push_back(preset);
        }

        // 3. 808 Kit
        {
            PresetData preset("808 Kit", "Drums", "TR-808 style drum kit");
            preset.params.ampAttack = 0.001f;
            preset.params.ampDecay = 0.3f;
            preset.params.ampSustain = 0.0f;
            preset.params.ampRelease = 0.4f;
            presets.push_back(preset);
        }

        // 4. Hip-Hop Kit
        {
            PresetData preset("Hip-Hop Kit", "Drums", "Modern hip-hop drums");
            preset.params.ampAttack = 0.001f;
            preset.params.ampRelease = 0.1f;
            preset.params.filterCutoff = 0.7f;
            presets.push_back(preset);
        }

        // 5. Jazz Kit
        {
            PresetData preset("Jazz Kit", "Drums", "Warm jazz drum kit");
            preset.params.ampAttack = 0.005f;
            preset.params.ampRelease = 0.4f;
            preset.params.filterCutoff = 0.85f;
            presets.push_back(preset);
        }

        // ==================== INSTRUMENTS ====================

        // 6. Grand Piano
        {
            PresetData preset("Grand Piano", "Instruments", "Sampled grand piano");
            preset.params.ampAttack = 0.001f;
            preset.params.ampDecay = 0.5f;
            preset.params.ampSustain = 0.5f;
            preset.params.ampRelease = 0.8f;
            preset.params.loopMode = 1;  // Forward loop
            presets.push_back(preset);
        }

        // 7. Electric Piano
        {
            PresetData preset("Electric Piano", "Instruments", "Rhodes-style electric piano");
            preset.params.ampAttack = 0.01f;
            preset.params.ampDecay = 0.3f;
            preset.params.ampSustain = 0.6f;
            preset.params.ampRelease = 0.5f;
            preset.params.loopMode = 1;
            presets.push_back(preset);
        }

        // 8. Acoustic Bass
        {
            PresetData preset("Acoustic Bass", "Instruments", "Upright bass");
            preset.params.ampAttack = 0.005f;
            preset.params.ampDecay = 0.2f;
            preset.params.ampSustain = 0.7f;
            preset.params.ampRelease = 0.3f;
            preset.params.filterCutoff = 0.6f;
            presets.push_back(preset);
        }

        // 9. Strings Ensemble
        {
            PresetData preset("Strings Ensemble", "Instruments", "Lush string section");
            preset.params.ampAttack = 0.2f;
            preset.params.ampDecay = 0.3f;
            preset.params.ampSustain = 0.9f;
            preset.params.ampRelease = 1.0f;
            preset.params.loopMode = 1;  // Forward loop
            presets.push_back(preset);
        }

        // 10. Brass Section
        {
            PresetData preset("Brass Section", "Instruments", "Big band brass");
            preset.params.ampAttack = 0.05f;
            preset.params.ampDecay = 0.2f;
            preset.params.ampSustain = 0.8f;
            preset.params.ampRelease = 0.4f;
            preset.params.loopMode = 1;
            presets.push_back(preset);
        }

        // ==================== TEXTURES ====================

        // 11. Ambient Pad
        {
            PresetData preset("Ambient Pad", "Textures", "Evolving ambient texture");
            preset.params.ampAttack = 0.8f;
            preset.params.ampDecay = 0.5f;
            preset.params.ampSustain = 0.9f;
            preset.params.ampRelease = 2.0f;
            preset.params.filterCutoff = 0.7f;
            preset.params.filterEnvAmount = 0.3f;
            preset.params.filtAttack = 1.0f;
            preset.params.loopMode = 1;
            presets.push_back(preset);
        }

        // 12. Cinematic Impact
        {
            PresetData preset("Cinematic Impact", "Textures", "Powerful impact sounds");
            preset.params.ampAttack = 0.001f;
            preset.params.ampDecay = 1.5f;
            preset.params.ampSustain = 0.3f;
            preset.params.ampRelease = 2.0f;
            presets.push_back(preset);
        }

        // 13. Vinyl Texture
        {
            PresetData preset("Vinyl Texture", "Textures", "Vinyl crackle and noise");
            preset.params.ampAttack = 0.1f;
            preset.params.ampSustain = 1.0f;
            preset.params.ampRelease = 0.5f;
            preset.params.filterCutoff = 0.5f;
            preset.params.loopMode = 1;
            presets.push_back(preset);
        }

        // 14. Rain Ambience
        {
            PresetData preset("Rain Ambience", "Textures", "Natural rain sounds");
            preset.params.ampAttack = 0.5f;
            preset.params.ampSustain = 1.0f;
            preset.params.ampRelease = 1.0f;
            preset.params.filterCutoff = 0.6f;
            preset.params.loopMode = 1;
            presets.push_back(preset);
        }

        // 15. Sci-Fi FX
        {
            PresetData preset("Sci-Fi FX", "Textures", "Futuristic sound effects");
            preset.params.ampAttack = 0.05f;
            preset.params.ampDecay = 0.8f;
            preset.params.ampSustain = 0.5f;
            preset.params.ampRelease = 1.0f;
            preset.params.filterCutoff = 0.4f;
            preset.params.filterEnvAmount = 0.5f;
            preset.params.filtAttack = 0.3f;
            presets.push_back(preset);
        }

        // 16. Vocal Chops
        {
            PresetData preset("Vocal Chops", "Textures", "Chopped vocal samples");
            preset.params.ampAttack = 0.001f;
            preset.params.ampDecay = 0.1f;
            preset.params.ampSustain = 0.5f;
            preset.params.ampRelease = 0.2f;
            preset.params.filterCutoff = 0.75f;
            presets.push_back(preset);
        }

        // 17. Granular Clouds
        {
            PresetData preset("Granular Clouds", "Textures", "Granular synthesis textures");
            preset.params.ampAttack = 0.3f;
            preset.params.ampDecay = 0.5f;
            preset.params.ampSustain = 0.8f;
            preset.params.ampRelease = 1.5f;
            preset.params.filterCutoff = 0.6f;
            preset.params.filterEnvAmount = 0.4f;
            preset.params.loopMode = 2;  // Ping-pong
            presets.push_back(preset);
        }

        // 18. Synth Lead
        {
            PresetData preset("Synth Lead", "Instruments", "Bright synth lead");
            preset.params.ampAttack = 0.01f;
            preset.params.ampDecay = 0.2f;
            preset.params.ampSustain = 0.7f;
            preset.params.ampRelease = 0.3f;
            preset.params.filterCutoff = 0.8f;
            preset.params.filterEnvAmount = 0.3f;
            preset.params.loopMode = 1;
            presets.push_back(preset);
        }

        // 19. Organ
        {
            PresetData preset("Organ", "Instruments", "Hammond-style organ");
            preset.params.ampAttack = 0.01f;
            preset.params.ampDecay = 0.1f;
            preset.params.ampSustain = 1.0f;
            preset.params.ampRelease = 0.1f;
            preset.params.filterCutoff = 0.9f;
            preset.params.loopMode = 1;
            presets.push_back(preset);
        }

        // 20. Plucked Strings
        {
            PresetData preset("Plucked Strings", "Instruments", "Guitar/harp plucks");
            preset.params.ampAttack = 0.001f;
            preset.params.ampDecay = 0.4f;
            preset.params.ampSustain = 0.4f;
            preset.params.ampRelease = 0.6f;
            preset.params.filterCutoff = 0.85f;
            presets.push_back(preset);
        }

        return presets;
    }

    static juce::StringArray getCategories()
    {
        return {"Drums", "Instruments", "Textures"};
    }

    static std::vector<PresetData> getPresetsInCategory(const juce::String& category)
    {
        auto allPresets = getAllPresets();
        std::vector<PresetData> filtered;

        for (const auto& preset : allPresets)
        {
            if (preset.category == category)
                filtered.push_back(preset);
        }

        return filtered;
    }
};
