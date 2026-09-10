cmake_minimum_required(VERSION 3.15)

project(AcousticBass VERSION 1.0.0)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../shared ${CMAKE_CURRENT_BINARY_DIR}/shared)

juce_add_plugin(AcousticBass
    COMPANY_NAME "AudioForge"
    PLUGIN_MANUFACTURER_CODE AFrg
    PLUGIN_CODE ABas
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "AcousticBass"
    PLUGIN_NAME "AudioForge - AcousticBass"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_CATEGORIES "Instrument" "Synth"
    AU_MAIN_TYPE "kAudioUnitType_MusicDevice")

target_sources(AcousticBass PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/BassVoice.cpp
    Source/BassEngine.cpp
    ../../midi/MIDILearnManager.cpp)

target_compile_definitions(AcousticBass PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_MODAL_LOOPS_PERMITTED=1)

target_link_libraries(AcousticBass PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    AudioForgeDSP
    AudioForgeSynth
    AudioForgePresets)

target_compile_features(AcousticBass PRIVATE cxx_std_17)
