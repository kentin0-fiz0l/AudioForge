/*
  PluginEditor.cpp - UI Implementation
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

BassLineEditor::BassLineEditor (BassLineProcessor& p)
    : AudioProcessorEditor (&p), processor_ (p)
{
    setSize (700, 550);

    // Title
    titleLabel_.setText ("BassLine Generator", juce::dontSendNotification);
    titleLabel_.setFont (juce::Font (32.0f, juce::Font::bold));
    titleLabel_.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (titleLabel_);

    // Style selector
    styleSelector_.addItem ("Rock", 1);
    styleSelector_.addItem ("Funk", 2);
    styleSelector_.addItem ("Jazz", 3);
    styleSelector_.addItem ("EDM", 4);
    styleSelector_.addItem ("Reggae", 5);
    styleSelector_.addItem ("Latin", 6);
    styleSelector_.addItem ("Blues", 7);
    styleSelector_.setSelectedId (1);
    styleSelector_.addListener (this);
    addAndMakeVisible (styleSelector_);

    // Key selector
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
    keySelector_.setSelectedId (1);
    keySelector_.addListener (this);
    addAndMakeVisible (keySelector_);

    // Scale selector
    scaleSelector_.addItem ("Major", 1);
    scaleSelector_.addItem ("Minor", 2);
    scaleSelector_.setSelectedId (1);
    scaleSelector_.addListener (this);
    addAndMakeVisible (scaleSelector_);

    // Octave selector
    octaveLabel_.setText ("Octave:", juce::dontSendNotification);
    octaveLabel_.setFont (juce::Font (12.0f));
    addAndMakeVisible (octaveLabel_);

    octaveSelector_.addItem ("C1 (Low)", 1);
    octaveSelector_.addItem ("C2 (Mid)", 2);
    octaveSelector_.addItem ("C3 (High)", 3);
    octaveSelector_.setSelectedId (2);  // Default C2
    octaveSelector_.addListener (this);
    addAndMakeVisible (octaveSelector_);

    // Waveform selector
    waveformLabel_.setText ("Wave:", juce::dontSendNotification);
    waveformLabel_.setFont (juce::Font (12.0f));
    addAndMakeVisible (waveformLabel_);

    waveformSelector_.addItem ("Sine (Sub)", 1);
    waveformSelector_.addItem ("Saw (Bright)", 2);
    waveformSelector_.addItem ("Square (Hollow)", 3);
    waveformSelector_.addItem ("Triangle (Warm)", 4);
    waveformSelector_.setSelectedId (1);
    waveformSelector_.addListener (this);
    addAndMakeVisible (waveformSelector_);

    // ADSR label
    adsrLabel_.setText ("ADSR", juce::dontSendNotification);
    adsrLabel_.setFont (juce::Font (14.0f, juce::Font::bold));
    addAndMakeVisible (adsrLabel_);

    // ADSR sliders
    attackSlider_.setRange (0.001, 0.5, 0.001);
    attackSlider_.setValue (0.005);
    attackSlider_.setSliderStyle (juce::Slider::Rotary);
    attackSlider_.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 50, 20);
    attackSlider_.addListener (this);
    addAndMakeVisible (attackSlider_);

    decaySlider_.setRange (0.001, 1.0, 0.001);
    decaySlider_.setValue (0.1);
    decaySlider_.setSliderStyle (juce::Slider::Rotary);
    decaySlider_.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 50, 20);
    decaySlider_.addListener (this);
    addAndMakeVisible (decaySlider_);

    sustainSlider_.setRange (0.0, 1.0, 0.01);
    sustainSlider_.setValue (0.8);
    sustainSlider_.setSliderStyle (juce::Slider::Rotary);
    sustainSlider_.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 50, 20);
    sustainSlider_.addListener (this);
    addAndMakeVisible (sustainSlider_);

    releaseSlider_.setRange (0.001, 1.0, 0.001);
    releaseSlider_.setValue (0.2);
    releaseSlider_.setSliderStyle (juce::Slider::Rotary);
    releaseSlider_.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 50, 20);
    releaseSlider_.addListener (this);
    addAndMakeVisible (releaseSlider_);

    // Filter slider
    filterLabel_.setText ("Filter Cutoff", juce::dontSendNotification);
    filterLabel_.setFont (juce::Font (12.0f));
    addAndMakeVisible (filterLabel_);

    filterSlider_.setRange (100.0, 2000.0, 1.0);
    filterSlider_.setValue (800.0);
    filterSlider_.setSliderStyle (juce::Slider::Rotary);
    filterSlider_.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    filterSlider_.addListener (this);
    addAndMakeVisible (filterSlider_);

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

    // Generate button
    generateButton_.setButtonText ("Generate Bass Line");
    generateButton_.addListener (this);
    addAndMakeVisible (generateButton_);

    // Export button
    exportButton_.setButtonText ("Export MIDI");
    exportButton_.addListener (this);
    addAndMakeVisible (exportButton_);

    // Stats label
    statsLabel_.setText ("12 notes | C Major | Rock", juce::dontSendNotification);
    statsLabel_.setFont (juce::Font (14.0f));
    addAndMakeVisible (statsLabel_);

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

    // Start timer for animation
    startTimer (100);
}

BassLineEditor::~BassLineEditor()
{
}

void BassLineEditor::paint (juce::Graphics& g)
{
    // Background gradient
    g.fillAll (juce::Colour (0xff1e1e2e));

    juce::ColourGradient gradient (
        juce::Colour (0xff2d2d44), 0, 0,
        juce::Colour (0xff1e1e2e), 0, static_cast<float>(getHeight()), false
    );
    g.setGradientFill (gradient);
    g.fillRect (getLocalBounds());

    // Bass line visualization area (piano roll style, low register)
    g.setColour (juce::Colour (0xff16213e));
    g.fillRoundedRectangle (20, 120, getWidth() - 40, 280, 8.0f);

    // Draw piano roll grid (bass register: C1-C3)
    g.setColour (juce::Colour (0xff0f3460).withAlpha(0.3f));
    for (int i = 0; i < 12; ++i) {
        int y = 120 + (i * 280 / 12);
        g.drawLine (20, static_cast<float>(y), static_cast<float>(getWidth() - 20), static_cast<float>(y), 1.0f);
    }

    // Draw the bass line
    drawBassLine (g, processor_.getCurrentBassLine());
}

void BassLineEditor::resized()
{
    titleLabel_.setBounds (0, 20, getWidth(), 40);

    // Left side: Style and Octave
    styleSelector_.setBounds (20, 70, 140, 30);
    octaveLabel_.setBounds (170, 70, 60, 30);
    octaveSelector_.setBounds (235, 70, 100, 30);

    // Center: Key selection
    keySelector_.setBounds (380, 70, 60, 30);
    scaleSelector_.setBounds (450, 70, 100, 30);

    // Right side: Stats
    statsLabel_.setBounds (getWidth() - 240, 70, 220, 30);

    // Sound controls area (below visualization)
    int soundControlsY = 410;
    waveformLabel_.setBounds (20, soundControlsY, 50, 20);
    waveformSelector_.setBounds (75, soundControlsY, 140, 24);

    // Filter control
    filterLabel_.setBounds (235, soundControlsY - 5, 90, 20);
    filterSlider_.setBounds (235, soundControlsY + 15, 70, 70);

    // ADSR label and knobs
    adsrLabel_.setBounds (330, soundControlsY - 5, 60, 30);
    attackSlider_.setBounds (395, soundControlsY - 30, 70, 70);
    decaySlider_.setBounds (470, soundControlsY - 30, 70, 70);
    sustainSlider_.setBounds (545, soundControlsY - 30, 70, 70);
    releaseSlider_.setBounds (620, soundControlsY - 30, 70, 70);

    // ADSR knob labels
    attackLabel_.setBounds (395, soundControlsY - 50, 70, 20);
    decayLabel_.setBounds (470, soundControlsY - 50, 70, 20);
    sustainLabel_.setBounds (545, soundControlsY - 50, 70, 20);
    releaseLabel_.setBounds (620, soundControlsY - 50, 70, 20);

    // Effects controls (below visualization, left side)
    int effectsY = 480;
    effectsLabel_.setBounds (20, effectsY - 5, 80, 30);
    reverbSlider_.setBounds (110, effectsY - 30, 70, 70);
    delayMixSlider_.setBounds (200, effectsY - 30, 70, 70);

    // Effects knob labels
    reverbLabel_.setBounds (110, effectsY - 50, 70, 20);
    delayLabel_.setBounds (200, effectsY - 50, 70, 20);

    // Generate and Export buttons
    generateButton_.setBounds ((getWidth() - 420) / 2, 500, 200, 35);
    exportButton_.setBounds ((getWidth() - 420) / 2 + 220, 500, 200, 35);
}

void BassLineEditor::timerCallback()
{
    // Repaint for any animations
    repaint();
}

void BassLineEditor::sliderValueChanged (juce::Slider* slider)
{
    if (slider == &attackSlider_)
        processor_.setAttack(static_cast<float>(attackSlider_.getValue()));
    else if (slider == &decaySlider_)
        processor_.setDecay(static_cast<float>(decaySlider_.getValue()));
    else if (slider == &sustainSlider_)
        processor_.setSustain(static_cast<float>(sustainSlider_.getValue()));
    else if (slider == &releaseSlider_)
        processor_.setRelease(static_cast<float>(releaseSlider_.getValue()));
    else if (slider == &filterSlider_)
        processor_.setFilterCutoff(static_cast<float>(filterSlider_.getValue()));
    else if (slider == &reverbSlider_)
        processor_.setReverbMix(static_cast<float>(reverbSlider_.getValue()));
    else if (slider == &delayMixSlider_)
        processor_.setDelayMix(static_cast<float>(delayMixSlider_.getValue()));
}

void BassLineEditor::comboBoxChanged (juce::ComboBox* comboBox)
{
    if (comboBox == &waveformSelector_)
    {
        processor_.setWaveform(waveformSelector_.getSelectedId() - 1);
    }
    else if (comboBox == &octaveSelector_)
    {
        processor_.setOctave(octaveSelector_.getSelectedId());

        // Auto-regenerate
        int styleId = styleSelector_.getSelectedId();
        std::string style = "Rock";
        switch (styleId) {
            case 1: style = "Rock"; break;
            case 2: style = "Funk"; break;
            case 3: style = "Jazz"; break;
            case 4: style = "EDM"; break;
            case 5: style = "Reggae"; break;
            case 6: style = "Latin"; break;
            case 7: style = "Blues"; break;
        }

        processor_.generateNewBassLine(style, 4);

        auto& bassLine = processor_.getCurrentBassLine();
        juce::String keyName = keySelector_.getText();
        juce::String scaleName = scaleSelector_.getText();
        juce::String stats = juce::String(static_cast<int>(bassLine.size())) + " notes | " +
                            keyName + " " + scaleName + " | " + juce::String(style);
        statsLabel_.setText (stats, juce::dontSendNotification);

        repaint();
    }
    else if (comboBox == &keySelector_ || comboBox == &scaleSelector_)
    {
        // Get selected key
        int keyId = keySelector_.getSelectedId() - 1;
        int rootNote = 48 + keyId;  // C2 = 48

        // Get selected scale
        bool isMajor = (scaleSelector_.getSelectedId() == 1);

        // Update processor
        processor_.setKey(rootNote, isMajor);

        // Auto-generate new bass line
        int styleId = styleSelector_.getSelectedId();
        std::string style = "Rock";
        switch (styleId) {
            case 1: style = "Rock"; break;
            case 2: style = "Funk"; break;
            case 3: style = "Jazz"; break;
            case 4: style = "EDM"; break;
            case 5: style = "Reggae"; break;
            case 6: style = "Latin"; break;
            case 7: style = "Blues"; break;
        }

        processor_.generateNewBassLine(style, 4);

        // Update stats
        auto& bassLine = processor_.getCurrentBassLine();
        juce::String keyName = keySelector_.getText();
        juce::String scaleName = scaleSelector_.getText();
        juce::String stats = juce::String(static_cast<int>(bassLine.size())) + " notes | " +
                            keyName + " " + scaleName + " | " + juce::String(style);
        statsLabel_.setText (stats, juce::dontSendNotification);

        repaint();
    }
}

void BassLineEditor::buttonClicked (juce::Button* button)
{
    if (button == &generateButton_)
    {
        int styleId = styleSelector_.getSelectedId();
        std::string style = "Rock";

        switch (styleId) {
            case 1: style = "Rock"; break;
            case 2: style = "Funk"; break;
            case 3: style = "Jazz"; break;
            case 4: style = "EDM"; break;
            case 5: style = "Reggae"; break;
            case 6: style = "Latin"; break;
            case 7: style = "Blues"; break;
        }

        processor_.generateNewBassLine(style, 4);

        // Update stats
        auto& bassLine = processor_.getCurrentBassLine();
        juce::String keyName = keySelector_.getText();
        juce::String scaleName = scaleSelector_.getText();
        juce::String stats = juce::String(static_cast<int>(bassLine.size())) + " notes | " +
                            keyName + " " + scaleName + " | " + juce::String(style);
        statsLabel_.setText (stats, juce::dontSendNotification);

        repaint();
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

void BassLineEditor::drawBassLine (juce::Graphics& g, const std::vector<BassEngine::Note>& bassLine)
{
    if (bassLine.empty())
        return;

    // Find pitch range (bass register: typically C1-C3)
    int minPitch = 24;   // C1
    int maxPitch = 60;   // C4

    float visualWidth = static_cast<float>(getWidth() - 40);
    float visualHeight = 280.0f;
    float beatWidth = visualWidth / 16.0f;  // 16 beats (4 bars)

    // Draw notes
    for (const auto& note : bassLine) {
        float x = 20 + (note.startTime * beatWidth);
        float width = note.duration * beatWidth;

        // Map pitch to Y position (inverted: lower pitch = lower on screen)
        float pitchNormalized = static_cast<float>(note.pitch - minPitch) / static_cast<float>(maxPitch - minPitch);
        pitchNormalized = 1.0f - pitchNormalized;  // Invert
        float y = 120 + (pitchNormalized * visualHeight);
        float height = 20.0f;

        // Color based on whether it's a root note
        juce::Colour noteColor = note.isRoot
            ? juce::Colour (0xffff6b35)  // Orange for root
            : juce::Colour (0xff4ecdc4);  // Teal for other notes

        g.setColour (noteColor);
        g.fillRoundedRectangle (x, y, width * 0.95f, height, 4.0f);

        // Note outline
        g.setColour (noteColor.brighter(0.3f));
        g.drawRoundedRectangle (x, y, width * 0.95f, height, 4.0f, 1.5f);
    }
}
