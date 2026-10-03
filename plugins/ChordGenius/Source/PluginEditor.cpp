/*
  PluginEditor.cpp - UI Implementation
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

ChordGeniusEditor::ChordGeniusEditor (ChordGeniusProcessor& p)
    : AudioProcessorEditor (&p), processor_ (p)
{
    setSize (600, 400);

    // Title
    titleLabel_.setText ("ChordGenius", juce::dontSendNotification);
    titleLabel_.setFont (juce::Font (32.0f, juce::Font::bold));
    titleLabel_.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (titleLabel_);

    // Genre selector
    genreSelector_.addItem ("Pop", 1);
    genreSelector_.addItem ("Rock", 2);
    genreSelector_.addItem ("Jazz", 3);
    genreSelector_.addItem ("Blues", 4);
    genreSelector_.addItem ("EDM", 5);
    genreSelector_.addItem ("Emotional", 6);
    genreSelector_.addItem ("Happy", 7);
    genreSelector_.addItem ("Sad", 8);
    genreSelector_.setSelectedId (1);
    addAndMakeVisible (genreSelector_);

    // Generate button
    generateButton_.setButtonText ("Generate Progression");
    generateButton_.addListener (this);
    addAndMakeVisible (generateButton_);

    // Export button
    exportButton_.setButtonText ("Export MIDI");
    exportButton_.addListener (this);
    addAndMakeVisible (exportButton_);

    // Key display
    keyLabel_.setText ("Key: C Major", juce::dontSendNotification);
    keyLabel_.setFont (juce::Font (18.0f));
    addAndMakeVisible (keyLabel_);

    // Create chord labels (4 chords)
    for (int i = 0; i < 4; ++i)
    {
        auto chordLabel = std::make_unique<juce::Label>();
        chordLabel->setFont (juce::Font (24.0f, juce::Font::bold));
        chordLabel->setJustificationType (juce::Justification::centred);
        chordLabel->setText ("---", juce::dontSendNotification);
        addAndMakeVisible (chordLabel.get());
        chordLabels_.push_back (std::move(chordLabel));

        auto romanLabel = std::make_unique<juce::Label>();
        romanLabel->setFont (juce::Font (16.0f));
        romanLabel->setJustificationType (juce::Justification::centred);
        romanLabel->setText ("---", juce::dontSendNotification);
        addAndMakeVisible (romanLabel.get());
        romanLabels_.push_back (std::move(romanLabel));
    }

    // Display initial progression
    auto& progression = processor_.getCurrentProgression();
    for (size_t i = 0; i < progression.size() && i < chordLabels_.size(); ++i)
    {
        chordLabels_[i]->setText (progression[i].name, juce::dontSendNotification);
        romanLabels_[i]->setText (progression[i].romanNumeral, juce::dontSendNotification);
    }
}

ChordGeniusEditor::~ChordGeniusEditor()
{
}

void ChordGeniusEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff1a1a2e));

    // Draw chord boxes
    g.setColour (juce::Colour (0xff16213e));
    for (int i = 0; i < 4; ++i)
    {
        int x = 50 + i * 130;
        g.fillRoundedRectangle (x, 150, 120, 100, 10.0f);
    }
}

void ChordGeniusEditor::resized()
{
    titleLabel_.setBounds (0, 20, getWidth(), 40);
    keyLabel_.setBounds (20, 70, 200, 30);
    genreSelector_.setBounds (getWidth() - 220, 70, 200, 30);
    // Generate and Export buttons
    generateButton_.setBounds ((getWidth() - 420) / 2, 320, 200, 35);
    exportButton_.setBounds ((getWidth() - 420) / 2 + 220, 320, 200, 35);

    // Position chord labels
    for (int i = 0; i < 4; ++i)
    {
        int x = 50 + i * 130;
        chordLabels_[i]->setBounds (x, 170, 120, 40);
        romanLabels_[i]->setBounds (x, 210, 120, 30);
    }
}

void ChordGeniusEditor::buttonClicked (juce::Button* button)
{
    if (button == &generateButton_)
    {
        // Get selected genre
        int selectedId = genreSelector_.getSelectedId();
        std::string genre = "pop";

        switch (selectedId)
        {
            case 1: genre = "pop"; break;
            case 2: genre = "rock"; break;
            case 3: genre = "jazz"; break;
            case 4: genre = "blues"; break;
            case 5: genre = "edm"; break;
            case 6: genre = "emotional"; break;
            case 7: genre = "happy"; break;
            case 8: genre = "sad"; break;
        }

        // Generate new progression
        processor_.generateNewProgression(genre);

        // Update display
        auto& progression = processor_.getCurrentProgression();
        for (size_t i = 0; i < progression.size() && i < chordLabels_.size(); ++i)
        {
            chordLabels_[i]->setText (progression[i].name, juce::dontSendNotification);
            romanLabels_[i]->setText (progression[i].romanNumeral, juce::dontSendNotification);
        }
    }
    else if (button == &exportButton_) {
        auto fileChooser = std::make_unique<juce::FileChooser>(
            "Export MIDI File",
            juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
            "*.mid"
        );

        auto chooserFlags = juce::FileBrowserComponent::saveMode |
                           juce::FileBrowserComponent::canSelectFiles;

        fileChooser->launchAsync(chooserFlags, [this](const juce::FileChooser& chooser) {
            auto file = chooser.getResult();
            if (file != juce::File()) {
                if (!file.hasFileExtension(".mid"))
                    file = file.withFileExtension(".mid");

                processor_.exportToMidiFile(file);

                juce::AlertWindow::showMessageBoxAsync(
                    juce::AlertWindow::InfoIcon,
                    "Export Complete",
                    "MIDI file saved to:\n" + file.getFullPathName()
                );
            }
        });
    }
}
