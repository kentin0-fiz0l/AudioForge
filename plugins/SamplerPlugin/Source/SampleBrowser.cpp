#include "SampleBrowser.h"

SampleBrowser::SampleBrowser(SampleLibrary& library)
    : sampleLibrary(library)
    , listBox("Samples", this)
{
    addAndMakeVisible(listBox);
    listBox.setRowHeight(30);
    listBox.setMultipleSelectionEnabled(false);

    setSize(200, 300);
}

SampleBrowser::~SampleBrowser()
{
}

void SampleBrowser::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff2a2a2a));

    // Header
    auto bounds = getLocalBounds();
    auto header = bounds.removeFromTop(30);

    g.setColour(juce::Colour(0xff3a3a3a));
    g.fillRect(header);

    g.setColour(juce::Colours::white);
    g.setFont(14.0f);
    g.drawText("SAMPLES (" + juce::String(sampleLibrary.getNumSamples()) + ")",
              header.reduced(5, 0), juce::Justification::centredLeft, true);

    // Memory usage
    auto memUsed = sampleLibrary.getTotalMemoryUsed() / (1024.0 * 1024.0);
    auto maxMem = SampleLibrary::MAX_MEMORY_BYTES / (1024.0 * 1024.0);
    juce::String memText = juce::String::formatted("%.0f/%.0f MB", memUsed, maxMem);
    g.setFont(10.0f);
    g.setColour(juce::Colours::grey);
    g.drawText(memText, header.reduced(5, 0), juce::Justification::centredRight, true);
}

void SampleBrowser::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(30);  // Skip header
    listBox.setBounds(bounds);
}

int SampleBrowser::getNumRows()
{
    return sampleLibrary.getNumSamples();
}

void SampleBrowser::paintListBoxItem(int rowNumber, juce::Graphics& g,
                                     int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
    {
        g.fillAll(juce::Colour(0xff4a9eff).withAlpha(0.3f));
    }
    else if (rowNumber % 2 == 0)
    {
        g.fillAll(juce::Colour(0xff1a1a1a));
    }

    auto sample = sampleLibrary.getSample(rowNumber);
    if (!sample)
        return;

    g.setColour(juce::Colours::white);
    g.setFont(12.0f);

    // Sample index
    juce::String indexText = juce::String::formatted("[%d]", rowNumber);
    g.drawText(indexText, 5, 0, 40, height, juce::Justification::centredLeft, true);

    // Sample info
    double lengthSec = sample->getLengthInSamples() / sample->getSampleRate();
    size_t sizeKB = sample->getSizeInBytes() / 1024;

    juce::String info = juce::String::formatted("Sample %d (%.1fs, %d KB)",
        rowNumber, lengthSec, static_cast<int>(sizeKB));

    g.drawText(info, 50, 0, width - 55, height, juce::Justification::centredLeft, true);
}

void SampleBrowser::selectedRowsChanged(int lastRowSelected)
{
    selectedSampleIndex = lastRowSelected;
}

juce::var SampleBrowser::getDragSourceDescription(const juce::SparseSet<int>& selectedRows)
{
    if (selectedRows.size() > 0)
    {
        // Return sample index as drag data
        return selectedRows[0];
    }
    return {};
}

void SampleBrowser::refresh()
{
    listBox.updateContent();
    repaint();
}
