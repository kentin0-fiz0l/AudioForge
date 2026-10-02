/*
  PluginEditor.cpp - UI Implementation
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

MelodyMakerEditor::MelodyMakerEditor (MelodyMakerProcessor& p)
    : AudioProcessorEditor (&p), processor_ (p)
{
    setSize (700, 550);

    // Title
    titleLabel_.setText ("MelodyMaker", juce::dontSendNotification);
    titleLabel_.setFont (juce::Font (32.0f, juce::Font::bold));
    titleLabel_.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (titleLabel_);

    // Style selector
    styleSelector_.addItem ("Pop", 1);
    styleSelector_.addItem ("Rock", 2);
    styleSelector_.addItem ("Jazz", 3);
    styleSelector_.addItem ("Blues", 4);
    styleSelector_.addItem ("EDM", 5);
    styleSelector_.addItem ("Ballad", 6);
    styleSelector_.addItem ("Funk", 7);
    styleSelector_.addItem ("Classical", 8);
    styleSelector_.setSelectedId (1);
    addAndMakeVisible (styleSelector_);

    // Key selector (root note)
    keySelector_.addItem ("C", 1);
    keySelector_.addItem ("C#", 2);
    keySelector_.addItem ("D", 3);
    keySelector_.addItem ("D#", 4);
    keySelector_.addItem ("E", 5);
    keySelector_.addItem ("F", 6);
    keySelector_.addItem ("F#", 7);
    keySelector_.addItem ("G", 8);
    keySelector_.addItem ("G#", 9);
    keySelector_.addItem ("A", 10);
    keySelector_.addItem ("A#", 11);
    keySelector_.addItem ("B", 12);
    keySelector_.setSelectedId (1); // C
    keySelector_.addListener (this);
    addAndMakeVisible (keySelector_);

    // Scale selector (Major/Minor)
    scaleSelector_.addItem ("Major", 1);
    scaleSelector_.addItem ("Minor", 2);
    scaleSelector_.setSelectedId (1); // Major
    scaleSelector_.addListener (this);
    addAndMakeVisible (scaleSelector_);

    // Generate button
    generateButton_.setButtonText ("Generate Melody");
    generateButton_.addListener (this);
    addAndMakeVisible (generateButton_);

    // Export button
    exportButton_.setButtonText ("Export MIDI");
    exportButton_.addListener (this);
    addAndMakeVisible (exportButton_);

    // Stats label
    statsLabel_.setText ("8 notes | C Major", juce::dontSendNotification);
    statsLabel_.setFont (juce::Font (14.0f));
    addAndMakeVisible (statsLabel_);

    // Waveform selector
    waveformLabel_.setText ("Wave:", juce::dontSendNotification);
    waveformLabel_.setFont (juce::Font (12.0f));
    addAndMakeVisible (waveformLabel_);

    waveformSelector_.addItem ("Sine", 1);
    waveformSelector_.addItem ("Saw", 2);
    waveformSelector_.addItem ("Square", 3);
    waveformSelector_.addItem ("Triangle", 4);
    waveformSelector_.setSelectedId (1);
    waveformSelector_.addListener (this);
    addAndMakeVisible (waveformSelector_);

    // ADSR label
    adsrLabel_.setText ("ADSR", juce::dontSendNotification);
    adsrLabel_.setFont (juce::Font (14.0f, juce::Font::bold));
    addAndMakeVisible (adsrLabel_);

    // ADSR sliders
    attackSlider_.setRange (0.001, 2.0, 0.001);
    attackSlider_.setValue (0.01);
    attackSlider_.setSliderStyle (juce::Slider::Rotary);
    attackSlider_.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 50, 20);
    attackSlider_.addListener (this);
    addAndMakeVisible (attackSlider_);

    decaySlider_.setRange (0.001, 2.0, 0.001);
    decaySlider_.setValue (0.1);
    decaySlider_.setSliderStyle (juce::Slider::Rotary);
    decaySlider_.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 50, 20);
    decaySlider_.addListener (this);
    addAndMakeVisible (decaySlider_);

    sustainSlider_.setRange (0.0, 1.0, 0.01);
    sustainSlider_.setValue (0.7);
    sustainSlider_.setSliderStyle (juce::Slider::Rotary);
    sustainSlider_.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 50, 20);
    sustainSlider_.addListener (this);
    addAndMakeVisible (sustainSlider_);

    releaseSlider_.setRange (0.001, 3.0, 0.001);
    releaseSlider_.setValue (0.3);
    releaseSlider_.setSliderStyle (juce::Slider::Rotary);
    releaseSlider_.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 50, 20);
    releaseSlider_.addListener (this);
    addAndMakeVisible (releaseSlider_);

    // ADSR knob labels
    attackLabel_.setText ("A", juce::dontSendNotification);
    attackLabel_.setFont (juce::Font (11.0f));
    attackLabel_.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (attackLabel_);

    decayLabel_.setText ("D", juce::dontSendNotification);
    decayLabel_.setFont (juce::Font (11.0f));
    decayLabel_.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (decayLabel_);

    sustainLabel_.setText ("S", juce::dontSendNotification);
    sustainLabel_.setFont (juce::Font (11.0f));
    sustainLabel_.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (sustainLabel_);

    releaseLabel_.setText ("R", juce::dontSendNotification);
    releaseLabel_.setFont (juce::Font (11.0f));
    releaseLabel_.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (releaseLabel_);

    // Effects label
    effectsLabel_.setText ("EFFECTS", juce::dontSendNotification);
    effectsLabel_.setFont (juce::Font (14.0f, juce::Font::bold));
    addAndMakeVisible (effectsLabel_);

    // Reverb slider
    reverbSlider_.setRange (0.0, 1.0, 0.01);
    reverbSlider_.setValue (0.0);
    reverbSlider_.setSliderStyle (juce::Slider::Rotary);
    reverbSlider_.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 50, 20);
    reverbSlider_.addListener (this);
    addAndMakeVisible (reverbSlider_);

    reverbLabel_.setText ("Reverb", juce::dontSendNotification);
    reverbLabel_.setFont (juce::Font (11.0f));
    reverbLabel_.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (reverbLabel_);

    // Delay Mix slider
    delayMixSlider_.setRange (0.0, 1.0, 0.01);
    delayMixSlider_.setValue (0.0);
    delayMixSlider_.setSliderStyle (juce::Slider::Rotary);
    delayMixSlider_.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 50, 20);
    delayMixSlider_.addListener (this);
    addAndMakeVisible (delayMixSlider_);

    delayLabel_.setText ("Delay", juce::dontSendNotification);
    delayLabel_.setFont (juce::Font (11.0f));
    delayLabel_.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (delayLabel_);

    // Filter Cutoff slider
    filterCutoffSlider_.setRange (200.0, 20000.0, 1.0);
    filterCutoffSlider_.setValue (20000.0);
    filterCutoffSlider_.setSliderStyle (juce::Slider::Rotary);
    filterCutoffSlider_.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 50, 20);
    filterCutoffSlider_.setSkewFactorFromMidPoint (2000.0);
    filterCutoffSlider_.addListener (this);
    addAndMakeVisible (filterCutoffSlider_);

    filterLabel_.setText ("Filter", juce::dontSendNotification);
    filterLabel_.setFont (juce::Font (11.0f));
    filterLabel_.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (filterLabel_);

    // Start timer for animation (optional)
    startTimer (100);
}

MelodyMakerEditor::~MelodyMakerEditor()
{
    stopTimer();
}

void MelodyMakerEditor::paint (juce::Graphics& g)
{
    // Dark background
    g.fillAll (juce::Colour (0xff1a1a2e));

    // Draw melody visualization area
    g.setColour (juce::Colour (0xff16213e));
    g.fillRoundedRectangle (20, 120, getWidth() - 40, 280, 10.0f);

    // Draw piano roll grid
    g.setColour (juce::Colour (0xff0f3460).withAlpha(0.3f));
    for (int i = 0; i < 12; ++i) {
        int y = 120 + (i * 280 / 12);
        g.drawLine (20, y, getWidth() - 20, y, 1.0f);
    }

    // Draw the melody
    drawMelody (g, processor_.getCurrentMelody());
}

void MelodyMakerEditor::resized()
{
    titleLabel_.setBounds (0, 20, getWidth(), 40);

    // Left side: Style
    styleSelector_.setBounds (20, 70, 140, 30);

    // Center: Key selection
    keySelector_.setBounds (220, 70, 60, 30);
    scaleSelector_.setBounds (290, 70, 100, 30);

    // Right side: Stats
    statsLabel_.setBounds (getWidth() - 200, 70, 180, 30);

    // Sound controls area (below key selection)
    int soundControlsY = 410;
    waveformLabel_.setBounds (20, soundControlsY, 50, 20);
    waveformSelector_.setBounds (75, soundControlsY, 120, 24);

    // ADSR label and knobs
    adsrLabel_.setBounds (220, soundControlsY - 5, 60, 30);
    attackSlider_.setBounds (290, soundControlsY - 30, 70, 70);
    decaySlider_.setBounds (365, soundControlsY - 30, 70, 70);
    sustainSlider_.setBounds (440, soundControlsY - 30, 70, 70);
    releaseSlider_.setBounds (515, soundControlsY - 30, 70, 70);

    // ADSR knob labels (above each knob)
    attackLabel_.setBounds (290, soundControlsY - 50, 70, 20);
    decayLabel_.setBounds (365, soundControlsY - 50, 70, 20);
    sustainLabel_.setBounds (440, soundControlsY - 50, 70, 20);
    releaseLabel_.setBounds (515, soundControlsY - 50, 70, 20);

    // Effects controls (below ADSR)
    int effectsY = 480;
    effectsLabel_.setBounds (20, effectsY - 5, 80, 30);
    reverbSlider_.setBounds (110, effectsY - 30, 70, 70);
    delayMixSlider_.setBounds (200, effectsY - 30, 70, 70);
    filterCutoffSlider_.setBounds (290, effectsY - 30, 70, 70);

    // Effects knob labels (above each knob)
    reverbLabel_.setBounds (110, effectsY - 50, 70, 20);
    delayLabel_.setBounds (200, effectsY - 50, 70, 20);
    filterLabel_.setBounds (290, effectsY - 50, 70, 20);

    // Generate and Export buttons
    generateButton_.setBounds ((getWidth() - 420) / 2, 500, 200, 35);
    exportButton_.setBounds ((getWidth() - 420) / 2 + 220, 500, 200, 35);
}

void MelodyMakerEditor::timerCallback()
{
    // Repaint for any animations
    repaint();
}

void MelodyMakerEditor::sliderValueChanged (juce::Slider* slider)
{
    if (slider == &attackSlider_)
        processor_.setAttack((float)attackSlider_.getValue());
    else if (slider == &decaySlider_)
        processor_.setDecay((float)decaySlider_.getValue());
    else if (slider == &sustainSlider_)
        processor_.setSustain((float)sustainSlider_.getValue());
    else if (slider == &releaseSlider_)
        processor_.setRelease((float)releaseSlider_.getValue());
    else if (slider == &reverbSlider_)
        processor_.setReverbMix((float)reverbSlider_.getValue());
    else if (slider == &delayMixSlider_)
        processor_.setDelayMix((float)delayMixSlider_.getValue());
    else if (slider == &filterCutoffSlider_)
        processor_.setFilterCutoff((float)filterCutoffSlider_.getValue());
}

void MelodyMakerEditor::comboBoxChanged (juce::ComboBox* comboBox)
{
    if (comboBox == &waveformSelector_)
    {
        processor_.setWaveform(waveformSelector_.getSelectedId() - 1);
    }
    else if (comboBox == &keySelector_ || comboBox == &scaleSelector_)
    {
        // Get selected key (C=0, C#=1, D=2, etc.)
        int keyId = keySelector_.getSelectedId() - 1; // Convert to 0-11
        int rootNote = 60 + keyId; // C4 = 60

        // Get selected scale
        bool isMajor = (scaleSelector_.getSelectedId() == 1);

        // Update processor
        processor_.setKey(rootNote, isMajor);

        // Auto-generate new melody in the new key
        int styleId = styleSelector_.getSelectedId();
        std::string style = "Pop";
        switch (styleId) {
            case 1: style = "Pop"; break;
            case 2: style = "Rock"; break;
            case 3: style = "Jazz"; break;
            case 4: style = "Blues"; break;
            case 5: style = "EDM"; break;
            case 6: style = "Ballad"; break;
            case 7: style = "Funk"; break;
            case 8: style = "Classical"; break;
        }

        processor_.generateNewMelody(style, 4);

        // Update stats display
        auto& melody = processor_.getCurrentMelody();
        juce::String keyName = keySelector_.getText();
        juce::String scaleName = scaleSelector_.getText();
        juce::String stats = juce::String(melody.size()) + " notes | " + keyName + " " + scaleName;
        statsLabel_.setText (stats, juce::dontSendNotification);

        repaint();
    }
}

void MelodyMakerEditor::buttonClicked (juce::Button* button)
{
    if (button == &generateButton_)
    {
        // Get selected style
        int selectedId = styleSelector_.getSelectedId();
        std::string style = "Pop";

        switch (selectedId)
        {
            case 1: style = "Pop"; break;
            case 2: style = "Rock"; break;
            case 3: style = "Jazz"; break;
            case 4: style = "Blues"; break;
            case 5: style = "EDM"; break;
            case 6: style = "Ballad"; break;
            case 7: style = "Funk"; break;
            case 8: style = "Classical"; break;
        }

        // Generate new melody
        processor_.generateNewMelody(style, 4);

        // Update stats
        auto& melody = processor_.getCurrentMelody();
        juce::String keyName = keySelector_.getText();
        juce::String scaleName = scaleSelector_.getText();
        juce::String stats = juce::String(melody.size()) + " notes | " + keyName + " " + scaleName;
        statsLabel_.setText (stats, juce::dontSendNotification);

        repaint();
    }
    else if (button == &exportButton_)
    {
        // Export MIDI dialog
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

void MelodyMakerEditor::drawMelody (juce::Graphics& g,
                                     const std::vector<MelodyEngine::Note>& melody)
{
    if (melody.empty()) return;

    // Find pitch range for scaling
    int minPitch = 127;
    int maxPitch = 0;
    float maxTime = 0.0f;

    for (const auto& note : melody) {
        minPitch = std::min(minPitch, note.pitch);
        maxPitch = std::max(maxPitch, note.pitch);
        maxTime = std::max(maxTime, note.startTime + note.duration);
    }

    if (maxPitch == minPitch) maxPitch = minPitch + 12;

    float areaWidth = getWidth() - 60.0f;
    float areaHeight = 260.0f;
    float xOffset = 30.0f;
    float yOffset = 130.0f;

    // Draw notes as rectangles
    g.setColour (juce::Colour (0xff00d4ff)); // Bright cyan

    for (const auto& note : melody) {
        float x = xOffset + (note.startTime / maxTime) * areaWidth;
        float width = (note.duration / maxTime) * areaWidth;

        float normalizedPitch = (note.pitch - minPitch) / static_cast<float>(maxPitch - minPitch);
        float y = yOffset + areaHeight - (normalizedPitch * areaHeight);
        float height = 8.0f;

        g.fillRoundedRectangle (x, y, width, height, 3.0f);

        // Draw velocity as alpha
        float alpha = note.velocity / 127.0f;
        g.setColour (juce::Colour (0xff00d4ff).withAlpha(alpha));
    }
}
