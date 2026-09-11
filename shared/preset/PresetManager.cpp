#include "PresetManager.h"

namespace AudioForge {

PresetManager::PresetManager(juce::AudioProcessorValueTreeState& apvts,
                            const juce::String& pluginName)
    : apvts_(apvts),
      pluginName_(pluginName),
      currentPreset_(nullptr),
      isModified_(false) {

    // Create user preset directory if it doesn't exist
    auto userDir = getUserPresetDirectory();
    if (!userDir.exists())
        userDir.createDirectory();

    // Scan for existing presets
    scanPresets();
}

PresetManager::~PresetManager() {}

void PresetManager::savePreset(const juce::String& name,
                               const juce::String& category,
                               const juce::String& author) {
    auto userDir = getUserPresetDirectory();
    auto presetFile = userDir.getChildFile(name + ".afpreset");

    // Create preset XML
    juce::XmlElement preset("Preset");
    preset.setAttribute("name", name);
    preset.setAttribute("plugin", pluginName_);
    preset.setAttribute("version", "1.0");
    preset.setAttribute("category", category);
    preset.setAttribute("author", author.isEmpty() ? "User" : author);

    // Save all APVTS parameters
    auto state = apvts_.copyState();
    auto* params = preset.createNewChildElement("Parameters");
    params->addChildElement(state.createXml().release());

    // Write to file
    if (preset.writeTo(presetFile)) {
        // Rescan to update preset list
        scanPresets();

        // Set as current preset
        for (const auto& p : presets_) {
            if (p.file == presetFile) {
                currentPreset_ = &p;
                isModified_ = false;
                break;
            }
        }
    }
}

bool PresetManager::loadPreset(const juce::File& presetFile) {
    if (!presetFile.existsAsFile())
        return false;

    auto xml = juce::parseXML(presetFile);
    if (!xml)
        return false;

    // Verify it's a valid preset for this plugin
    if (xml->getStringAttribute("plugin") != pluginName_)
        return false;

    // Load parameters
    auto* params = xml->getChildByName("Parameters");
    if (!params || params->getNumChildElements() == 0)
        return false;

    auto* state = params->getFirstChildElement();
    if (state) {
        auto valueTree = juce::ValueTree::fromXml(*state);
        apvts_.replaceState(valueTree);

        // Update current preset tracking
        for (const auto& p : presets_) {
            if (p.file == presetFile) {
                currentPreset_ = &p;
                isModified_ = false;
                return true;
            }
        }
    }

    return false;
}

bool PresetManager::loadPreset(const juce::String& presetName) {
    for (const auto& preset : presets_) {
        if (preset.name == presetName) {
            return loadPreset(preset.file);
        }
    }
    return false;
}

void PresetManager::deletePreset(const juce::File& presetFile) {
    // Only allow deleting user presets
    if (!presetFile.isAChildOf(getUserPresetDirectory()))
        return;

    if (currentPreset_ && currentPreset_->file == presetFile)
        currentPreset_ = nullptr;

    presetFile.deleteFile();
    scanPresets();
}

void PresetManager::scanPresets() {
    presets_.clear();

    // Scan factory presets
    auto factoryDir = getFactoryPresetDirectory();
    if (factoryDir.exists())
        scanDirectory(factoryDir, true);

    // Scan user presets
    auto userDir = getUserPresetDirectory();
    if (userDir.exists())
        scanDirectory(userDir, false);
}

void PresetManager::scanDirectory(const juce::File& directory, bool isFactory) {
    for (const auto& entry : juce::RangedDirectoryIterator(
            directory, false, "*.afpreset", juce::File::findFiles)) {

        auto file = entry.getFile();
        auto xml = juce::parseXML(file);

        if (!xml)
            continue;

        // Verify it's for this plugin
        if (xml->getStringAttribute("plugin") != pluginName_)
            continue;

        Preset preset;
        preset.name = xml->getStringAttribute("name", file.getFileNameWithoutExtension());
        preset.category = xml->getStringAttribute("category", "Uncategorized");
        preset.author = xml->getStringAttribute("author", isFactory ? "AudioForge" : "User");
        preset.file = file;
        preset.isFactory = isFactory;

        presets_.add(preset);
    }
}

juce::Array<PresetManager::Preset> PresetManager::getPresetsByCategory(
    const juce::String& category) const {
    juce::Array<Preset> filtered;
    for (const auto& preset : presets_) {
        if (preset.category == category)
            filtered.add(preset);
    }
    return filtered;
}

juce::StringArray PresetManager::getCategories() const {
    juce::StringArray categories;
    for (const auto& preset : presets_) {
        if (!categories.contains(preset.category))
            categories.add(preset.category);
    }
    categories.sort(true);
    return categories;
}

juce::String PresetManager::getCurrentPresetName() const {
    if (currentPreset_)
        return currentPreset_->name + (isModified_ ? " *" : "");
    return "Init";
}

void PresetManager::installFactoryPresets(const juce::File& resourceDir) {
    auto factoryDir = getFactoryPresetDirectory();
    if (!factoryDir.exists())
        factoryDir.createDirectory();

    // Copy presets from resource directory to factory preset directory
    if (resourceDir.exists() && resourceDir.isDirectory()) {
        for (const auto& entry : juce::RangedDirectoryIterator(
                resourceDir, false, "*.afpreset", juce::File::findFiles)) {

            auto source = entry.getFile();
            auto dest = factoryDir.getChildFile(source.getFileName());

            if (!dest.exists())
                source.copyFileTo(dest);
        }
    }

    scanPresets();
}

std::unique_ptr<juce::XmlElement> PresetManager::saveToXml() const {
    auto xml = std::make_unique<juce::XmlElement>("PresetManagerState");

    if (currentPreset_) {
        xml->setAttribute("currentPreset", currentPreset_->name);
        xml->setAttribute("isModified", isModified_);
    }

    return xml;
}

void PresetManager::loadFromXml(const juce::XmlElement& xml) {
    if (xml.hasAttribute("currentPreset")) {
        auto presetName = xml.getStringAttribute("currentPreset");
        loadPreset(presetName);
        isModified_ = xml.getBoolAttribute("isModified", false);
    }
}

juce::File PresetManager::getUserPresetDirectory() const {
    return getUserPresetsPath();
}

juce::File PresetManager::getFactoryPresetDirectory() const {
    return getFactoryPresetsPath();
}

juce::File PresetManager::getUserPresetsPath() const {
    auto appSupport = juce::File::getSpecialLocation(
        juce::File::userApplicationDataDirectory);
    return appSupport.getChildFile("AudioForge")
                    .getChildFile(pluginName_)
                    .getChildFile("Presets");
}

juce::File PresetManager::getFactoryPresetsPath() const {
    // Factory presets ship with the plugin bundle
    // For now, we'll use a subfolder in user presets until we set up bundles
    return getUserPresetsPath().getChildFile("Factory");
}

} // namespace AudioForge
