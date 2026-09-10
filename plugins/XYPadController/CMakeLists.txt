cmake_minimum_required(VERSION 3.22)
project(XYPadController VERSION 1.0.0)

add_subdirectory(../../JUCE JUCE)

juce_add_plugin(XYPadController
    COMPANY_NAME "AudioForge"
    PLUGIN_MANUFACTURER_CODE Afge
    PLUGIN_CODE Xypc
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "XY Pad Controller"
    VST3_CATEGORIES Fx
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT FALSE
)

target_sources(XYPadController PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/XYPadComponent.cpp
    ../../midi/MIDILearnManager.cpp
)

target_compile_definitions(XYPadController PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(XYPadController PRIVATE
    juce::juce_audio_utils
    juce::juce_gui_basics
)
