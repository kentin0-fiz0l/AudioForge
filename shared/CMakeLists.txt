# AudioForge Shared Libraries
# Contains reusable DSP components, synth utilities, and preset management

cmake_minimum_required(VERSION 3.22)

# ============================================================================
# AudioForgeDSP - Header-only DSP library
# ============================================================================
add_library(AudioForgeDSP INTERFACE)

target_include_directories(AudioForgeDSP INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(AudioForgeDSP INTERFACE
    juce::juce_core
    juce::juce_dsp
)

target_compile_features(AudioForgeDSP INTERFACE cxx_std_17)

# ============================================================================
# AudioForgeSynth - Synthesizer utilities (ADSR, filters, voice management)
# ============================================================================
add_library(AudioForgeSynth STATIC
    synth/ADSREnvelope.cpp
    synth/MultiModeFilter.cpp
)

target_include_directories(AudioForgeSynth PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(AudioForgeSynth PUBLIC
    juce::juce_core
    juce::juce_dsp
    AudioForgeDSP
)

target_compile_features(AudioForgeSynth PUBLIC cxx_std_17)

set_target_properties(AudioForgeSynth PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    POSITION_INDEPENDENT_CODE ON
)

# ============================================================================
# AudioForgePresets - Preset management system
# ============================================================================
add_library(AudioForgePresets STATIC
    presets/PresetManager.cpp
)

target_include_directories(AudioForgePresets PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(AudioForgePresets PUBLIC
    juce::juce_core
    juce::juce_data_structures
    juce::juce_audio_processors
)

target_compile_features(AudioForgePresets PUBLIC cxx_std_17)

set_target_properties(AudioForgePresets PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    POSITION_INDEPENDENT_CODE ON
)
