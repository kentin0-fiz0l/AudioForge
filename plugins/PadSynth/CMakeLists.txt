# PadSynth Plugin
# Part of the AudioForge instrument pack

cmake_minimum_required(VERSION 3.15)

project(PadSynth VERSION 1.0.0)

# Add JUCE to the project
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

# Add AudioForge shared libraries
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../shared ${CMAKE_CURRENT_BINARY_DIR}/shared)

juce_add_plugin(PadSynth
    COMPANY_NAME "AudioForge"
    PLUGIN_MANUFACTURER_CODE AFrg
    PLUGIN_CODE PadS
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_NAME "PadSynth"
    PRODUCT_NAME "PadSynth"
    FORMATS VST3 AU Standalone
)

# Source files
target_sources(PadSynth PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/UnisonVoice.cpp
    Source/Voice.cpp
)

# Include directories
target_include_directories(PadSynth PRIVATE
    Source
)

# Link libraries
target_link_libraries(PadSynth PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp
    AudioForgeDSP
    AudioForgeSynth
    AudioForgePresets
)

# Compiler definitions
target_compile_definitions(PadSynth PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)

# Set C++17 standard
target_compile_features(PadSynth PRIVATE cxx_std_17)
