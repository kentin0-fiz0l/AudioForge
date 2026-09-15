cmake_minimum_required(VERSION 3.15)

project(AIProducer VERSION 1.0.0)

# Add JUCE to the project
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

# Include compiler warnings
include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/CompilerWarnings.cmake)

# Create the plugin
juce_add_plugin(AIProducer
    COMPANY_NAME "AudioForge"
    PLUGIN_MANUFACTURER_CODE AFrg
    PLUGIN_CODE AIPr
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "AI Producer"
    PLUGIN_NAME "AIProducer"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT TRUE    # This plugin generates MIDI!
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE  # For text input
    COPY_PLUGIN_AFTER_BUILD TRUE)

# Add source files
target_sources(AIProducer PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/MIDIGenerator.cpp
    Source/MusicTheory.cpp
    Source/AIClient.cpp)

# Compiler definitions
target_compile_definitions(AIProducer PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=1           # Enable CURL for HTTP requests
    JUCE_DISPLAY_SPLASH_SCREEN=0)

# Link JUCE libraries
target_link_libraries(AIProducer PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_gui_basics)

# Set C++ standard
target_compile_features(AIProducer PRIVATE cxx_std_17)

# Add compiler warnings
add_audioforge_warnings(AIProducer)
