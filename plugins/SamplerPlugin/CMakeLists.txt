cmake_minimum_required(VERSION 3.15)

project(SamplerPlugin VERSION 1.0.0)

# Add JUCE to the project
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

# Add AudioForge shared DSP library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../shared ${CMAKE_CURRENT_BINARY_DIR}/shared)

# Create the plugin
juce_add_plugin(SamplerPlugin
    COMPANY_NAME "AudioForge"
    PLUGIN_MANUFACTURER_CODE AFrg
    PLUGIN_CODE Smpl
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "Sampler"
    PLUGIN_NAME "Sampler"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE)

# Add source files
target_sources(SamplerPlugin PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/Sample.cpp
    Source/SampleLibrary.cpp
    Source/SamplerVoice.cpp
    Source/KeyZone.cpp
    Source/ZoneEditor.cpp
    Source/WaveformDisplay.cpp
    Source/VelocityLayerEditor.cpp
    Source/SampleBrowser.cpp)

# Compiler definitions
target_compile_definitions(SamplerPlugin PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_DISPLAY_SPLASH_SCREEN=0)

# Link JUCE libraries and shared libraries
target_link_libraries(SamplerPlugin PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_formats
    juce::juce_dsp
    AudioForgeDSP
    AudioForgeSynth
    AudioForgePresets)

# Set C++ standard
target_compile_features(SamplerPlugin PRIVATE cxx_std_17)

# Tests
if(BUILD_TESTING)
    enable_testing()

    add_executable(SamplerPluginTests
        Source/SamplerTests.cpp
        Source/Sample.cpp
        Source/SampleLibrary.cpp
        Source/KeyZone.cpp
        Source/SamplerVoice.cpp
        Source/PluginProcessor.cpp)

    target_link_libraries(SamplerPluginTests PRIVATE
        juce::juce_audio_processors
        juce::juce_audio_formats
        juce::juce_dsp
        AudioForgeDSP
        AudioForgeSynth
        AudioForgePresets)

    target_compile_features(SamplerPluginTests PRIVATE cxx_std_17)

    add_test(NAME SamplerPluginTests COMMAND SamplerPluginTests)
endif()
