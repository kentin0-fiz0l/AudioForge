cmake_minimum_required(VERSION 3.15)
project(ShimmerReverb VERSION 1.0.0)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)
juce_add_plugin(ShimmerReverb COMPANY_NAME "AudioForge" PLUGIN_MANUFACTURER_CODE AFrg PLUGIN_CODE ShRv IS_SYNTH FALSE NEEDS_MIDI_INPUT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE COPY_PLUGIN_AFTER_BUILD TRUE PLUGIN_NAME "ShimmerReverb" PRODUCT_NAME "ShimmerReverb" FORMATS VST3 AU Standalone)
target_sources(ShimmerReverb PRIVATE Source/PluginProcessor.cpp Source/PluginEditor.cpp Source/ShimmerEngine.cpp ../../midi/MIDILearnManager.cpp
    ../../shared/preset/PresetManager.cpp
    ../../shared/preset/PresetBrowser.cpp
    ../../shared/preset/PresetBrowser.cpp)
target_include_directories(ShimmerReverb PRIVATE Source)
target_link_libraries(ShimmerReverb PRIVATE juce::juce_audio_utils juce::juce_audio_processors juce::juce_dsp)
target_compile_definitions(ShimmerReverb PUBLIC JUCE_VST3_CAN_REPLACE_VST2=0 JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
target_compile_features(ShimmerReverb PRIVATE cxx_std_17)
