cmake_minimum_required(VERSION 3.15)

project(AudioForgeTests VERSION 1.0.0)

# Coverage support option
option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)

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
    Plugins/BasicSynthTests.cpp
    Plugins/CleanDelayTests.cpp
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/BasicSynth/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/CleanDelay/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/../shared)  # For shared DSP library

# Add plugin source files directly for testing
# Note: We need to compile the processor files but exclude createPluginFilter() to avoid duplicates
# We'll handle this by defining a macro to disable the factory function
target_sources(TestRunner PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/SimpleGain/Source/PluginProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/SimpleGain/Source/PluginEditor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/PanUtil/Source/PluginProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/PanUtil/Source/PluginEditor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/BasicSynth/Source/PluginProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/BasicSynth/Source/PluginEditor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/BasicSynth/Source/Voice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/BasicSynth/Source/Oscillator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/CleanDelay/Source/PluginProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/CleanDelay/Source/PluginEditor.cpp)

# Compiler definitions
target_compile_definitions(TestRunner PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_STANDALONE_APPLICATION=1
    JucePlugin_Name="AudioForge_TestRunner"
    AUDIOFORGE_TESTS=1)

# Set C++ standard
target_compile_features(TestRunner PRIVATE cxx_std_17)

# Code coverage configuration
if(ENABLE_COVERAGE)
    message(STATUS "Code coverage enabled")

    # Add coverage flags for GCC/Clang
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(TestRunner PRIVATE
            --coverage
            -fprofile-arcs
            -ftest-coverage
            -O0
            -g)

        target_link_options(TestRunner PRIVATE
            --coverage)

        # Add gcov library for Clang
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            target_link_libraries(TestRunner PRIVATE
                --coverage)
        endif()
    else()
        message(WARNING "Code coverage only supported for GCC and Clang compilers")
    endif()
endif()

# Create .lcovrc configuration file for coverage exclusions
if(ENABLE_COVERAGE)
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/.lcovrc
"# LCOV configuration for AudioForge
# Exclude external code from coverage reports

# Branch coverage
lcov_branch_coverage = 1

# Exclude patterns
# These patterns exclude JUCE framework, test code, and system headers
# from coverage calculations to focus on actual plugin code
")
endif()
