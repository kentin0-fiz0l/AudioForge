cmake_minimum_required(VERSION 3.15)
project(ConvolutionReverb VERSION 1.0.0)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)
juce_add_plugin(ConvolutionReverb COMPANY_NAME "AudioForge" PLUGIN_MANUFACTURER_CODE AFrg PLUGIN_CODE CvRv IS_SYNTH FALSE NEEDS_MIDI_INPUT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE COPY_PLUGIN_AFTER_BUILD TRUE PLUGIN_NAME "ConvolutionReverb" PRODUCT_NAME "ConvolutionReverb" FORMATS VST3 AU Standalone)
target_sources(ConvolutionReverb PRIVATE Source/PluginProcessor.cpp Source/PluginEditor.cpp
    ../../midi/MIDILearnManager.cpp
    ../../shared/preset/PresetManager.cpp
    ../../shared/preset/PresetBrowser.cpp)
target_include_directories(ConvolutionReverb PRIVATE Source)
target_link_libraries(ConvolutionReverb PRIVATE juce::juce_audio_utils juce::juce_audio_processors juce::juce_audio_formats juce::juce_dsp)
target_compile_definitions(ConvolutionReverb PUBLIC JUCE_VST3_CAN_REPLACE_VST2=0 JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
target_compile_features(ConvolutionReverb PRIVATE cxx_std_17)
