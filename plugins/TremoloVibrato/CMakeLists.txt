cmake_minimum_required(VERSION 3.22)
project(TremoloVibrato VERSION 1.0.0)
add_subdirectory(../../JUCE JUCE)
juce_add_plugin(TremoloVibrato
    COMPANY_NAME "AudioForge"
    PLUGIN_MANUFACTURER_CODE Afge
    PLUGIN_CODE Trvb
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "TremoloVibrato"
    VST3_CATEGORIES Fx Modulation
)
target_sources(TremoloVibrato PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/TremoloVibratoEngine.cpp
)
target_compile_definitions(TremoloVibrato PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)
target_link_libraries(TremoloVibrato PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
)
