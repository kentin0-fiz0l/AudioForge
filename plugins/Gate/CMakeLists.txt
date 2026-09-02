# Gate Plugin - Professional Noise Gate & Expander
# AudioForge v1.7.0

cmake_minimum_required(VERSION 3.15)

project(Gate VERSION 1.0.0)

# Add JUCE
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

# Plugin configuration
juce_add_plugin(Gate
    COMPANY_NAME "AudioForge"
    PLUGIN_MANUFACTURER_CODE Afge
    PLUGIN_CODE Gate
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "Gate"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_CATEGORIES "Fx" "Dynamics"
    AU_MAIN_TYPE "kAudioUnitType_Effect"
)

# Source files
target_sources(Gate PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/GateEngine.cpp
    ../../shared/dsp/EnvelopeFollower.cpp
    ../../shared/dsp/Sidechain.cpp
)

# Include directories
target_include_directories(Gate PRIVATE
    Source
    ${CMAKE_SOURCE_DIR}/shared/utils
    ${CMAKE_SOURCE_DIR}/shared/dsp
)

# JUCE modules
target_compile_definitions(Gate PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
)

# Link JUCE libraries
target_link_libraries(Gate PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
)

# Compiler settings
target_compile_features(Gate PRIVATE cxx_std_17)

# Install targets
install(TARGETS Gate
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
