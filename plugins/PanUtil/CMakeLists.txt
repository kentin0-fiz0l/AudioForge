cmake_minimum_required(VERSION 3.15)

project(PanUtil VERSION 1.0.0)

# Add JUCE to the project
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

# Create the plugin
juce_add_plugin(PanUtil
    COMPANY_NAME "AudioForge"
    PLUGIN_MANUFACTURER_CODE AFrg
    PLUGIN_CODE PanU
    FORMATS VST3 Standalone
    PRODUCT_NAME "PanUtil"
    PLUGIN_NAME "PanUtil"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE)

# Add source files
target_sources(PanUtil PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp)

# Compiler definitions
target_compile_definitions(PanUtil PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_DISPLAY_SPLASH_SCREEN=0)

# Link JUCE libraries
target_link_libraries(PanUtil PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp)

# Set C++ standard
target_compile_features(PanUtil PRIVATE cxx_std_17)
