cmake_minimum_required(VERSION 3.22)
project(MacroController VERSION 1.0.0)

add_subdirectory(../../JUCE JUCE)

juce_add_plugin(MacroController
    COMPANY_NAME "AudioForge"
    PLUGIN_MANUFACTURER_CODE Afge
    PLUGIN_CODE Macr
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "Macro Controller"
    VST3_CATEGORIES Fx
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT FALSE
)

target_sources(MacroController PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/MacroEngine.cpp
    ../../midi/MIDILearnManager.cpp
)

target_compile_definitions(MacroController PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(MacroController PRIVATE
    juce::juce_audio_utils
    juce::juce_gui_basics
)
