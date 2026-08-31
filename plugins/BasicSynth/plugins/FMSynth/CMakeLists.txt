# FMSynth Plugin
# Part of the AudioForge instrument pack

juce_add_plugin(FMSynth
    COMPANY_NAME "AudioForge"
    PLUGIN_MANUFACTURER_CODE AFrg
    PLUGIN_CODE FMSy
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_NAME "FMSynth"
    PRODUCT_NAME "FMSynth"
    FORMATS VST3 AU Standalone
)

# Source files
target_sources(FMSynth PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)

# Include directories
target_include_directories(FMSynth PRIVATE
    Source
)

# Link libraries
target_link_libraries(FMSynth PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp
    AudioForgeDSP
    AudioForgeSynth
    AudioForgePresets
)

# Compiler definitions
target_compile_definitions(FMSynth PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)

# Set C++17 standard
target_compile_features(FMSynth PRIVATE cxx_std_17)
