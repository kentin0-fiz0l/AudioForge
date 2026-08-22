cmake_minimum_required(VERSION 3.15)

project(ChorusFlanger VERSION 1.0.0)

# Add JUCE to the project
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

# Create the plugin target
juce_add_plugin(ChorusFlanger
    COMPANY_NAME "AudioForge"
    PLUGIN_MANUFACTURER_CODE Afrg
    PLUGIN_CODE Chfl
    FORMATS VST3 Standalone
    PRODUCT_NAME "ChorusFlanger"
    DESCRIPTION "Chorus and Flanger modulation effects"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_CATEGORIES "Fx" "Modulation")

# Add source files
target_sources(ChorusFlanger PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp)

# Link JUCE libraries
target_link_libraries(ChorusFlanger PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp)

# Add include directories for shared DSP library
target_include_directories(ChorusFlanger PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../shared)

# Compiler definitions
target_compile_definitions(ChorusFlanger PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_DISPLAY_SPLASH_SCREEN=0)

# Set C++ standard
target_compile_features(ChorusFlanger PRIVATE cxx_std_17)
