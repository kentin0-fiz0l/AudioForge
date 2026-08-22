cmake_minimum_required(VERSION 3.15)

project(BasicSynth VERSION 0.1.0)

# Add JUCE to the project
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

# Add AudioForge shared DSP library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../shared ${CMAKE_CURRENT_BINARY_DIR}/shared)

# Create the plugin
juce_add_plugin(BasicSynth
    COMPANY_NAME "AudioForge"
    PLUGIN_MANUFACTURER_CODE AFrg
    PLUGIN_CODE BaSy
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "BasicSynth"
    PLUGIN_NAME "BasicSynth"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE)

# Add source files
target_sources(BasicSynth PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/Voice.cpp
    Source/Oscillator.cpp)

# Compiler definitions
target_compile_definitions(BasicSynth PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_DISPLAY_SPLASH_SCREEN=0)

# Link JUCE libraries and shared DSP library
target_link_libraries(BasicSynth PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    AudioForgeDSP)

# Set C++ standard
target_compile_features(BasicSynth PRIVATE cxx_std_17)
