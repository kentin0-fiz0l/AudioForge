cmake_minimum_required(VERSION 3.22)
project(MIDIChordGenerator VERSION 1.0.0)

add_subdirectory(../../JUCE JUCE)

juce_add_plugin(MIDIChordGenerator
    COMPANY_NAME "AudioForge"
    PLUGIN_MANUFACTURER_CODE Afge
    PLUGIN_CODE Mchd
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "MIDI Chord Generator"
    VST3_CATEGORIES Fx
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT TRUE
)

target_sources(MIDIChordGenerator PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/ChordEngine.cpp
    ../../midi/MIDILearnManager.cpp
)

target_compile_definitions(MIDIChordGenerator PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(MIDIChordGenerator PRIVATE
    juce::juce_audio_utils
)
