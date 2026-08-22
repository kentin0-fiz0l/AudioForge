cmake_minimum_required(VERSION 3.15)

project(SimpleEQ VERSION 0.1.0)

# Add JUCE to the project
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

# Create plugin target
juce_add_plugin(SimpleEQ
    COMPANY_NAME "AudioForge"
    PLUGIN_MANUFACTURER_CODE AFge
    PLUGIN_CODE SmEQ
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "SimpleEQ"

    # Plugin characteristics
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE

    # Copy to system plugin folder after build
    COPY_PLUGIN_AFTER_BUILD TRUE)

# Add source files
target_sources(SimpleEQ PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h)

# Link JUCE libraries
target_link_libraries(SimpleEQ PRIVATE
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_dsp)

# Add include directories for shared DSP library
target_include_directories(SimpleEQ PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../shared)

# Compiler definitions
target_compile_definitions(SimpleEQ PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JucePlugin_Name="SimpleEQ"
    JucePlugin_Desc="3-band parametric EQ with HPF/LPF"
    JucePlugin_Manufacturer="AudioForge"
    JucePlugin_ManufacturerWebsite="https://audioforge.fluxstudio.art"
    JucePlugin_ManufacturerEmail="support@audioforge.fluxstudio.art"
    JucePlugin_VersionString="${SimpleEQ_VERSION}")

# Set C++ standard
target_compile_features(SimpleEQ PRIVATE cxx_std_17)
