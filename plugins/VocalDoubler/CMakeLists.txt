cmake_minimum_required(VERSION 3.15)

project(VocalDoubler VERSION 1.0.0)

# Add JUCE to the project
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

# Add AudioForge shared libraries
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../shared ${CMAKE_CURRENT_BINARY_DIR}/shared)

# Include compiler warnings
include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/CompilerWarnings.cmake)

# Create the plugin
juce_add_plugin(VocalDoubler
    COMPANY_NAME "AudioForge"
    PLUGIN_MANUFACTURER_CODE AFrg
    PLUGIN_CODE VDbl
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "Vocal Doubler"
    PLUGIN_NAME "VocalDoubler"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE)

# Add source files
target_sources(VocalDoubler PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/DoublerEngine.cpp)

# Compiler definitions
target_compile_definitions(VocalDoubler PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_DISPLAY_SPLASH_SCREEN=0)

# Link JUCE libraries and shared libraries
target_link_libraries(VocalDoubler PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    AudioForgeDSP
    AudioForgePresets
    juce::juce_audio_processors)

# Set C++ standard
target_compile_features(VocalDoubler PRIVATE cxx_std_17)

# Add compiler warnings
add_audioforge_warnings(VocalDoubler)
