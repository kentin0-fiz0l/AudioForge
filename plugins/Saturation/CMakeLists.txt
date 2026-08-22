cmake_minimum_required(VERSION 3.15)

project(Saturation VERSION 0.1.0)

# Add JUCE to the project
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

# Create the plugin target
juce_add_plugin(Saturation
    COMPANY_NAME "AudioForge"
    PLUGIN_MANUFACTURER_CODE Afrg
    PLUGIN_CODE Satu
    FORMATS VST3 Standalone
    PRODUCT_NAME "Saturation"
    PLUGIN_NAME "Saturation"
    DESCRIPTION "Multi-mode analog saturation and harmonic enhancement"
    COMPANY_WEBSITE "https://audioforge.fluxstudio.art"
    COMPANY_EMAIL "support@audioforge.fluxstudio.art"
    COPY_PLUGIN_AFTER_BUILD TRUE)

# Add source files
target_sources(Saturation PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h)

# Link required JUCE modules
target_link_libraries(Saturation PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp)

# Add include directories for shared DSP library
target_include_directories(Saturation PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../shared)

# Compiler definitions
target_compile_definitions(Saturation PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_DISPLAY_SPLASH_SCREEN=0)

# Set C++ standard
target_compile_features(Saturation PRIVATE cxx_std_17)
