cmake_minimum_required(VERSION 3.22)
project(MIDIArpeggiator VERSION 1.0.0)

add_subdirectory(../../JUCE JUCE)

juce_add_plugin(MIDIArpeggiator
    COMPANY_NAME "AudioForge"
    PLUGIN_MANUFACTURER_CODE Afge
    PLUGIN_CODE Marp
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "MIDI Arpeggiator"
    VST3_CATEGORIES Fx
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT TRUE
)

target_sources(MIDIArpeggiator PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/ArpEngine.cpp
    ../../midi/MIDILearnManager.cpp
)

target_compile_definitions(MIDIArpeggiator PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(MIDIArpeggiator PRIVATE
    juce::juce_audio_utils
)
