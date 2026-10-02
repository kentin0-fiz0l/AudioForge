cmake_minimum_required(VERSION 3.15)
project(ChordGenius VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

# Add JUCE
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

# Plugin definition
juce_add_plugin(ChordGenius
    COMPANY_NAME "AI Producer"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Aipr
    PLUGIN_CODE Chgn
    FORMATS VST3 AU
    PRODUCT_NAME "ChordGenius")

# Source files
target_sources(ChordGenius
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/ChordEngine.cpp
        Source/ChordEngine.h)

# JUCE modules
target_compile_definitions(ChordGenius
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(ChordGenius
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
