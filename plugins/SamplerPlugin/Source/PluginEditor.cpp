#include "PluginEditor.h"
#include "../../shared/ui/AudioForgeTheme.h"

SamplerPluginEditor::SamplerPluginEditor(SamplerPluginProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{

    // Create components
    waveformDisplay = std::make_unique<WaveformDisplay>();
    addAndMakeVisible(*waveformDisplay);
    waveformDisplay->addListener(this);

    zoneEditor = std::make_unique<ZoneEditor>(processor.getKeyZoneMap());
    addAndMakeVisible(*zoneEditor);
    zoneEditor->addListener(this);

    velocityLayerEditor = std::make_unique<VelocityLayerEditor>();
    addAndMakeVisible(*velocityLayerEditor);
    velocityLayerEditor->addListener(this);

    sampleBrowser = std::make_unique<SampleBrowser>(processor.getSampleLibrary());
    addAndMakeVisible(*sampleBrowser);

    // Buttons
    loadSampleButton.setButtonText("Load Sample...");
    loadSampleButton.onClick = [this]()
    {
        auto chooser = std::make_shared<juce::FileChooser>("Select audio file",
                                                           juce::File(),
                                                           "*.wav;*.aiff;*.flac");

        chooser->launchAsync(juce::FileBrowserComponent::openMode |
                           juce::FileBrowserComponent::canSelectFiles,
                           [this, chooser](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file.existsAsFile())
            {
                loadSampleFile(file);
            }
        });
    };
    addAndMakeVisible(loadSampleButton);

    addZoneButton.setButtonText("Add Zone");
    addZoneButton.onClick = [this]()
    {
        // Add zone at middle C
        KeyZone newZone(60, 60, 60);
        processor.getKeyZoneMap().addZone(newZone);
        zoneEditor->repaint();
        updateZoneInfo();
    };
    addAndMakeVisible(addZoneButton);

    deleteZoneButton.setButtonText("Delete Zone");
    deleteZoneButton.onClick = [this]()
    {
        int selectedZone = zoneEditor->getSelectedZoneIndex();
        if (selectedZone >= 0)
        {
            processor.getKeyZoneMap().removeZone(selectedZone);
            zoneEditor->setSelectedZone(-1);
            zoneEditor->repaint();
            updateZoneInfo();
            velocityLayerEditor->clearZone();
        }
    };
    addAndMakeVisible(deleteZoneButton);

    addLayerButton.setButtonText("Add Layer");
    addLayerButton.onClick = [this]()
    {
        int selectedSample = sampleBrowser->getSelectedSampleIndex();
        if (selectedSample >= 0)
        {
            velocityLayerEditor->addLayer(selectedSample);
        }
    };
    addAndMakeVisible(addLayerButton);

    deleteLayerButton.setButtonText("Delete Layer");
    deleteLayerButton.onClick = [this]()
    {
        // Implementation for deleting selected velocity layer
    };
    addAndMakeVisible(deleteLayerButton);

    // Loop mode selector
    loopModeSelector.addItem("None", 1);
    loopModeSelector.addItem("Forward", 2);
    loopModeSelector.addItem("Ping-Pong", 3);
    loopModeSelector.setSelectedId(1, juce::dontSendNotification);
    loopModeSelector.onChange = [this]()
    {
        auto sample = waveformDisplay->getSample();
        if (sample)
        {
            int mode = loopModeSelector.getSelectedId() - 1;
            sample->setLoopMode(static_cast<Sample::LoopMode>(mode));
        }
    };
    addAndMakeVisible(loopModeSelector);
    loopModeLabel.setText("Loop Mode", juce::dontSendNotification);
    loopModeLabel.setJustificationType(juce::Justification::centred);
    loopModeLabel.attachToComponent(&loopModeSelector, false);
    addAndMakeVisible(loopModeLabel);

    // Preset selector
    populatePresetSelector();
    presetSelector.onChange = [this]()
    {
        int selectedIndex = presetSelector.getSelectedItemIndex();
        if (selectedIndex >= 0)
        {
            loadPreset(selectedIndex);
        }
    };
    addAndMakeVisible(presetSelector);
    presetLabel.setText("Preset", juce::dontSendNotification);
    presetLabel.setJustificationType(juce::Justification::centred);
    presetLabel.attachToComponent(&presetSelector, false);
    addAndMakeVisible(presetLabel);

    savePresetButton.setButtonText("Save Preset");
    savePresetButton.onClick = [this]()
    {
        // Future: implement user preset saving
    };
    addAndMakeVisible(savePresetButton);

    // Labels
    memoryLabel.setText("Memory: 0 / 512 MB", juce::dontSendNotification);
    memoryLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(memoryLabel);

    zoneInfoLabel.setText("Zones: 0", juce::dontSendNotification);
    zoneInfoLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(zoneInfoLabel);

    // ADSR Sliders
    ampAttackSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    ampAttackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(ampAttackSlider);
    ampAttackLabel.setText("Attack", juce::dontSendNotification);
    ampAttackLabel.setJustificationType(juce::Justification::centred);
    ampAttackLabel.attachToComponent(&ampAttackSlider, false);
    addAndMakeVisible(ampAttackLabel);

    ampDecaySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    ampDecaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(ampDecaySlider);
    ampDecayLabel.setText("Decay", juce::dontSendNotification);
    ampDecayLabel.setJustificationType(juce::Justification::centred);
    ampDecayLabel.attachToComponent(&ampDecaySlider, false);
    addAndMakeVisible(ampDecayLabel);

    ampSustainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    ampSustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(ampSustainSlider);
    ampSustainLabel.setText("Sustain", juce::dontSendNotification);
    ampSustainLabel.setJustificationType(juce::Justification::centred);
    ampSustainLabel.attachToComponent(&ampSustainSlider, false);
    addAndMakeVisible(ampSustainLabel);

    ampReleaseSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    ampReleaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(ampReleaseSlider);
    ampReleaseLabel.setText("Release", juce::dontSendNotification);
    ampReleaseLabel.setJustificationType(juce::Justification::centred);
    ampReleaseLabel.attachToComponent(&ampReleaseSlider, false);
    addAndMakeVisible(ampReleaseLabel);

    // Filter Sliders
    filterCutoffSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    filterCutoffSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(filterCutoffSlider);
    filterCutoffLabel.setText("Cutoff", juce::dontSendNotification);
    filterCutoffLabel.setJustificationType(juce::Justification::centred);
    filterCutoffLabel.attachToComponent(&filterCutoffSlider, false);
    addAndMakeVisible(filterCutoffLabel);

    filterResonanceSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    filterResonanceSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(filterResonanceSlider);
    filterResonanceLabel.setText("Resonance", juce::dontSendNotification);
    filterResonanceLabel.setJustificationType(juce::Justification::centred);
    filterResonanceLabel.attachToComponent(&filterResonanceSlider, false);
    addAndMakeVisible(filterResonanceLabel);

    // Attachments
    auto& apvts = processor.getAPVTS();
    ampAttackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, SamplerPluginProcessor::PARAM_AMP_ATTACK, ampAttackSlider);
    ampDecayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, SamplerPluginProcessor::PARAM_AMP_DECAY, ampDecaySlider);
    ampSustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, SamplerPluginProcessor::PARAM_AMP_SUSTAIN, ampSustainSlider);
    ampReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, SamplerPluginProcessor::PARAM_AMP_RELEASE, ampReleaseSlider);
    filterCutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, SamplerPluginProcessor::PARAM_FILTER_CUTOFF, filterCutoffSlider);
    filterResonanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, SamplerPluginProcessor::PARAM_FILTER_RESONANCE, filterResonanceSlider);

    // Set size
    setSize(950, 650);

    // Initial updates
    updateMemoryDisplay();
    updateZoneInfo();

    // Timer for periodic updates
    // Temporarily disabled for debugging
    // startTimerHz(10);
}

SamplerPluginEditor::~SamplerPluginEditor()
{
    waveformDisplay->removeListener(this);
    zoneEditor->removeListener(this);
    velocityLayerEditor->removeListener(this);

    // Explicitly destroy components in controlled order
    waveformDisplay.reset();
    zoneEditor.reset();
    velocityLayerEditor.reset();
    sampleBrowser.reset();

    // Reset parameter attachments
    ampAttackAttachment.reset();
    ampDecayAttachment.reset();
    ampSustainAttachment.reset();
    ampReleaseAttachment.reset();
    filterCutoffAttachment.reset();
    filterResonanceAttachment.reset();

}

void SamplerPluginEditor::paint(juce::Graphics& g)
{
    using namespace AudioForge;

    // Background
    g.fillAll(Colors::Background);

    // Title bar (only if not dragging)
    if (!isDraggingFile)
    {
        Layout::drawTitleBar(g, "AudioForge Sampler", Categories::Synthesizer, getWidth());
    }

    // Drag and drop overlay
    if (isDraggingFile)
    {
        g.setColour(Colors::Primary.withAlpha(0.3f));
        g.fillAll();
        g.setColour(Colors::Text);
        g.setFont(Typography::Title);
        g.drawText("Drop audio file to load",
                  getLocalBounds(), juce::Justification::centred, true);
    }
}

void SamplerPluginEditor::resized()
{
    auto bounds = getLocalBounds();

    // Top section: Waveform display (200px)
    waveformDisplay->setBounds(bounds.removeFromTop(200));

    // Zone editor (120px)
    zoneEditor->setBounds(bounds.removeFromTop(120));

    // Control buttons (40px)
    auto buttonArea = bounds.removeFromTop(40).reduced(10, 5);
    loadSampleButton.setBounds(buttonArea.removeFromLeft(120));
    buttonArea.removeFromLeft(10);
    addZoneButton.setBounds(buttonArea.removeFromLeft(100));
    buttonArea.removeFromLeft(10);
    deleteZoneButton.setBounds(buttonArea.removeFromLeft(100));

    buttonArea.removeFromLeft(20);
    zoneInfoLabel.setBounds(buttonArea.removeFromLeft(150));

    memoryLabel.setBounds(buttonArea.removeFromRight(150));

    // Bottom area split: parameters (left) and layer editor + sample browser (right)
    auto bottomArea = bounds.reduced(10, 10);

    // Right side: Velocity layer editor + sample browser (300px wide)
    auto rightPanel = bottomArea.removeFromRight(300);

    // Velocity layer editor (top half)
    auto velLayerArea = rightPanel.removeFromTop(rightPanel.getHeight() / 2);
    addLayerButton.setBounds(velLayerArea.removeFromTop(30).reduced(5, 2));
    deleteLayerButton.setBounds(velLayerArea.removeFromTop(30).reduced(5, 2));
    velocityLayerEditor->setBounds(velLayerArea);

    // Sample browser (bottom half)
    sampleBrowser->setBounds(rightPanel);

    // Left side: Parameters
    bottomArea.removeFromLeft(10);  // Spacing
    auto paramArea = bottomArea;

    // Preset selector row (top)
    auto presetRow = paramArea.removeFromTop(50);
    presetRow.removeFromTop(20);  // Label space
    presetSelector.setBounds(presetRow.removeFromLeft(200));
    presetRow.removeFromLeft(10);
    savePresetButton.setBounds(presetRow.removeFromLeft(100));

    // Loop mode row
    auto loopRow = paramArea.removeFromTop(50);
    loopRow.removeFromTop(20);  // Label space
    loopModeSelector.setBounds(loopRow.removeFromLeft(150));

    paramArea.removeFromTop(10);  // Spacing

    // ADSR section
    auto adsrRow = paramArea.removeFromTop(120);
    ampAttackSlider.setBounds(adsrRow.removeFromLeft(90));
    adsrRow.removeFromLeft(10);
    ampDecaySlider.setBounds(adsrRow.removeFromLeft(90));
    adsrRow.removeFromLeft(10);
    ampSustainSlider.setBounds(adsrRow.removeFromLeft(90));
    adsrRow.removeFromLeft(10);
    ampReleaseSlider.setBounds(adsrRow.removeFromLeft(90));

    paramArea.removeFromTop(10);  // Spacing

    // Filter section
    auto filterRow = paramArea.removeFromTop(120);
    filterCutoffSlider.setBounds(filterRow.removeFromLeft(90));
    filterRow.removeFromLeft(10);
    filterResonanceSlider.setBounds(filterRow.removeFromLeft(90));
}

bool SamplerPluginEditor::isInterestedInFileDrag(const juce::StringArray& files)
{
    for (const auto& file : files)
    {
        if (file.endsWithIgnoreCase(".wav") ||
            file.endsWithIgnoreCase(".aiff") ||
            file.endsWithIgnoreCase(".flac"))
        {
            return true;
        }
    }
    return false;
}

void SamplerPluginEditor::filesDropped(const juce::StringArray& files, int x, int y)
{
    isDraggingFile = false;
    repaint();

    if (!files.isEmpty())
    {
        loadSampleFile(juce::File(files[0]));
    }
}

void SamplerPluginEditor::fileDragEnter(const juce::StringArray& files, int x, int y)
{
    isDraggingFile = true;
    repaint();
}

void SamplerPluginEditor::fileDragExit(const juce::StringArray& files)
{
    isDraggingFile = false;
    repaint();
}

void SamplerPluginEditor::zoneSelectionChanged(int zoneIndex)
{
    updateZoneInfo();
    updateVelocityLayerEditor();
}

void SamplerPluginEditor::zoneRangeChanged(int zoneIndex, int newMin, int newMax)
{
    updateZoneInfo();
}

void SamplerPluginEditor::zoneAdded(int zoneIndex)
{
    updateZoneInfo();
}

void SamplerPluginEditor::zoneDeleted(int zoneIndex)
{
    updateZoneInfo();
}

void SamplerPluginEditor::loopPointsChanged(double loopStartSec, double loopEndSec)
{
    // Update sample loop points
    auto sample = waveformDisplay->getSample();
    if (sample)
    {
        sample->setLoopPoints(loopStartSec, loopEndSec);
    }
}

void SamplerPluginEditor::loadSampleFile(const juce::File& file)
{
    // Load sample into library
    auto sample = processor.getSampleLibrary().loadSample(file);

    if (sample)
    {
        // Display in waveform
        waveformDisplay->setSample(sample);

        // Auto-create zone if none exist
        if (processor.getKeyZoneMap().getNumZones() == 0)
        {
            KeyZone newZone(60, 72, 60);  // C4-C5, root at C4
            VelocityLayer layer(0, 0.0f, 1.0f);  // First sample, full velocity
            newZone.velocityLayers.push_back(layer);
            processor.getKeyZoneMap().addZone(newZone);
            zoneEditor->repaint();
        }

        updateMemoryDisplay();
        updateZoneInfo();
    }
    else
    {
        // Show error
        juce::String error = processor.getSampleLibrary().getLastError();
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon,
            "Failed to load sample",
            error,
            "OK");
    }
}

void SamplerPluginEditor::updateMemoryDisplay()
{
    auto memUsed = processor.getSampleLibrary().getTotalMemoryUsed() / (1024.0 * 1024.0);
    auto maxMem = SampleLibrary::MAX_MEMORY_BYTES / (1024.0 * 1024.0);

    juce::String text = juce::String::formatted("Memory: %.1f / %.0f MB", memUsed, maxMem);
    memoryLabel.setText(text, juce::dontSendNotification);
}

void SamplerPluginEditor::updateZoneInfo()
{
    int numZones = processor.getKeyZoneMap().getNumZones();
    int selectedZone = zoneEditor->getSelectedZoneIndex();

    juce::String text;
    if (selectedZone >= 0)
    {
        const auto& zone = processor.getKeyZoneMap().getZones()[selectedZone];
        juce::String noteRange = juce::MidiMessage::getMidiNoteName(zone.noteMin, true, true, 3) +
                                 " - " +
                                 juce::MidiMessage::getMidiNoteName(zone.noteMax, true, true, 3);
        text = juce::String::formatted("Zone %d/%d: %s",
                                       selectedZone + 1, numZones, noteRange.toRawUTF8());
    }
    else
    {
        text = juce::String::formatted("Zones: %d", numZones);
    }

    zoneInfoLabel.setText(text, juce::dontSendNotification);
}

void SamplerPluginEditor::timerCallback()
{
    updateMemoryDisplay();
}

void SamplerPluginEditor::velocityLayerChanged(int layerIndex)
{
    // Velocity layer was modified, update UI if needed
    updateVelocityLayerEditor();
}

void SamplerPluginEditor::updateVelocityLayerEditor()
{
    int selectedZone = zoneEditor->getSelectedZoneIndex();
    if (selectedZone >= 0)
    {
        auto& zone = processor.getKeyZoneMap().getZones()[selectedZone];
        velocityLayerEditor->setZone(&zone, &processor.getSampleLibrary());
    }
    else
    {
        velocityLayerEditor->clearZone();
    }
}

void SamplerPluginEditor::loadPreset(int presetIndex)
{
    auto presets = FactoryPresets::getAllPresets();
    if (presetIndex < 0 || presetIndex >= static_cast<int>(presets.size()))
        return;

    const auto& preset = presets[presetIndex];
    auto& apvts = processor.getAPVTS();

    // Apply parameter values (normalized 0-1)
    apvts.getParameter(SamplerPluginProcessor::PARAM_AMP_ATTACK)->setValueNotifyingHost(preset.params.ampAttack);
    apvts.getParameter(SamplerPluginProcessor::PARAM_AMP_DECAY)->setValueNotifyingHost(preset.params.ampDecay);
    apvts.getParameter(SamplerPluginProcessor::PARAM_AMP_SUSTAIN)->setValueNotifyingHost(preset.params.ampSustain);
    apvts.getParameter(SamplerPluginProcessor::PARAM_AMP_RELEASE)->setValueNotifyingHost(preset.params.ampRelease);

    apvts.getParameter(SamplerPluginProcessor::PARAM_FILTER_CUTOFF)->setValueNotifyingHost(preset.params.filterCutoff);
    apvts.getParameter(SamplerPluginProcessor::PARAM_FILTER_RESONANCE)->setValueNotifyingHost(preset.params.filterResonance);

    // Update loop mode selector
    loopModeSelector.setSelectedId(preset.params.loopMode + 1, juce::dontSendNotification);
}

void SamplerPluginEditor::populatePresetSelector()
{
    auto presets = FactoryPresets::getAllPresets();

    presetSelector.clear();
    for (int i = 0; i < static_cast<int>(presets.size()); ++i)
    {
        presetSelector.addItem(presets[i].name, i + 1);
    }
}
