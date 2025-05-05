cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "Runtime Library")
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.13 CACHE STRING "Build for 10.13")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(BUILD_STANDALONE "Build Standalone plugin format" ON) # Allow overriding from the command line

project(RipplerX VERSION 1.4.3)

add_compile_definitions(PROJECT_VERSION="${PROJECT_VERSION}")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_XCODE_GENERATE_SCHEME OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(libs/JUCE)

add_library(mts-esp-client STATIC libs/MTS-ESP/Client/libMTSClient.cpp)
target_include_directories(mts-esp-client PUBLIC libs/MTS-ESP/Client)
if (UNIX AND NOT APPLE)
     target_link_libraries(mts-esp-client PUBLIC dl)
endif ()


set(plugin_formats
    VST3
    LV2
    AU
)

if(BUILD_STANDALONE)
    list(APPEND plugin_formats Standalone)
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
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

source_group(Source
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PluginProcessor.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PluginProcessor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PluginEditor.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PluginEditor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Globals.h
)

source_group(Source\\ui
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/CustomLookAndFeel.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/CustomLookAndFeel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/Rotary.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/Rotary.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/Meter.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/Meter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/Pitch.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/Pitch.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/ui/About.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/About.cpp
)

source_group(Source\\dsp
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dsp/Envelope.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dsp/Envelope.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dsp/Filter.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dsp/Filter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dsp/Mallet.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dsp/Mallet.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dsp/Resonator.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dsp/Resonator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dsp/Partial.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dsp/Partial.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dsp/Waveguide.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dsp/Waveguide.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dsp/Noise.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dsp/Noise.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dsp/Voice.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dsp/Voice.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dsp/Comb.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dsp/Limiter.h
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
        mts-esp-client
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

file(GLOB_RECURSE res "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.*")
juce_add_binary_data(${PROJECT_NAME}_res SOURCES ${res})
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_res)

if(APPLE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC JUCE_AU=1)
endif()
