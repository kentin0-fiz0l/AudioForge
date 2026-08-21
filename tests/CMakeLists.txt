cmake_minimum_required(VERSION 3.15)

project(AudioForgeTests VERSION 1.0.0)

# Add JUCE to the project
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

# Create test runner executable
add_executable(TestRunner
    TestRunner.cpp
    DSP/GainProcessingTests.cpp
    DSP/PanningTests.cpp
    DSP/ParameterSmoothingTests.cpp
    Plugins/SimpleGainTests.cpp
    Plugins/PanUtilTests.cpp
    Integration/PluginIntegrationTests.cpp)

# Link JUCE libraries
target_link_libraries(TestRunner PRIVATE
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_dsp)

# Add include directories for plugin sources and DSP library
target_include_directories(TestRunner PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/SimpleGain/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/PanUtil/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/../shared)  # For shared DSP library

# Add plugin source files directly for testing
# Note: We need to compile the processor files but exclude createPluginFilter() to avoid duplicates
# We'll handle this by defining a macro to disable the factory function
target_sources(TestRunner PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/SimpleGain/Source/PluginProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/SimpleGain/Source/PluginEditor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/PanUtil/Source/PluginProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/PanUtil/Source/PluginEditor.cpp)

# Compiler definitions
target_compile_definitions(TestRunner PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_STANDALONE_APPLICATION=1
    JucePlugin_Name="SimpleGain"
    AUDIOFORGE_TESTS=1)

# Set C++ standard
target_compile_features(TestRunner PRIVATE cxx_std_17)
