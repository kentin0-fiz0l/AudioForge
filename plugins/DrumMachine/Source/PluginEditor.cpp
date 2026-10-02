/*
  PluginEditor.cpp - UI Implementation
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

DrumMachineEditor::DrumMachineEditor (DrumMachineProcessor& p)
    : AudioProcessorEditor (&p), processor_ (p)
{
    setSize (800, 550);

    // Title
    titleLabel_.setText ("DrumMachine", juce::dontSendNotification);
    titleLabel_.setFont (juce::Font (32.0f, juce::Font::bold));
    titleLabel_.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (titleLabel_);

    // Style selector
    styleSelector_.addItem ("Rock", 1);
    styleSelector_.addItem ("Hip-Hop", 2);
    styleSelector_.addItem ("Jazz", 3);
    styleSelector_.addItem ("EDM", 4);
    styleSelector_.addItem ("Funk", 5);
    styleSelector_.addItem ("Latin", 6);
    styleSelector_.addItem ("Blues", 7);
    styleSelector_.setSelectedId (1);
    styleSelector_.addListener (this);
    addAndMakeVisible (styleSelector_);

    // Complexity slider
    complexityLabel_.setText ("Complexity", juce::dontSendNotification);
    complexityLabel_.setFont (juce::Font (12.0f));
    addAndMakeVisible (complexityLabel_);

    complexitySlider_.setRange (1, 10, 1);
    complexitySlider_.setValue (5);
    complexitySlider_.setSliderStyle (juce::Slider::Rotary);
    complexitySlider_.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 50, 20);
    complexitySlider_.addListener (this);
    addAndMakeVisible (complexitySlider_);

    // Swing slider
    swingLabel_.setText ("Swing", juce::dontSendNotification);
    swingLabel_.setFont (juce::Font (12.0f));
    addAndMakeVisible (swingLabel_);

    swingSlider_.setRange (0.0, 1.0, 0.01);
    swingSlider_.setValue (0.0);
    swingSlider_.setSliderStyle (juce::Slider::Rotary);
    swingSlider_.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 50, 20);
    swingSlider_.addListener (this);
    addAndMakeVisible (swingSlider_);

    // Fills toggle
    fillsLabel_.setText ("Fills", juce::dontSendNotification);
    fillsLabel_.setFont (juce::Font (12.0f));
    addAndMakeVisible (fillsLabel_);

    fillsToggle_.setButtonText ("");
    fillsToggle_.addListener (this);
    addAndMakeVisible (fillsToggle_);

    // Generate button
    generateButton_.setButtonText ("Generate Pattern");
    generateButton_.addListener (this);
    addAndMakeVisible (generateButton_);

    // Clear button
    clearButton_.setButtonText ("Clear Pattern");
    clearButton_.addListener (this);
    addAndMakeVisible (clearButton_);

    // Export button
    exportButton_.setButtonText ("Export MIDI");
    exportButton_.addListener (this);
    addAndMakeVisible (exportButton_);

    // Edit label
    editLabel_.setText ("Click grid to edit pattern", juce::dontSendNotification);
    editLabel_.setFont (juce::Font (11.0f, juce::Font::italic));
    editLabel_.setColour (juce::Label::textColourId, juce::Colour (0xff888888));
    addAndMakeVisible (editLabel_);

    // Stats label
    statsLabel_.setText ("Rock | 4 bars | 16 steps", juce::dontSendNotification);
    statsLabel_.setFont (juce::Font (14.0f));
    addAndMakeVisible (statsLabel_);

    // Start timer for animation
    startTimer (100);
}

DrumMachineEditor::~DrumMachineEditor()
{
}

void DrumMachineEditor::paint (juce::Graphics& g)
{
    // Background gradient
    g.fillAll (juce::Colour (0xff1e1e2e));

    juce::ColourGradient gradient (
        juce::Colour (0xff2d2d44), 0, 0,
        juce::Colour (0xff1e1e2e), 0, static_cast<float>(getHeight()), false
    );
    g.setGradientFill (gradient);
    g.fillRect (getLocalBounds());

    // Pattern grid area
    g.setColour (juce::Colour (0xff16213e));
    g.fillRoundedRectangle (20, 120, getWidth() - 40, 300, 8.0f);

    // Draw pattern grid
    drawPatternGrid (g, processor_.getCurrentPattern());
}

void DrumMachineEditor::resized()
{
    titleLabel_.setBounds (0, 20, getWidth(), 40);

    // Left side: Style
    styleSelector_.setBounds (20, 70, 140, 30);

    // Center: Complexity and Swing
    complexityLabel_.setBounds (220, 50, 80, 20);
    complexitySlider_.setBounds (220, 70, 70, 70);

    swingLabel_.setBounds (310, 50, 60, 20);
    swingSlider_.setBounds (310, 70, 70, 70);

    // Right side: Fills toggle
    fillsLabel_.setBounds (400, 70, 50, 30);
    fillsToggle_.setBounds (455, 75, 30, 20);

    // Stats
    statsLabel_.setBounds (getWidth() - 240, 70, 220, 30);

    // Edit label (above grid)
    editLabel_.setBounds (20, 100, 200, 20);

    // Generate, Clear, and Export buttons
    int buttonY = 490;
    generateButton_.setBounds (20, buttonY, 180, 35);
    clearButton_.setBounds (220, buttonY, 180, 35);
    exportButton_.setBounds (420, buttonY, 180, 35);

    // Update grid width for hit detection
    gridWidth_ = static_cast<float>(getWidth() - 80);
}

void DrumMachineEditor::timerCallback()
{
    // Repaint for visualization updates
    repaint();
}

void DrumMachineEditor::sliderValueChanged (juce::Slider* slider)
{
    if (slider == &complexitySlider_) {
        processor_.setComplexity(static_cast<int>(complexitySlider_.getValue()));
    }
    else if (slider == &swingSlider_) {
        processor_.setSwing(static_cast<float>(swingSlider_.getValue()));
    }
}

void DrumMachineEditor::comboBoxChanged (juce::ComboBox* comboBox)
{
    if (comboBox == &styleSelector_) {
        // Auto-generate on style change
        int styleId = styleSelector_.getSelectedId();
        std::string style = "Rock";

        switch (styleId) {
            case 1: style = "Rock"; break;
            case 2: style = "Hip-Hop"; break;
            case 3: style = "Jazz"; break;
            case 4: style = "EDM"; break;
            case 5: style = "Funk"; break;
            case 6: style = "Latin"; break;
            case 7: style = "Blues"; break;
        }

        processor_.generateNewPattern(style);

        juce::String stats = juce::String(style) + " | 4 bars | 16 steps";
        statsLabel_.setText (stats, juce::dontSendNotification);

        repaint();
    }
}

void DrumMachineEditor::buttonClicked (juce::Button* button)
{
    if (button == &generateButton_) {
        int styleId = styleSelector_.getSelectedId();
        std::string style = "Rock";

        switch (styleId) {
            case 1: style = "Rock"; break;
            case 2: style = "Hip-Hop"; break;
            case 3: style = "Jazz"; break;
            case 4: style = "EDM"; break;
            case 5: style = "Funk"; break;
            case 6: style = "Latin"; break;
            case 7: style = "Blues"; break;
        }

        processor_.generateNewPattern(style);

        juce::String stats = juce::String(style) + " | 4 bars | 16 steps";
        statsLabel_.setText (stats, juce::dontSendNotification);

        repaint();
    }
    else if (button == &clearButton_) {
        // Clear all hits from the pattern
        auto& pattern = processor_.getCurrentPattern();
        for (auto& voiceHits : pattern.voices) {
            voiceHits.clear();
        }
        repaint();
    }
    else if (button == &exportButton_) {
        // Open file save dialog
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
                // Add .mid extension if not present
                if (!file.hasFileExtension(".mid"))
                    file = file.withFileExtension(".mid");

                processor_.exportToMidiFile(file);

                // Show success message
                juce::AlertWindow::showMessageBoxAsync(
                    juce::AlertWindow::InfoIcon,
                    "Export Complete",
                    "MIDI file saved to:\n" + file.getFullPathName()
                );
            }
        });
    }
    else if (button == &fillsToggle_) {
        processor_.setFillsEnabled(fillsToggle_.getToggleState());
    }
}

void DrumMachineEditor::drawPatternGrid (juce::Graphics& g, const DrumEngine::DrumPattern& pattern)
{
    float gridX = 40;
    float gridY = 140;
    float gridWidth = static_cast<float>(getWidth() - 80);
    float gridHeight = 260.0f;

    int numSteps = pattern.numSteps;
    float stepWidth = gridWidth / static_cast<float>(numSteps);
    float voiceHeight = gridHeight / static_cast<float>(DrumEngine::NUM_VOICES);

    // Voice labels
    const char* voiceNames[] = {"KICK", "SNARE", "HIHAT", "TOM", "CRASH"};
    g.setColour (juce::Colour (0xffaaaaaa));
    g.setFont (juce::Font (11.0f));

    for (int v = 0; v < DrumEngine::NUM_VOICES; ++v) {
        float y = gridY + (v * voiceHeight);
        g.drawText (voiceNames[v], 5, static_cast<int>(y), 30, static_cast<int>(voiceHeight),
                    juce::Justification::centredRight);
    }

    // Draw grid lines
    g.setColour (juce::Colour (0xff0f3460).withAlpha(0.3f));

    // Horizontal lines
    for (int v = 0; v <= DrumEngine::NUM_VOICES; ++v) {
        float y = gridY + (v * voiceHeight);
        g.drawLine (gridX, y, gridX + gridWidth, y, 1.0f);
    }

    // Vertical lines (emphasize every 4 steps)
    for (int s = 0; s <= numSteps; ++s) {
        float x = gridX + (s * stepWidth);
        float thickness = (s % 4 == 0) ? 2.0f : 1.0f;
        auto color = (s % 4 == 0) ? juce::Colour (0xff0f3460).withAlpha(0.6f)
                                   : juce::Colour (0xff0f3460).withAlpha(0.2f);
        g.setColour (color);
        g.drawLine (x, gridY, x, gridY + gridHeight, thickness);
    }

    // Draw hits
    for (int v = 0; v < DrumEngine::NUM_VOICES; ++v) {
        const auto& voiceHits = pattern.voices[v];

        for (const auto& hit : voiceHits) {
            float x = gridX + (hit.step * stepWidth);
            float y = gridY + (v * voiceHeight);

            // Color based on voice
            juce::Colour hitColor;
            switch (hit.voice) {
                case DrumEngine::KICK:   hitColor = juce::Colour (0xffff6b35); break;  // Orange
                case DrumEngine::SNARE:  hitColor = juce::Colour (0xff4ecdc4); break;  // Teal
                case DrumEngine::HIHAT:  hitColor = juce::Colour (0xfff7c948); break;  // Yellow
                case DrumEngine::TOM:    hitColor = juce::Colour (0xffb565d8); break;  // Purple
                case DrumEngine::CRASH:  hitColor = juce::Colour (0xffff4757); break;  // Red
                default:                 hitColor = juce::Colour (0xffffffff); break;
            }

            // Velocity affects brightness
            hitColor = hitColor.withAlpha(hit.velocity);

            g.setColour (hitColor);
            g.fillEllipse (x + 4, y + (voiceHeight * 0.25f),
                          stepWidth - 8, voiceHeight * 0.5f);

            // Outline
            g.setColour (hitColor.brighter(0.3f));
            g.drawEllipse (x + 4, y + (voiceHeight * 0.25f),
                          stepWidth - 8, voiceHeight * 0.5f, 1.5f);
        }
    }
}

void DrumMachineEditor::getCellFromPosition (int x, int y, int& voice, int& step)
{
    // Convert mouse position to grid cell
    float relativeX = static_cast<float>(x) - gridX_;
    float relativeY = static_cast<float>(y) - gridY_;

    if (relativeX < 0 || relativeY < 0 || relativeX > gridWidth_ || relativeY > gridHeight_) {
        voice = -1;
        step = -1;
        return;
    }

    int numSteps = 16;
    float stepWidth = gridWidth_ / static_cast<float>(numSteps);
    float voiceHeight = gridHeight_ / static_cast<float>(DrumEngine::NUM_VOICES);

    step = static_cast<int>(relativeX / stepWidth);
    voice = static_cast<int>(relativeY / voiceHeight);

    // Bounds check
    if (step < 0 || step >= numSteps || voice < 0 || voice >= DrumEngine::NUM_VOICES) {
        voice = -1;
        step = -1;
    }
}

void DrumMachineEditor::toggleHitAtPosition (int x, int y)
{
    int voice, step;
    getCellFromPosition(x, y, voice, step);

    if (voice == -1 || step == -1)
        return;

    auto& pattern = processor_.getCurrentPattern();
    auto& voiceHits = pattern.voices[voice];

    // Check if hit already exists at this position
    bool hitExists = false;
    for (auto it = voiceHits.begin(); it != voiceHits.end(); ++it) {
        if (it->step == step) {
            // Remove existing hit
            voiceHits.erase(it);
            hitExists = true;
            break;
        }
    }

    // If no hit existed, add one
    if (!hitExists) {
        DrumEngine::DrumHit newHit;
        newHit.voice = static_cast<DrumEngine::DrumVoice>(voice);
        newHit.step = step;
        newHit.velocity = 0.8f;  // Default velocity
        voiceHits.push_back(newHit);
    }

    repaint();
}

void DrumMachineEditor::mouseDown (const juce::MouseEvent& event)
{
    toggleHitAtPosition(event.x, event.y);
}

void DrumMachineEditor::mouseDrag (const juce::MouseEvent& event)
{
    // Paint mode - add hits while dragging
    int voice, step;
    getCellFromPosition(event.x, event.y, voice, step);

    if (voice == -1 || step == -1)
        return;

    auto& pattern = processor_.getCurrentPattern();
    auto& voiceHits = pattern.voices[voice];

    // Check if hit already exists
    bool hitExists = false;
    for (const auto& hit : voiceHits) {
        if (hit.step == step) {
            hitExists = true;
            break;
        }
    }

    // Add hit if it doesn't exist
    if (!hitExists) {
        DrumEngine::DrumHit newHit;
        newHit.voice = static_cast<DrumEngine::DrumVoice>(voice);
        newHit.step = step;
        newHit.velocity = 0.8f;
        voiceHits.push_back(newHit);
        repaint();
    }
}
