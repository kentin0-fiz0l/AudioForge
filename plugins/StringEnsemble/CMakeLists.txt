cmake_minimum_required(VERSION 3.15)

project(StringEnsemble VERSION 1.0.0)

# Add JUCE to the project
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

# Add AudioForge shared libraries
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../shared ${CMAKE_CURRENT_BINARY_DIR}/shared)

# Create the plugin
juce_add_plugin(StringEnsemble
    COMPANY_NAME "AudioForge"
    PLUGIN_MANUFACTURER_CODE AFrg
    PLUGIN_CODE AStr
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "StringEnsemble"
    PLUGIN_NAME "AudioForge - StringEnsemble"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_CATEGORIES "Instrument" "Synth"
    AU_MAIN_TYPE "kAudioUnitType_MusicDevice")

# Add source files
target_sources(StringEnsemble PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/StringVoice.cpp
    Source/StringEngine.cpp
    ../../midi/MIDILearnManager.cpp
    ../../shared/preset/PresetManager.cpp
    ../../shared/preset/PresetBrowser.cpp
    ../../shared/preset/PresetBrowser.cpp)

# Compiler definitions
target_compile_definitions(StringEnsemble PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_MODAL_LOOPS_PERMITTED=1)

# Link JUCE libraries and shared DSP library
target_link_libraries(StringEnsemble PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    AudioForgeDSP
    AudioForgeSynth
    AudioForgePresets)

# Set C++ standard
target_compile_features(StringEnsemble PRIVATE cxx_std_17)
