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

    // Key, mode and genre selectors
    setUpSelector (keySelector_, "key", keyAttachment_);
    setUpSelector (modeSelector_, "mode", modeAttachment_);
    setUpSelector (genreSelector_, "genre", genreAttachment_);

    // Export button
    exportButton_.setButtonText ("Export MIDI");
    exportButton_.addListener (this);
    addAndMakeVisible (exportButton_);

    // Key display
    keyLabel_.setText ("Key:", juce::dontSendNotification);
    keyLabel_.setFont (juce::Font (18.0f));
    addAndMakeVisible (keyLabel_);

    // Create chord labels (4 chords)
    for (int i = 0; i < ChordGeniusProcessor::kChordsPerProgression; ++i)
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

    showProgression();
}

ChordGeniusEditor::~ChordGeniusEditor()
{
}

void ChordGeniusEditor::setUpSelector (juce::ComboBox& selector, const juce::String& paramID,
                                       std::unique_ptr<ComboBoxAttachment>& attachment)
{
    auto& state = processor_.getValueTreeState();
    if (auto* choice = dynamic_cast<juce::AudioParameterChoice*> (state.getParameter (paramID)))
        selector.addItemList (choice->choices, 1);

    attachment = std::make_unique<ComboBoxAttachment> (state, paramID, selector);
    selector.onChange = [this] { showProgression(); };
    addAndMakeVisible (selector);
}

void ChordGeniusEditor::showProgression()
{
    const auto progression = processor_.getCurrentProgression();
    for (size_t i = 0; i < progression.size() && i < chordLabels_.size(); ++i)
    {
        chordLabels_[i]->setText (progression[i].name, juce::dontSendNotification);
        romanLabels_[i]->setText (progression[i].romanNumeral, juce::dontSendNotification);
    }
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
    keyLabel_.setBounds (20, 70, 50, 30);
    keySelector_.setBounds (70, 70, 80, 30);
    modeSelector_.setBounds (160, 70, 100, 30);
    genreSelector_.setBounds (getWidth() - 220, 70, 200, 30);
    exportButton_.setBounds ((getWidth() - 200) / 2, 320, 200, 35);

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
    if (button == &exportButton_) {
        // Kept as a member: an async chooser closes as soon as it is destroyed
        fileChooser_ = std::make_unique<juce::FileChooser>(
            "Export MIDI File",
            juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
            "*.mid"
        );

        auto chooserFlags = juce::FileBrowserComponent::saveMode |
                           juce::FileBrowserComponent::canSelectFiles;

        fileChooser_->launchAsync(chooserFlags, [this](const juce::FileChooser& chooser) {
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
