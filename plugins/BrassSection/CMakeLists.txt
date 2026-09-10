cmake_minimum_required(VERSION 3.15)

project(BrassSection VERSION 1.0.0)

# Add JUCE to the project
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

# Add AudioForge shared libraries
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../shared ${CMAKE_CURRENT_BINARY_DIR}/shared)

# Create the plugin
juce_add_plugin(BrassSection
    COMPANY_NAME "AudioForge"
    PLUGIN_MANUFACTURER_CODE AFrg
    PLUGIN_CODE ABrs
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "BrassSection"
    PLUGIN_NAME "AudioForge - BrassSection"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_CATEGORIES "Instrument" "Synth"
    AU_MAIN_TYPE "kAudioUnitType_MusicDevice")

# Add source files
target_sources(BrassSection PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/BrassVoice.cpp
    Source/BrassEngine.cpp
    ../../midi/MIDILearnManager.cpp)

# Compiler definitions
target_compile_definitions(BrassSection PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_MODAL_LOOPS_PERMITTED=1)

# Link JUCE libraries and shared DSP library
target_link_libraries(BrassSection PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    AudioForgeDSP
    AudioForgeSynth
    AudioForgePresets)

# Set C++ standard
target_compile_features(BrassSection PRIVATE cxx_std_17)
