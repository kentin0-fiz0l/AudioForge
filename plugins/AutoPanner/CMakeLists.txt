cmake_minimum_required(VERSION 3.22)
project(AutoPanner VERSION 1.0.0)
add_subdirectory(../../JUCE JUCE)
juce_add_plugin(AutoPanner
    COMPANY_NAME "AudioForge"
    PLUGIN_MANUFACTURER_CODE Afge
    PLUGIN_CODE Apan
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "AutoPanner"
    VST3_CATEGORIES Fx Modulation
)
target_sources(AutoPanner PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/PannerEngine.cpp
    ../../midi/MIDILearnManager.cpp
)
target_compile_definitions(AutoPanner PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)
target_link_libraries(AutoPanner PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
)
