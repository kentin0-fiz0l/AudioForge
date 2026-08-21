# AudioForge Shared DSP Library
# Static library containing reusable DSP components

cmake_minimum_required(VERSION 3.22)

# Define the library as INTERFACE (header-only)
# Since all our DSP code is in headers, we don't need a compiled library
add_library(AudioForgeDSP INTERFACE)

# Set include directories
target_include_directories(AudioForgeDSP INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link with JUCE (required for juce::jlimit, juce::MathConstants, etc.)
# We only need core JUCE headers, not the full framework
target_link_libraries(AudioForgeDSP INTERFACE
    juce::juce_core
    juce::juce_dsp
)

# Set C++17 requirement
target_compile_features(AudioForgeDSP INTERFACE cxx_std_17)

# Export the library for use by plugins
set_target_properties(AudioForgeDSP PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)
