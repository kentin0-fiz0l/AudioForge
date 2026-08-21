cmake_minimum_required(VERSION 3.15)

project(SimpleGain VERSION 1.0.0)

# Add JUCE to the project
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

# Add AudioForge shared DSP library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../shared ${CMAKE_CURRENT_BINARY_DIR}/shared)

# Create the plugin
juce_add_plugin(SimpleGain
    COMPANY_NAME "AudioForge"
    PLUGIN_MANUFACTURER_CODE AFrg
    PLUGIN_CODE SmpG
    FORMATS VST3 Standalone
    PRODUCT_NAME "SimpleGain"
    PLUGIN_NAME "SimpleGain"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE)

# Add source files
target_sources(SimpleGain PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp)

# Compiler definitions
target_compile_definitions(SimpleGain PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_DISPLAY_SPLASH_SCREEN=0)

# Link JUCE libraries and shared DSP library
target_link_libraries(SimpleGain PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    AudioForgeDSP)

# Set C++ standard
target_compile_features(SimpleGain PRIVATE cxx_std_17)
