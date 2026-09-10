cmake_minimum_required(VERSION 3.15)
project(HiHat VERSION 1.0.0)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)
juce_add_plugin(HiHat
    COMPANY_NAME "AudioForge" PLUGIN_MANUFACTURER_CODE AFrg PLUGIN_CODE HiHt
    IS_SYNTH TRUE NEEDS_MIDI_INPUT TRUE EDITOR_WANTS_KEYBOARD_FOCUS FALSE COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_NAME "HiHat" PRODUCT_NAME "HiHat" FORMATS VST3 AU Standalone)
target_sources(HiHat PRIVATE Source/PluginProcessor.cpp Source/PluginEditor.cpp Source/HiHatEngine.cpp Source/HiHatVoice.cpp)
target_include_directories(HiHat PRIVATE Source)
target_link_libraries(HiHat PRIVATE juce::juce_audio_utils juce::juce_audio_processors juce::juce_dsp)
target_compile_definitions(HiHat PUBLIC JUCE_VST3_CAN_REPLACE_VST2=0 JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
target_compile_features(HiHat PRIVATE cxx_std_17)
