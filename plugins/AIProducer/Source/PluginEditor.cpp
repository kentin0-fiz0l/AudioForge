#include "PluginEditor.h"

AIProducerEditor::AIProducerEditor(AIProducerProcessor& p)
    : AudioProcessorEditor(&p), processor_(p) {

    // Generate button
    generateButton_.setButtonText("🎵 Generate Track");
    generateButton_.onClick = [this] { generateButtonClicked(); };
    addAndMakeVisible(generateButton_);

    // Prompt label and editor
    promptLabel_.setText("Describe your track:", juce::dontSendNotification);
    promptLabel_.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(promptLabel_);

    promptEditor_.setMultiLine(true);
    promptEditor_.setReturnKeyStartsNewLine(true);
    promptEditor_.setText("Create a progressive house track with energetic drops and emotional breaks");
    addAndMakeVisible(promptEditor_);

    // Status label
    statusLabel_.setText("Ready to generate", juce::dontSendNotification);
    statusLabel_.setJustificationType(juce::Justification::centred);
    statusLabel_.setFont(juce::Font(14.0f));
    addAndMakeVisible(statusLabel_);

    // Info labels
    infoLabel1_.setText("AI Producer will create a complete track arrangement with:", juce::dontSendNotification);
    infoLabel1_.setJustificationType(juce::Justification::centred);
    infoLabel1_.setFont(juce::Font(12.0f));
    addAndMakeVisible(infoLabel1_);

    infoLabel2_.setText("• Drums  • Bass  • Chords  • Lead Melody  • Full Arrangement", juce::dontSendNotification);
    infoLabel2_.setJustificationType(juce::Justification::centred);
    infoLabel2_.setFont(juce::Font(12.0f));
    addAndMakeVisible(infoLabel2_);

    startTimerHz(10); // Update status 10 times per second
    setSize(600, 400);
}

AIProducerEditor::~AIProducerEditor() {
    stopTimer();
}

void AIProducerEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff1a1a2e)); // Dark blue background

    // Title
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(28.0f, juce::Font::bold));
    g.drawText("AI Producer", getLocalBounds().removeFromTop(60),
               juce::Justification::centred);

    // Border
    g.setColour(juce::Colour(0xff16213e));
    g.drawRect(getLocalBounds(), 2);
}

void AIProducerEditor::resized() {
    auto bounds = getLocalBounds().reduced(20);
    bounds.removeFromTop(60); // Title space

    // Info labels
    infoLabel1_.setBounds(bounds.removeFromTop(25));
    infoLabel2_.setBounds(bounds.removeFromTop(25));
    bounds.removeFromTop(10);

    // Prompt section
    promptLabel_.setBounds(bounds.removeFromTop(30));
    promptEditor_.setBounds(bounds.removeFromTop(100));
    bounds.removeFromTop(20);

    // Generate button (large and prominent)
    generateButton_.setBounds(bounds.removeFromTop(60).reduced(100, 0));
    bounds.removeFromTop(20);

    // Status
    statusLabel_.setBounds(bounds.removeFromTop(30));
}

void AIProducerEditor::timerCallback() {
    // Update status from processor
    juce::String newStatus = processor_.getStatus();
    if (statusLabel_.getText() != newStatus) {
        statusLabel_.setText(newStatus, juce::dontSendNotification);
    }

    // Update button state
    generateButton_.setEnabled(!processor_.isGenerating());
}

void AIProducerEditor::generateButtonClicked() {
    juce::String prompt = promptEditor_.getText();
    if (prompt.isEmpty()) {
        prompt = "Create a house track";
    }

    processor_.generateTrack(prompt);
}
