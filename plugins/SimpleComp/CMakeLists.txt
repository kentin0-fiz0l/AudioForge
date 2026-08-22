cmake_minimum_required(VERSION 3.15)

project(SimpleComp VERSION 0.1.0)

# Add JUCE to the project
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

# Create the plugin target
juce_add_plugin(SimpleComp
    COMPANY_NAME "AudioForge"
    PLUGIN_MANUFACTURER_CODE Afrg
    PLUGIN_CODE SmCp
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "SimpleComp"
    PLUGIN_NAME "SimpleComp"
    DESCRIPTION "Basic dynamic range compressor"
    COMPANY_WEBSITE "https://audioforge.fluxstudio.art"
    COMPANY_EMAIL "support@audioforge.fluxstudio.art"
    COPY_PLUGIN_AFTER_BUILD TRUE)

# Add source files
target_sources(SimpleComp PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h)

# Link required JUCE modules
target_link_libraries(SimpleComp PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp)

# Add include directories for shared DSP library
target_include_directories(SimpleComp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../shared)

# Compiler definitions
target_compile_definitions(SimpleComp PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_DISPLAY_SPLASH_SCREEN=0)

# Set C++ standard
target_compile_features(SimpleComp PRIVATE cxx_std_17)
