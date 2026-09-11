#include "PresetBrowser.h"

namespace AudioForge {

PresetBrowser::PresetBrowser(PresetManager& presetManager)
    : presetManager_(presetManager) {

    // Preset label
    presetLabel_.setText("Preset:", juce::dontSendNotification);
    presetLabel_.setJustificationType(juce::Justification::centredRight);
    presetLabel_.setColour(juce::Label::textColourId, juce::Colour(0xffaaaaaa));
    addAndMakeVisible(presetLabel_);

    // Preset combo box
    presetComboBox_.setTextWhenNothingSelected("-- Select Preset --");
    presetComboBox_.setTextWhenNoChoicesAvailable("No presets available");
    presetComboBox_.addListener(this);
    addAndMakeVisible(presetComboBox_);

    // Save button
    saveButton_.setButtonText("Save");
    saveButton_.addListener(this);
    addAndMakeVisible(saveButton_);

    // Delete button
    deleteButton_.setButtonText("Delete");
    deleteButton_.addListener(this);
    addAndMakeVisible(deleteButton_);

    // Load preset list
    refreshPresetList();
}

PresetBrowser::~PresetBrowser() {
    presetComboBox_.removeListener(this);
    saveButton_.removeListener(this);
    deleteButton_.removeListener(this);
}

void PresetBrowser::paint(juce::Graphics& g) {
    // Optional: Draw background or border
    g.setColour(juce::Colour(0xff2a2a2a));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 4.0f);
}

void PresetBrowser::resized() {
    auto bounds = getLocalBounds().reduced(5);

    // Layout: [Label] [ComboBox] [Save] [Delete]
    auto labelBounds = bounds.removeFromLeft(60);
    presetLabel_.setBounds(labelBounds);

    auto deleteBounds = bounds.removeFromRight(60);
    deleteButton_.setBounds(deleteBounds);

    bounds.removeFromRight(5); // Spacing

    auto saveBounds = bounds.removeFromRight(60);
    saveButton_.setBounds(saveBounds);

    bounds.removeFromRight(5); // Spacing

    presetComboBox_.setBounds(bounds);
}

void PresetBrowser::comboBoxChanged(juce::ComboBox* box) {
    if (box == &presetComboBox_) {
        int selectedId = presetComboBox_.getSelectedId();
        if (selectedId > 0) {
            auto presets = presetManager_.getPresets();
            int index = selectedId - 1;

            if (index >= 0 && index < presets.size()) {
                presetManager_.loadPreset(presets[index].file);
            }
        }
    }
}

void PresetBrowser::buttonClicked(juce::Button* button) {
    if (button == &saveButton_) {
        showSaveDialog();
    } else if (button == &deleteButton_) {
        confirmDelete();
    }
}

void PresetBrowser::refreshPresetList() {
    presetComboBox_.clear(juce::dontSendNotification);

    auto presets = presetManager_.getPresets();

    if (presets.isEmpty()) {
        presetComboBox_.setTextWhenNothingSelected("No presets available");
        return;
    }

    // Add presets grouped by category
    auto categories = presetManager_.getCategories();

    for (const auto& category : categories) {
        presetComboBox_.addSeparator();
        presetComboBox_.addItem("-- " + category + " --", -1);

        auto categoryPresets = presetManager_.getPresetsByCategory(category);
        for (int i = 0; i < categoryPresets.size(); ++i) {
            int globalIndex = presets.indexOf(categoryPresets[i]);
            if (globalIndex >= 0) {
                juce::String displayName = categoryPresets[i].name;
                if (categoryPresets[i].isFactory)
                    displayName += " (Factory)";

                presetComboBox_.addItem(displayName, globalIndex + 1);
            }
        }
    }

    // Select current preset if one is loaded
    auto* current = presetManager_.getCurrentPreset();
    if (current) {
        int index = presets.indexOf(*current);
        if (index >= 0)
            presetComboBox_.setSelectedId(index + 1, juce::dontSendNotification);
    }
}

void PresetBrowser::showSaveDialog() {
    // Simplified save - use default name and category for now
    // TODO: Implement async dialog in JUCE 7
    juce::String name = "Preset " + juce::String(presetManager_.getPresets().size() + 1);
    juce::String category = "User";

    presetManager_.savePreset(name, category);
    refreshPresetList();
}

void PresetBrowser::confirmDelete() {
    auto* current = presetManager_.getCurrentPreset();
    if (!current) {
        juce::NativeMessageBox::showMessageBoxAsync(
            juce::MessageBoxIconType::WarningIcon,
            "Delete Preset",
            "No preset selected.");
        return;
    }

    if (current->isFactory) {
        juce::NativeMessageBox::showMessageBoxAsync(
            juce::MessageBoxIconType::WarningIcon,
            "Delete Preset",
            "Cannot delete factory presets.");
        return;
    }

    // Simplified delete - just delete directly
    // TODO: Add async confirmation dialog
    presetManager_.deletePreset(current->file);
    refreshPresetList();
}

} // namespace AudioForge
