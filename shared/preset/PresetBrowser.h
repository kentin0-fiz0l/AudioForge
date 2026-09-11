#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "PresetManager.h"

namespace AudioForge {

class PresetBrowser : public juce::Component,
                      public juce::ComboBox::Listener,
                      public juce::Button::Listener {
public:
    explicit PresetBrowser(PresetManager& presetManager);
    ~PresetBrowser() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    // Listeners
    void comboBoxChanged(juce::ComboBox* box) override;
    void buttonClicked(juce::Button* button) override;

    // Update preset list
    void refreshPresetList();

private:
    PresetManager& presetManager_;

    juce::ComboBox presetComboBox_;
    juce::TextButton saveButton_;
    juce::TextButton deleteButton_;
    juce::Label presetLabel_;

    void showSaveDialog();
    void confirmDelete();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetBrowser)
};

} // namespace AudioForge
