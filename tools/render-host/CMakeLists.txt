cmake_minimum_required(VERSION 3.22)

project(RenderHost VERSION 0.1.0)

# Add JUCE to the project
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

juce_add_console_app(RenderHost PRODUCT_NAME "RenderHost")

target_sources(RenderHost PRIVATE main.cpp)

target_compile_definitions(RenderHost PRIVATE
    JUCE_PLUGINHOST_VST3=1
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0)

target_link_libraries(RenderHost PRIVATE
    juce::juce_audio_processors
    juce::juce_audio_formats
    juce::juce_recommended_config_flags)

target_compile_features(RenderHost PRIVATE cxx_std_17)
