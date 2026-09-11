cmake_minimum_required(VERSION 3.15)

project(SnareSynth VERSION 1.0.0)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

juce_add_plugin(SnareSynth
    COMPANY_NAME "AudioForge"
    PLUGIN_MANUFACTURER_CODE AFrg
    PLUGIN_CODE SnSy
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_NAME "SnareSynth"
    PRODUCT_NAME "SnareSynth"
    FORMATS VST3 AU Standalone
)

target_sources(SnareSynth PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/SnareEngine.cpp
    Source/SnareVoice.cpp
    ../../midi/MIDILearnManager.cpp
    ../../shared/preset/PresetManager.cpp
    ../../shared/preset/PresetBrowser.cpp
)

target_include_directories(SnareSynth PRIVATE Source)

target_link_libraries(SnareSynth PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp
)

target_compile_definitions(SnareSynth PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)

target_compile_features(SnareSynth PRIVATE cxx_std_17)
