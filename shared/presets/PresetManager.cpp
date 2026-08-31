#include "PresetManager.h"

namespace AudioForge {

PresetManager::PresetManager(const juce::String& pluginName,
                             juce::AudioProcessor& processor)
    : pluginName(pluginName)
    , processor(processor)
{
}

void PresetManager::scanPresets()
{
    presets.clear();
    loadFactoryPresets();
    loadUserPresets();
}

std::vector<PresetManager::PresetInfo> PresetManager::getPresetsInCategory(const juce::String& category) const
{
    std::vector<PresetInfo> result;
    for (const auto& preset : presets)
    {
        if (preset.category.equalsIgnoreCase(category))
        {
            result.push_back(preset);
        }
    }
    return result;
}

std::vector<juce::String> PresetManager::getCategories() const
{
    std::vector<juce::String> categories;
    for (const auto& preset : presets)
    {
        if (std::find(categories.begin(), categories.end(), preset.category) == categories.end())
        {
            categories.push_back(preset.category);
        }
    }
    return categories;
}

bool PresetManager::loadPreset(size_t index)
{
    if (index >= presets.size())
        return false;

    const auto& preset = presets[index];

    if (preset.isFactory)
    {
        // Load from embedded XML string
        if (index >= factoryPresetsXML.size())
            return false;

        if (loadPresetFromXML(factoryPresetsXML[index]))
        {
            currentPresetIndex = static_cast<int>(index);
            return true;
        }
    }
    else
    {
        // Load from file
        if (!preset.file.existsAsFile())
            return false;

        juce::String xmlData = preset.file.loadFileAsString();
        if (loadPresetFromXML(xmlData))
        {
            currentPresetIndex = static_cast<int>(index);
            return true;
        }
    }

    return false;
}

bool PresetManager::loadPresetFromXML(const juce::String& xmlData)
{
    std::unique_ptr<juce::XmlElement> xml(juce::XmlDocument::parse(xmlData));

    if (xml == nullptr || !xml->hasTagName("AudioForgePreset"))
        return false;

    // Get the processor state
    auto* stateElement = xml->getChildByName("ProcessorState");
    if (stateElement == nullptr)
        return false;

    // Load processor state
    juce::MemoryBlock memoryBlock;
    memoryBlock.fromBase64Encoding(stateElement->getAllSubText());

    if (memoryBlock.getSize() > 0)
    {
        processor.setStateInformation(memoryBlock.getData(), static_cast<int>(memoryBlock.getSize()));
        return true;
    }

    return false;
}

bool PresetManager::savePreset(const juce::String& name,
                               const juce::String& author,
                               const juce::String& category,
                               const juce::String& description)
{
    // Create preset XML
    auto xml = createPresetXML(name, author, category, description);
    if (xml == nullptr)
        return false;

    // Ensure user presets directory exists
    juce::File presetsDir = getUserPresetsDirectory();
    if (!presetsDir.exists())
    {
        if (!presetsDir.createDirectory())
            return false;
    }

    // Create filename from preset name (sanitize)
    juce::String filename = name.replaceCharacters(" /\\:*?\"<>|", "_________");
    filename += ".preset";

    juce::File presetFile = presetsDir.getChildFile(filename);

    // Save to file
    if (!xml->writeTo(presetFile))
        return false;

    // Rescan presets to update list
    scanPresets();

    return true;
}

bool PresetManager::deletePreset(size_t index)
{
    if (index >= presets.size())
        return false;

    const auto& preset = presets[index];

    // Can't delete factory presets
    if (preset.isFactory)
        return false;

    // Delete file
    if (preset.file.existsAsFile())
    {
        if (preset.file.deleteFile())
        {
            // Rescan presets
            scanPresets();
            currentPresetIndex = -1;
            return true;
        }
    }

    return false;
}

void PresetManager::setFactoryPresets(const std::vector<juce::String>& factoryPresetsXML_)
{
    factoryPresetsXML = factoryPresetsXML_;
}

juce::File PresetManager::getUserPresetsDirectory()
{
    // ~/Documents/AudioForge/{PluginName}/Presets/
    juce::File documentsDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    return documentsDir.getChildFile("AudioForge")
                      .getChildFile(pluginName)
                      .getChildFile("Presets");
}

void PresetManager::loadFactoryPresets()
{
    for (size_t i = 0; i < factoryPresetsXML.size(); ++i)
    {
        std::unique_ptr<juce::XmlElement> xml(juce::XmlDocument::parse(factoryPresetsXML[i]));

        if (xml != nullptr && xml->hasTagName("AudioForgePreset"))
        {
            PresetInfo info = parsePresetInfo(*xml, true);
            presets.push_back(info);
        }
    }
}

void PresetManager::loadUserPresets()
{
    juce::File presetsDir = getUserPresetsDirectory();

    if (!presetsDir.exists())
        return;

    // Find all .preset files
    juce::Array<juce::File> presetFiles;
    presetsDir.findChildFiles(presetFiles, juce::File::findFiles, false, "*.preset");

    for (const auto& file : presetFiles)
    {
        juce::String xmlData = file.loadFileAsString();
        std::unique_ptr<juce::XmlElement> xml(juce::XmlDocument::parse(xmlData));

        if (xml != nullptr && xml->hasTagName("AudioForgePreset"))
        {
            PresetInfo info = parsePresetInfo(*xml, false);
            info.file = file;
            presets.push_back(info);
        }
    }
}

PresetManager::PresetInfo PresetManager::parsePresetInfo(const juce::XmlElement& xml, bool isFactory)
{
    PresetInfo info;
    info.isFactory = isFactory;
    info.name = xml.getStringAttribute("name", "Unnamed");
    info.author = xml.getStringAttribute("author", "Unknown");
    info.category = xml.getStringAttribute("category", "Default");
    info.description = xml.getStringAttribute("description", "");

    return info;
}

std::unique_ptr<juce::XmlElement> PresetManager::createPresetXML(const juce::String& name,
                                                                   const juce::String& author,
                                                                   const juce::String& category,
                                                                   const juce::String& description)
{
    // Create root element
    auto xml = std::make_unique<juce::XmlElement>("AudioForgePreset");

    // Add metadata
    xml->setAttribute("name", name);
    xml->setAttribute("author", author);
    xml->setAttribute("category", category);
    xml->setAttribute("description", description);
    xml->setAttribute("version", "1.0");

    // Get current processor state
    juce::MemoryBlock stateData;
    processor.getStateInformation(stateData);

    // Add processor state as base64-encoded data
    auto* stateElement = xml->createNewChildElement("ProcessorState");
    stateElement->addTextElement(stateData.toBase64Encoding());

    return xml;
}

} // namespace AudioForge
