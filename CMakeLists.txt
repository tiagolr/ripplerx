cmake_minimum_required(VERSION 3.22.1)

# Change to your project name
project(RipplerX VERSION 1.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_XCODE_GENERATE_SCHEME OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "Runtime Library")
endif()

add_subdirectory(libs/JUCE)

set(plugin_formats
    # Standalone
    VST3
)
if(APPLE)
    list(APPEND plugin_formats AU)
elseif(UNIX AND NOT APPLE)
    list(APPEND plugin_formats LV2)
endif()

juce_add_plugin(${PROJECT_NAME}
        COMPANY_NAME Tilr
        IS_SYNTH TRUE
        NEEDS_MIDI_INPUT TRUE
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS FALSE
        JUCE_VST3_CAN_REPLACE_VST2 FALSE
        COPY_PLUGIN_AFTER_BUILD TRUE
        PLUGIN_MANUFACTURER_CODE Tilr
        PLUGIN_CODE Ripx
        FORMATS ${plugin_formats}
        PRODUCT_NAME "RipplerX"
        BUNDLE_ID "com.tilr.ripplerx"
        LV2URI "https://github.com/tiagolr/ripplerx"
)

file(GLOB_RECURSE src
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
)

source_group(Source
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/PluginProcessor.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/PluginProcessor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/PluginEditor.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/PluginEditor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Globals.h
)

source_group(Source\\ui
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/ui/CustomLookAndFeel.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/ui/CustomLookAndFeel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/ui/Rotary.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/ui/Rotary.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/ui/Meter.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/ui/Meter.cpp
)

source_group(Source\\dsp
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/dsp/Envelope.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/dsp/Envelope.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/dsp/Filter.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/dsp/Filter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/dsp/Mallet.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/dsp/Mallet.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/dsp/Resonator.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/dsp/Resonator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/dsp/Partial.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/dsp/Partial.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/dsp/Waveguide.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/dsp/Waveguide.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/dsp/Noise.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/dsp/Noise.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/dsp/Voice.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/dsp/Voice.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/dsp/Comb.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/dsp/Limiter.h
)

# Make the SourceFiles buildable
target_sources(${PROJECT_NAME} PRIVATE ${src})

# These are some toggleable options from the JUCE CMake API
target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# JUCE libraries to bring into our project
juce_generate_juce_header(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        juce::juce_core
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

file(GLOB_RECURSE res "${CMAKE_CURRENT_SOURCE_DIR}/Resources/*.*")
juce_add_binary_data(${PROJECT_NAME}_res SOURCES ${res})
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_res)

# message(STATUS "Resources found: ${res}")

if(APPLE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC JUCE_AU=1)

    # This option is relevant to x86_64 target architecture,
    # but I do not see how to detect the target architecture
    # during CMake generation time. So this will cause warnings when
    # compiling for arm64.
    target_compile_options(${PROJECT_NAME} PRIVATE "-mfma")
endif()
