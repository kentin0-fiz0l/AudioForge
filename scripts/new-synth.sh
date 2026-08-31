#!/bin/bash

# new-synth.sh - Generate a new synthesizer plugin template
# Usage: ./scripts/new-synth.sh PluginName "Plugin Code"
#
# Example: ./scripts/new-synth.sh FMSynth "FMSy"

set -e

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 PluginName PluginCode"
    echo "Example: $0 FMSynth FMSy"
    exit 1
fi

PLUGIN_NAME="$1"
PLUGIN_CODE="$2"

if [ ${#PLUGIN_CODE} -ne 4 ]; then
    echo "Error: Plugin code must be exactly 4 characters"
    echo "Example: FMSy for FMSynth"
    exit 1
fi

PLUGIN_DIR="plugins/$PLUGIN_NAME"

if [ -d "$PLUGIN_DIR" ]; then
    echo "Error: Plugin directory already exists: $PLUGIN_DIR"
    exit 1
fi

echo "Creating new synth plugin: $PLUGIN_NAME ($PLUGIN_CODE)"
echo "-----------------------------------------------"

# Create directory structure
mkdir -p "$PLUGIN_DIR/Source"

# Create CMakeLists.txt
cat > "$PLUGIN_DIR/CMakeLists.txt" << EOF
# $PLUGIN_NAME Plugin
# Part of the AudioForge instrument pack

juce_add_plugin($PLUGIN_NAME
    COMPANY_NAME "AudioForge"
    PLUGIN_MANUFACTURER_CODE AFrg
    PLUGIN_CODE $PLUGIN_CODE
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_NAME "$PLUGIN_NAME"
    PRODUCT_NAME "$PLUGIN_NAME"
    FORMATS VST3 AU Standalone
)

# Source files
target_sources($PLUGIN_NAME PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)

# Include directories
target_include_directories($PLUGIN_NAME PRIVATE
    Source
)

# Link libraries
target_link_libraries($PLUGIN_NAME PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp
    AudioForgeDSP
    AudioForgeSynth
    AudioForgePresets
)

# Compiler definitions
target_compile_definitions($PLUGIN_NAME PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)

# Set C++17 standard
target_compile_features($PLUGIN_NAME PRIVATE cxx_std_17)
EOF

# Create PluginProcessor.h
cat > "$PLUGIN_DIR/Source/PluginProcessor.h" << 'EOF'
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <synth/VoiceManager.h>
#include <presets/PresetManager.h>

/**
 * Voice class for this synthesizer.
 * Implements the per-voice DSP and state.
 */
class Voice
{
public:
    Voice();

    void noteOn(int midiNote, float velocity, double sampleRate);
    void noteOff();
    bool isActive() const { return active; }
    int getMidiNote() const { return currentNote; }
    float getLevel() const { return level; }
    void reset();

    float processSample(double sampleRate);

private:
    bool active = false;
    int currentNote = -1;
    float velocity = 0.0f;
    float level = 0.0f;

    // TODO: Add synthesis components (oscillators, envelopes, filters, etc.)
};

/**
 * Plugin Processor
 */
class PluginProcessor : public juce::AudioProcessor
{
public:
    PluginProcessor();
    ~PluginProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Parameters
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

private:
    juce::AudioProcessorValueTreeState apvts;
    AudioForge::VoiceManager<Voice, 8> voiceManager;
    AudioForge::PresetManager presetManager;

    void createParameters();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
};
EOF

# Create PluginProcessor.cpp
cat > "$PLUGIN_DIR/Source/PluginProcessor.cpp" << 'EOF'
#include "PluginProcessor.h"
#include "PluginEditor.h"

// ============================================================================
// Voice Implementation
// ============================================================================

Voice::Voice()
{
}

void Voice::noteOn(int midiNote, float velocity_, double sampleRate)
{
    currentNote = midiNote;
    velocity = velocity_;
    active = true;
    level = velocity;

    // TODO: Trigger envelopes, set oscillator frequency, etc.
}

void Voice::noteOff()
{
    // TODO: Trigger envelope release
    active = false; // For now, just stop immediately
}

void Voice::reset()
{
    active = false;
    currentNote = -1;
    velocity = 0.0f;
    level = 0.0f;
}

float Voice::processSample(double sampleRate)
{
    if (!active)
        return 0.0f;

    // TODO: Generate synthesis output
    // Example: return oscillator.processSample() * envelope.getNextValue();
    return 0.0f;
}

// ============================================================================
// Processor Implementation
// ============================================================================

PluginProcessor::PluginProcessor()
    : AudioProcessor(BusesProperties()
                    .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , apvts(*this, nullptr, "Parameters", {})
    , presetManager("PLUGIN_NAME_PLACEHOLDER", *this)
{
    createParameters();

    // TODO: Load factory presets
    // presetManager.setFactoryPresets({ ... });
    presetManager.scanPresets();
}

PluginProcessor::~PluginProcessor()
{
}

void PluginProcessor::createParameters()
{
    // TODO: Define parameters
    // Example:
    // apvts.createAndAddParameter("cutoff", "Cutoff", ..., defaultValue);
}

void PluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Prepare voices for playback
}

void PluginProcessor::releaseResources()
{
}

bool PluginProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // Mono or stereo output
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

void PluginProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // Clear output buffer
    buffer.clear();

    // Process MIDI events
    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();

        if (message.isNoteOn())
        {
            voiceManager.noteOn(message.getNoteNumber(),
                              message.getFloatVelocity(),
                              getSampleRate());
        }
        else if (message.isNoteOff())
        {
            voiceManager.noteOff(message.getNoteNumber());
        }
        else if (message.isAllNotesOff() || message.isAllSoundOff())
        {
            voiceManager.allNotesOff();
        }
    }

    // Process voices
    auto* channelDataL = buffer.getWritePointer(0);
    auto* channelDataR = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float outputSample = 0.0f;

        // Sum all voices
        for (auto& voice : voiceManager.getVoices())
        {
            outputSample += voice.processSample(getSampleRate());
        }

        // Write to output (mono -> stereo if needed)
        channelDataL[sample] = outputSample;
        if (channelDataR)
            channelDataR[sample] = outputSample;
    }
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor(*this);
}

bool PluginProcessor::hasEditor() const
{
    return true;
}

const juce::String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
    return true;
}

bool PluginProcessor::producesMidi() const
{
    return false;
}

bool PluginProcessor::isMidiEffect() const
{
    return false;
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 1;
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

void PluginProcessor::setCurrentProgram(int index)
{
}

const juce::String PluginProcessor::getProgramName(int index)
{
    return {};
}

void PluginProcessor::changeProgramName(int index, const juce::String& newName)
{
}

void PluginProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PluginProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}
EOF

# Create PluginEditor.h
cat > "$PLUGIN_DIR/Source/PluginEditor.h" << 'EOF'
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

/**
 * Plugin Editor (UI)
 */
class PluginEditor : public juce::AudioProcessorEditor
{
public:
    PluginEditor(PluginProcessor&);
    ~PluginEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    PluginProcessor& audioProcessor;

    // TODO: Add UI components (sliders, labels, preset browser, etc.)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};
EOF

# Create PluginEditor.cpp
cat > "$PLUGIN_DIR/Source/PluginEditor.cpp" << 'EOF'
#include "PluginProcessor.h"
#include "PluginEditor.h"

PluginEditor::PluginEditor(PluginProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(600, 400);
}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawFittedText("PLUGIN_NAME_PLACEHOLDER", getLocalBounds(), juce::Justification::centred, 1);

    // TODO: Draw UI
}

void PluginEditor::resized()
{
    // TODO: Layout UI components
}
EOF

# Replace PLUGIN_NAME_PLACEHOLDER with actual plugin name
sed -i '' "s/PLUGIN_NAME_PLACEHOLDER/$PLUGIN_NAME/g" "$PLUGIN_DIR/Source/PluginProcessor.cpp"
sed -i '' "s/PLUGIN_NAME_PLACEHOLDER/$PLUGIN_NAME/g" "$PLUGIN_DIR/Source/PluginEditor.cpp"

echo "✓ Created plugin structure at: $PLUGIN_DIR"
echo "✓ CMakeLists.txt configured with plugin code: $PLUGIN_CODE"
echo ""
echo "Next steps:"
echo "1. Add 'add_subdirectory(plugins/$PLUGIN_NAME)' to root CMakeLists.txt"
echo "2. Implement Voice class synthesis DSP"
echo "3. Add UI components in PluginEditor"
echo "4. Create factory presets"
echo "5. Add tests in tests/Plugins/${PLUGIN_NAME}Tests.cpp"
echo ""
echo "Build with:"
echo "  cmake -B build && cmake --build build --target $PLUGIN_NAME"
