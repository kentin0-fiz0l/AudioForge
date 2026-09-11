cmake_minimum_required(VERSION 3.15)
project(StereoChorus VERSION 1.0.0)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)
juce_add_plugin(StereoChorus COMPANY_NAME "AudioForge" PLUGIN_MANUFACTURER_CODE AFrg PLUGIN_CODE StCh IS_SYNTH FALSE NEEDS_MIDI_INPUT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE COPY_PLUGIN_AFTER_BUILD TRUE PLUGIN_NAME "StereoChorus" PRODUCT_NAME "StereoChorus" FORMATS VST3 AU Standalone)
target_sources(StereoChorus PRIVATE Source/PluginProcessor.cpp Source/PluginEditor.cpp
    ../../midi/MIDILearnManager.cpp)
    ../../shared/preset/PresetManager.cpp
    ../../shared/preset/PresetBrowser.cpp
target_include_directories(StereoChorus PRIVATE Source)
target_link_libraries(StereoChorus PRIVATE juce::juce_audio_utils juce::juce_audio_processors juce::juce_dsp)
target_compile_definitions(StereoChorus PUBLIC JUCE_VST3_CAN_REPLACE_VST2=0 JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
target_compile_features(StereoChorus PRIVATE cxx_std_17)
