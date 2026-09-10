cmake_minimum_required(VERSION 3.15)

project(AnalogKick VERSION 1.0.0)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

juce_add_plugin(AnalogKick
    COMPANY_NAME "AudioForge"
    PLUGIN_MANUFACTURER_CODE AFrg
    PLUGIN_CODE AnKk
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_NAME "AnalogKick"
    PRODUCT_NAME "AnalogKick"
    FORMATS VST3 AU Standalone
)

target_sources(AnalogKick PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/KickEngine.cpp
    Source/KickVoice.cpp
    ../../midi/MIDILearnManager.cpp
)

target_include_directories(AnalogKick PRIVATE Source)

target_link_libraries(AnalogKick PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp
)

target_compile_definitions(AnalogKick PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)

target_compile_features(AnalogKick PRIVATE cxx_std_17)
