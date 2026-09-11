cmake_minimum_required(VERSION 3.15)
project(TomSynth VERSION 1.0.0)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)
juce_add_plugin(TomSynth COMPANY_NAME "AudioForge" PLUGIN_MANUFACTURER_CODE AFrg PLUGIN_CODE TmSy IS_SYNTH TRUE NEEDS_MIDI_INPUT TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE COPY_PLUGIN_AFTER_BUILD TRUE PLUGIN_NAME "TomSynth" PRODUCT_NAME "TomSynth" FORMATS VST3 AU Standalone)
target_sources(TomSynth PRIVATE Source/PluginProcessor.cpp Source/PluginEditor.cpp Source/TomEngine.cpp Source/TomVoice.cpp
    ../../midi/MIDILearnManager.cpp
    ../../shared/preset/PresetManager.cpp
    ../../shared/preset/PresetBrowser.cpp
    ../../shared/preset/PresetBrowser.cpp)
target_include_directories(TomSynth PRIVATE Source)
target_link_libraries(TomSynth PRIVATE juce::juce_audio_utils juce::juce_audio_processors juce::juce_dsp)
target_compile_definitions(TomSynth PUBLIC JUCE_VST3_CAN_REPLACE_VST2=0 JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
target_compile_features(TomSynth PRIVATE cxx_std_17)
