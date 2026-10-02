cmake_minimum_required(VERSION 3.15)
project(DrumMachine VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add JUCE
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

# Plugin definition
juce_add_plugin(DrumMachine
    COMPANY_NAME "AudioForge"
    BUNDLE_ID "com.audioforge.DrumMachine"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Aufo
    PLUGIN_CODE Drum
    FORMATS VST3 AU
    PRODUCT_NAME "DrumMachine")

# Source files
target_sources(DrumMachine
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/DrumEngine.cpp
        Source/DrumEngine.h
        Source/DrumSynth.cpp
        Source/DrumSynth.h)

# JUCE modules
target_compile_definitions(DrumMachine
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(DrumMachine
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
