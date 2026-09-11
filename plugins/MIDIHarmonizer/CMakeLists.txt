cmake_minimum_required(VERSION 3.22)
project(MIDIHarmonizer VERSION 1.0.0)

add_subdirectory(../../JUCE JUCE)

juce_add_plugin(MIDIHarmonizer
    COMPANY_NAME "AudioForge"
    PLUGIN_MANUFACTURER_CODE Afge
    PLUGIN_CODE Mhrm
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "MIDI Harmonizer"
    VST3_CATEGORIES Fx
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT TRUE
)

target_sources(MIDIHarmonizer PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/HarmonizerEngine.cpp
    ../../midi/MIDILearnManager.cpp
    ../../shared/preset/PresetManager.cpp
    ../../shared/preset/PresetBrowser.cpp
)

target_compile_definitions(MIDIHarmonizer PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(MIDIHarmonizer PRIVATE
    juce::juce_audio_utils
)
