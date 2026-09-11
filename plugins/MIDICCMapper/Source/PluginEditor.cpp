#include "PluginEditor.h"

MIDICCMapperEditor::MIDICCMapperEditor(MIDICCMapperProcessor& p)
    : AudioProcessorEditor(&p), processor_(p),
      presetBrowser_(processor_.getPresetManager()) {

    setSize(500, 450);

    // Preset browser
    addAndMakeVisible(presetBrowser_);

    // Column headers
    juce::String headers[] = {"#", "Status", "In CC", "Out CC", "Range", "Curve"};
    for (int i = 0; i < 6; ++i) {
        headerLabels_[i].setText(headers[i], juce::dontSendNotification);
        headerLabels_[i].setJustificationType(juce::Justification::centred);
        headerLabels_[i].setColour(juce::Label::textColourId, juce::Colour(0xff00d4ff));
        headerLabels_[i].setFont(juce::Font(14.0f, juce::Font::bold));
        addAndMakeVisible(headerLabels_[i]);
    }

    // Mapping rows
    for (int i = 0; i < 8; ++i) {
        auto& row = rows_[i];

        row.indexLabel.setText(juce::String(i + 1), juce::dontSendNotification);
        row.indexLabel.setJustificationType(juce::Justification::centred);
        row.indexLabel.setColour(juce::Label::textColourId, juce::Colour(0xff808080));
        addAndMakeVisible(row.indexLabel);

        row.statusLabel.setText("Disabled", juce::dontSendNotification);
        row.statusLabel.setJustificationType(juce::Justification::centred);
        row.statusLabel.setColour(juce::Label::textColourId, juce::Colour(0xff606060));
        addAndMakeVisible(row.statusLabel);

        row.inputLabel.setText("---", juce::dontSendNotification);
        row.inputLabel.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(row.inputLabel);

        row.outputLabel.setText("---", juce::dontSendNotification);
        row.outputLabel.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(row.outputLabel);

        row.rangeLabel.setText("---", juce::dontSendNotification);
        row.rangeLabel.setJustificationType(juce::Justification::centred);
        row.rangeLabel.setFont(juce::Font(10.0f));
        addAndMakeVisible(row.rangeLabel);
    }
}

MIDICCMapperEditor::~MIDICCMapperEditor() {}

void MIDICCMapperEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff1e1e1e));

    g.setColour(juce::Colour(0xff00d4ff));
    g.setFont(28.0f);
    g.drawText("MIDI CC Mapper", getLocalBounds().removeFromTop(50), juce::Justification::centred);

    g.setColour(juce::Colour(0xff404040));
    g.drawRect(getLocalBounds().reduced(10), 2);

    // Draw table lines
    g.setColour(juce::Colour(0xff303030));
    auto bounds = getLocalBounds().reduced(20);
    bounds.removeFromTop(50);
    bounds.removeFromTop(20);
    int headerY = bounds.getY() + 30;

    // Horizontal line under headers
    g.drawLine(20.0f, (float)headerY, (float)getWidth() - 20.0f, (float)headerY, 1.0f);

    // Horizontal lines between rows
    for (int i = 1; i < 8; ++i) {
        int y = headerY + (i * 40);
        g.setColour(juce::Colour(0xff252525));
        g.drawLine(20.0f, (float)y, (float)getWidth() - 20.0f, (float)y, 1.0f);
    }
}

void MIDICCMapperEditor::resized() {
    auto bounds = getLocalBounds().reduced(20);
    bounds.removeFromTop(50); // Title
    bounds.removeFromTop(20);

    // Header row
    auto headerRow = bounds.removeFromTop(30);
    int colWidths[] = {40, 80, 80, 80, 120, 80};

    for (int i = 0; i < 6; ++i) {
        headerLabels_[i].setBounds(headerRow.removeFromLeft(colWidths[i]));
    }

    // Mapping rows
    for (int i = 0; i < 8; ++i) {
        auto row = bounds.removeFromTop(40);
        auto& mapping = rows_[i];

        mapping.indexLabel.setBounds(row.removeFromLeft(colWidths[0]));
        mapping.statusLabel.setBounds(row.removeFromLeft(colWidths[1]));
        mapping.inputLabel.setBounds(row.removeFromLeft(colWidths[2]));
        mapping.outputLabel.setBounds(row.removeFromLeft(colWidths[3]));
        mapping.rangeLabel.setBounds(row.removeFromLeft(colWidths[4]));
    }
}
