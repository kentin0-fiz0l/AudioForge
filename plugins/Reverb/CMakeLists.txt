cmake_minimum_required(VERSION 3.15)

project(Reverb VERSION 1.0.0)

# Add JUCE to the project
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

# Create the plugin target
juce_add_plugin(Reverb
    COMPANY_NAME "AudioForge"
    PLUGIN_MANUFACTURER_CODE Afrg
    PLUGIN_CODE Revb
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "Reverb"
    DESCRIPTION "Algorithmic reverb with Freeverb-inspired design"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_CATEGORIES "Fx" "Reverb")

# Add source files
target_sources(Reverb PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp)

# Link JUCE libraries
target_link_libraries(Reverb PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp)

# Add include directories for shared DSP library
target_include_directories(Reverb PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../shared)

# Compiler definitions
target_compile_definitions(Reverb PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_DISPLAY_SPLASH_SCREEN=0)

# Set C++ standard
target_compile_features(Reverb PRIVATE cxx_std_17)
