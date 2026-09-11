cmake_minimum_required(VERSION 3.22)
project(Polysynth VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

juce_add_plugin(Polysynth
    COMPANY_NAME "AudioForge"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    PLUGIN_MANUFACTURER_CODE Afge
    PLUGIN_CODE Poly
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "Polysynth"
    VST3_CATEGORIES "Instrument" "Synth"
    AU_MAIN_TYPE "kAudioUnitType_MusicDevice"
)

juce_generate_juce_header(Polysynth)

target_sources(Polysynth PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/PolysynthEngine.cpp
    Source/PolysynthVoice.cpp
    ../../midi/MIDILearnManager.cpp
    ../../shared/preset/PresetManager.cpp
    ../../shared/preset/PresetBrowser.cpp
)

target_compile_definitions(Polysynth PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(Polysynth PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_plugin_client
    juce::juce_dsp
)

juce_add_bundle_resources_directory(Polysynth Resources)
