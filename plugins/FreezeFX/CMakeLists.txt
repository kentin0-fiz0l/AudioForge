cmake_minimum_required(VERSION 3.15)

project(FreezeFX VERSION 1.0.0)

# Add JUCE to the project
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

# Add JUCE plugin target
juce_add_plugin(FreezeFX
    COMPANY_NAME "AudioForge"
    PLUGIN_MANUFACTURER_CODE Afge
    PLUGIN_CODE Frez
    FORMATS VST3 AU
    PRODUCT_NAME "FreezeFX"
    PLUGIN_NAME "FreezeFX"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_CATEGORIES "Fx" "Reverb"
    AU_MAIN_TYPE "kAudioUnitType_Effect"
)

# Source files
target_sources(FreezeFX PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/SpectralProcessor.cpp
    Source/FrozenSpectrum.cpp
    Source/PhaseEvolver.cpp
)

# Link libraries
target_link_libraries(FreezeFX
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Compile definitions
target_compile_definitions(FreezeFX
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# C++ standard
set_target_properties(FreezeFX PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)
