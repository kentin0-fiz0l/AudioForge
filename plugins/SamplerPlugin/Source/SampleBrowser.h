#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "SampleLibrary.h"

/**
 * Sample Browser Component
 *
 * Displays all loaded samples in a list view.
 * Allows drag-and-drop to assign samples to zones/layers.
 *
 * Features:
 * - List view of all loaded samples
 * - Sample info (name, length, size)
 * - Drag samples to zones or velocity layers
 * - Delete samples (if not in use)
 * - Memory usage indicator
 */
class SampleBrowser : public juce::Component,
                      public juce::ListBoxModel
{
public:
    SampleBrowser(SampleLibrary& library);
    ~SampleBrowser() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;

    //==============================================================================
    // ListBoxModel implementation
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g,
                         int width, int height, bool rowIsSelected) override;
    void selectedRowsChanged(int lastRowSelected) override;
    juce::var getDragSourceDescription(const juce::SparseSet<int>& selectedRows) override;

    //==============================================================================
    // Updates
    void refresh();
    int getSelectedSampleIndex() const { return selectedSampleIndex; }

private:
    SampleLibrary& sampleLibrary;
    juce::ListBox listBox;
    int selectedSampleIndex = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleBrowser)
};
