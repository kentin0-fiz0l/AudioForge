cmake_minimum_required(VERSION 3.22)
project(PhaserFlanger VERSION 1.0.0)
add_subdirectory(../../JUCE JUCE)
juce_add_plugin(PhaserFlanger
    COMPANY_NAME "AudioForge"
    PLUGIN_MANUFACTURER_CODE Afge
    PLUGIN_CODE Phfl
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "PhaserFlanger"
    VST3_CATEGORIES Fx Modulation
)
target_sources(PhaserFlanger PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/ModulationEngine.cpp
    ../../midi/MIDILearnManager.cpp
    ../../shared/preset/PresetManager.cpp
    ../../shared/preset/PresetBrowser.cpp
)
target_compile_definitions(PhaserFlanger PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)
target_link_libraries(PhaserFlanger PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
)
