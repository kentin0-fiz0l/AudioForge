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
    DSP/WaveShapingTests.cpp
    DSP/SaturationAlgorithmsTests.cpp
    DSP/MultiModeFilterTests.cpp
    Plugins/SimpleGainTests.cpp
    Plugins/PanUtilTests.cpp
    Plugins/BasicSynthTests.cpp
    Plugins/CleanDelayTests.cpp
    Plugins/SimpleEQTests.cpp
    Plugins/SimpleCompTests.cpp
    Plugins/WaveShaperTests.cpp
    Plugins/SaturationTests.cpp
    Plugins/ChorusFlangerTests.cpp
    Plugins/ReverbTests.cpp
    Plugins/AIProducerTests.cpp
    Plugins/BrassEngineTests.cpp
    Plugins/GateEngineTests.cpp
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/SimpleEQ/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/SimpleComp/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/WaveShaper/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/Saturation/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/ChorusFlanger/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/Reverb/Source
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/BasicSynth/Source/OscillatorBank.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/BasicSynth/Source/NoiseGenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/BasicSynth/Source/LFO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/BasicSynth/Source/ModulationMatrix.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/BasicSynth/Source/Chorus.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/BasicSynth/Source/Reverb.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/BasicSynth/Source/Saturation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/BasicSynth/Source/EffectsChain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../shared/synth/ADSREnvelope.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../shared/synth/MultiModeFilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/CleanDelay/Source/PluginProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/CleanDelay/Source/PluginEditor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/SimpleEQ/Source/PluginProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/SimpleEQ/Source/PluginEditor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/SimpleComp/Source/PluginProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/SimpleComp/Source/PluginEditor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/WaveShaper/Source/PluginProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/WaveShaper/Source/PluginEditor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/Saturation/Source/PluginProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/Saturation/Source/PluginEditor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/ChorusFlanger/Source/PluginProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/ChorusFlanger/Source/PluginEditor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/Reverb/Source/PluginProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/Reverb/Source/PluginEditor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/AIProducer/Source/PluginProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/AIProducer/Source/PluginEditor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/AIProducer/Source/MIDIGenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/AIProducer/Source/MusicTheory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/AIProducer/Source/AIClient.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/BrassSection/Source/BrassEngine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/Gate/Source/GateEngine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../shared/dsp/EnvelopeFollower.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../shared/dsp/Sidechain.cpp)

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

# Tests construct processors as local variables, and some are large:
# CleanDelay holds about 1.5 MB of delay buffers inline. Windows gives the main
# thread a 1 MB stack by default where macOS and Linux give 8 MB, so match them.
if(MSVC)
    target_link_options(TestRunner PRIVATE /STACK:8388608)
endif()

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

# Branch coverage (updated to non-deprecated option)
branch_coverage = 1

# Exclude patterns
# These patterns exclude JUCE framework, test code, and system headers
# from coverage calculations to focus on actual plugin code
geninfo_auto_base = 1
")
endif()
